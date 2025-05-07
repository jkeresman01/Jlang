#pragma once

namespace jlang
{

struct Token
{
    TokenType m_type;
    std::string m_lexeme;
    uint32_t m_CurrentLine;

    Token(const TokenType type, const std::string lexeme, uint32_t const currentLine)
        : m_type(type), m_lexeme(std::move(lexeme)), m_CurrentLine(currentLine)
    {
    }

    std::string ToString() const
    {
        std::stringstream ss;

        ss << token.m_CurrentLine << ": ";
        ss << token.m_lexeme;
        ss << " (" << static_cast<int32_t>(token.m_type) << ")";

        return ss.str();
    }
};

} // namespace jlang
