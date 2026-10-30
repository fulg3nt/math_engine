#ifndef PARSER_H
#define PARSER_H

#include "instr.h"
#include "lexer.h"
#include <optional>
#include <vector>

namespace MathEngine
{

class Parser
{
  public:
    static std::vector<Instruction> compile(std::string src,
                                            std::vector<std::string> vars);

  private:
    explicit Parser(std::string src, std::vector<std::string> vars);

    Lexer m_lexer;
    Token m_currentToken;

    std::vector<Instruction> m_instrs;

    std::vector<std::string> m_vars;
    std::optional<size_t> getVariableIndex(const std::string &name);

    template <typename... Args> bool check(Args... types) const;
    template <typename... Args> std::optional<Token> match(Args... types);

    Token consume(TokenType type, const std::string &errorMessage);
    Token consume();

    void parseFuncCall(size_t index, TokenType openingType, SourceRange range);
    void parseEquation();
    void parseExpression();
    void parseTerm();
    void parseFactor();
    void parsePrimary();
};

} // namespace MathEngine

#endif // PARSER_H
