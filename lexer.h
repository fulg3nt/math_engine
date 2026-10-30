#ifndef LEXER_H
#define LEXER_H

#include <string>
#include "input_error.h"

namespace MathEngine
{

enum class TokenType {
    NUMBER,
    IDENTIFIER,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    CARET,
    EQUALS,
    L_PAR,
    R_PAR,
    L_BRK,
    R_BRK,
    COMMA,
    END
};

struct Token {
    TokenType type;
    std::string value;
    size_t pos;

    SourceRange toSourceRange() { return {pos, !value.empty() ? value.length() : 1}; }
};

class Lexer
{
  public:
    explicit Lexer(std::string src);
    Token nextToken();

  private:
    std::string m_src;
    size_t m_pos = 0;

    char peek(size_t offset) const;
    char consume();
};

} // namespace MathEngine

#endif // LEXER_H