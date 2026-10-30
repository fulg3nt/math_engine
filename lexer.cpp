#include "lexer.h"
#include <cctype>
#include <stdexcept>
#include <vector>

namespace MathEngine
{

Lexer::Lexer(std::string src) : m_src(std::move(src)) {}

char Lexer::peek(size_t offset = 0) const
{
    if ((m_pos + offset) >= m_src.length()) return '\0';
    return m_src[m_pos + offset];
}

char Lexer::consume()
{
    if (m_pos >= m_src.length()) return '\0';
    return m_src[m_pos++];
}

Token Lexer::nextToken()
{
    while (std::isspace(peek())) {
        consume();
    }

    size_t startPos = m_pos;

    if (peek() == '\0') return {TokenType::END, "", startPos};

    if (std::isdigit(peek()) || (peek() == '.' && std::isdigit(peek(1)))) {
        std::string num;

        while (std::isdigit(peek())) {
            num += consume();
        }

        if (peek() == '.') {
            num += consume();

            while (std::isdigit(peek())) {
                num += consume();
            }
        }

        if (std::tolower(peek()) == 'e' &&
            (std::isdigit(peek(1)) ||
             ((peek(1) == '-' || peek(1) == '+') && std::isdigit(peek(2))))) {

            num += consume(); // e
            num += consume(); // +- or digit

            while (std::isdigit(peek())) {
                num += consume();
            }
        }

        return {TokenType::NUMBER, num, startPos};
    }

    if (std::isalpha(peek()) || peek() == '_') {
        std::string id;

        while (std::isalnum(peek()) || peek() == '_') {
            id += consume();
        }

        if (id == "inf") return {TokenType::NUMBER, id, startPos};

        return {TokenType::IDENTIFIER, id, startPos};
    }

    switch (char c = consume()) {
        case '+': return {TokenType::PLUS, "+", startPos};
        case '-': return {TokenType::MINUS, "-", startPos};
        case '*': return {TokenType::STAR, "*", startPos};
        case '/': return {TokenType::SLASH, "/", startPos};
        case '^': return {TokenType::CARET, "^", startPos};
        case '=': return {TokenType::EQUALS, "=", startPos};
        case '(': return {TokenType::L_PAR, "(", startPos};
        case ')': return {TokenType::R_PAR, ")", startPos};
        case '[': return {TokenType::L_BRK, "[", startPos};
        case ']': return {TokenType::R_BRK, "]", startPos};
        case ',': return {TokenType::COMMA, ",", startPos};
        default:
            throw InputError("unknown character: '" + std::string(1, c) + "'",
                             SourceRange{startPos, 1});
    }
}

} // namespace MathEngine