#pragma once

#include "../Enums/TokenTypes.h"
#include "../Parser/Parser.h"
#include "../Types/Token.h"

#include <string>
#include <vector>

namespace jlang
{

class Scanner
{
  public:
    explicit Scanner(const std::string &source);
    std::vector<Token> Tokenize();

  private:
    void ScanToken();
    char Advance();

    char Peek() const;
    char PeekNext() const;

    bool IsMatched(char expected);
    bool IsEndReached() const;

    void AddToken(TokenType type);
    void AddToken(TokenType type, const std::string &lexeme);

    void AddIdentifier();
    void AddNumber();
    void AddStringLiteral();
    void AddCharLiteral();

    void SkipWhitespace();

    TokenType IsKeywordOrIdentifier(const std::string &text);

  private:
    std::vector<Token> m_Tokens;

    const std::string &m_Source;

    size_t m_Start = 0;
    size_t m_CurrentPosition = 0;

    uint32_t m_CurrentLine = 1;
};

} // namespace jlang
