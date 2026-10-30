#include "parser.h"
#include "eval.h"
#include "input_error.h"
#include <stdexcept>

/*
<equation> ::= <expression> EQUALS <expression> END
<expression> ::= <term> { (PLUS | MINUS) <term> }
<term> ::= <factor> { (STAR | SLASH) <factor> }
<factor> ::= ( PLUS | MINUS ) <factor> | <primary> [ CARET <factor> ]
<primary> ::= NUMBER |
              IDENTIFIER [ L_PAR <arg-list> R_PAR | L_BRK <arg-list> R_BRK ] |
              L_PAR <expression> R_PAR | L_BRK <expression> R_BRK
<arg-list> ::= <expression> { COMMA <expression> }
*/

namespace MathEngine
{
using std::optional;
using std::pair;
using std::string;
using std::vector;

vector<Instruction> Parser::compile(string src, vector<string> vars)
{
    Parser parser(std::move(src), std::move(vars));

    // parser.parseEquation();

    // expression test
    parser.parseExpression();
    if (!parser.check(TokenType::END)) {
        throw InputError("unexpected token: '" + parser.m_currentToken.value + "'",
                         parser.m_currentToken.toSourceRange());
    }

    return std::move(parser.m_instrs);
}

Parser::Parser(string src, vector<string> vars)
    : m_lexer(std::move(src)), m_currentToken(m_lexer.nextToken()),
      m_vars(std::move(vars))
{
    if (check(TokenType::END)) {
        throw InputError("source is empty");
    }
}

optional<size_t> Parser::getVariableIndex(const string &name)
{
    for (size_t i = 0; i < m_vars.size(); i++) {
        if (m_vars[i] == name) return i;
    }
    return std::nullopt;
}

template <typename... Args> bool Parser::check(Args... types) const
{
    return ((m_currentToken.type == types) || ...);
}

template <typename... Args> optional<Token> Parser::match(Args... types)
{
    return check(types...) ? std::make_optional(consume()) : std::nullopt;
}

Token Parser::consume()
{
    Token prevToken = m_currentToken;
    m_currentToken = m_lexer.nextToken();
    return prevToken;
}

Token Parser::consume(TokenType type, const string &errorMessage)
{
    if (check(type)) return consume();
    throw InputError(errorMessage, m_currentToken.toSourceRange());
}

void Parser::parseEquation()
{
    parseExpression();
    consume(TokenType::EQUALS, "expected '='");
    parseExpression();
    if (!check(TokenType::END)) {
        throw InputError("unexpected token: '" + m_currentToken.value + "'",
                         m_currentToken.toSourceRange());
    }
    m_instrs.emplace_back(OpCode::SUB);
}

static OpCode binOpToOpCode(TokenType type)
{
    switch (type) {
        case TokenType::PLUS: return OpCode::ADD;
        case TokenType::MINUS: return OpCode::SUB;
        case TokenType::STAR: return OpCode::MUL;
        case TokenType::SLASH: return OpCode::DIV;
        default: throw std::logic_error("binOpToOpCode");
    }
}

void Parser::parseExpression()
{
    parseTerm();
    while (auto op = match(TokenType::PLUS, TokenType::MINUS)) {
        parseTerm();
        m_instrs.emplace_back(binOpToOpCode(op->type));
    }
}

void Parser::parseTerm()
{
    parseFactor();
    while (auto op = match(TokenType::STAR, TokenType::SLASH)) {
        parseFactor();
        m_instrs.emplace_back(binOpToOpCode(op->type));
    }
}

void Parser::parseFactor()
{
    if (auto op = match(TokenType::MINUS, TokenType::PLUS)) {
        parseFactor();
        if (op->type == TokenType::MINUS) {
            m_instrs.emplace_back(OpCode::NEG);
        }
        return;
    }
    parsePrimary();
    if (match(TokenType::CARET)) {
        parseFactor();
        m_instrs.emplace_back(OpCode::POW);
    }
}

static pair<TokenType, string> getMatchingPair(TokenType opening)
{
    switch (opening) {
        case TokenType::L_PAR: return {TokenType::R_PAR, "expected ')'"};
        case TokenType::L_BRK: return {TokenType::R_BRK, "expected ']'"};
        default: throw std::logic_error("getMatchingPair");
    }
}

void Parser::parsePrimary()
{
    if (auto num = match(TokenType::NUMBER)) {
        m_instrs.emplace_back(OpCode::PUSH_NUM, std::stod(num->value));

    } else if (auto id = match(TokenType::IDENTIFIER)) {
        string &name = id->value;
        auto opening = match(TokenType::L_PAR, TokenType::L_BRK);

        if (auto idx = Evaluator::getFunctionIndex(name.c_str())) {
            if (opening) {
                parseFuncCall(*idx, opening->type, id->toSourceRange());
            } else {
                throw InputError("function '" + name +
                                     "' must be called with '(...)' or '[...]'",
                                 id->toSourceRange());
            }
        } else if (auto idx = Evaluator::getConstantIndex(name.c_str())) {
            if (opening) {
                throw InputError("'" + name + "' (constant) is not callable",
                                 opening->toSourceRange());
            }
            m_instrs.emplace_back(OpCode::PUSH_CONST, *idx);
        } else if (auto idx = getVariableIndex(name)) {
            if (opening) {
                throw InputError("'" + name + "' (variable) is not callable",
                                 opening->toSourceRange());
            }
            m_instrs.emplace_back(OpCode::PUSH_VAR, *idx);
        } else {
            throw InputError("'" + name + "' is not defined", id->toSourceRange());
        }
    } else if (auto opening = match(TokenType::L_PAR, TokenType::L_BRK)) {
        auto [expected, message] = getMatchingPair(opening->type);
        parseExpression();
        consume(expected, message);
    } else if (check(TokenType::END)) {
        throw InputError("unexpected end of expression", m_currentToken.toSourceRange());
    } else {
        throw InputError("unexpected token: '" + m_currentToken.value + "'",
                         m_currentToken.toSourceRange());
    }
}

void Parser::parseFuncCall(size_t index, TokenType openingType, SourceRange range)
{
    size_t argCount = 0;
    auto [expected, message] = getMatchingPair(openingType);

    if (!check(expected)) {
        do {
            parseExpression();
            argCount += 1;
        } while (match(TokenType::COMMA));
    }
    consume(expected, message);

    size_t requiredArgCount = Evaluator::globalFunctions[index].argCount;

    if (argCount != requiredArgCount) {
        throw InputError(
            "function '" + string(Evaluator::globalFunctions[index].name) +
                "' requires " + std::to_string(requiredArgCount) +
                " argument(s), but got " + std::to_string(argCount), range);
    }

    m_instrs.emplace_back(OpCode::CALL_FUNC, index);
}

} // namespace MathEngine
