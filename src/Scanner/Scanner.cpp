#include "../Scanner/Scanner.h"

#include <cctype>
#include <unordered_map>

namespace jlang
{

static std::unordered_map<std::string, TokenType> s_Keywords = {
    // Control flow
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"return", TokenType::Return},
    // Declarations
    {"fn", TokenType::Fn},
    {"var", TokenType::Var},
    {"struct", TokenType::Struct},
    {"interface", TokenType::Interface},
    // Types
    {"void", TokenType::Void},
    {"i8", TokenType::I8},
    {"i16", TokenType::I16},
    {"i32", TokenType::I32},
    {"i64", TokenType::I64},
    {"u8", TokenType::U8},
    {"u16", TokenType::U16},
    {"u32", TokenType::U32},
    {"u64", TokenType::U64},
    {"f32", TokenType::F32},
    {"f64", TokenType::F64},
    {"bool", TokenType::Bool},
    {"char", TokenType::Char},
    // Literals
    {"null", TokenType::Null},
    {"true", TokenType::True},
    {"false", TokenType::False},
    // Memory
    {"alloc", TokenType::Alloc},
    // Logical operators (non-short-circuit)
    {"and", TokenType::AndKeyword},
    {"or", TokenType::OrKeyword},
};

Scanner::Scanner(const std::string &source) : m_Source(source) {}

std::vector<Token> Scanner::Tokenize()
{
    while (!IsEndReached())
    {
        SkipWhitespace();
        if (IsEndReached())
        {
            break;
        }
        m_Start = m_CurrentPosition;
        ScanToken();
    }

    m_Tokens.emplace_back(TokenType::EndOfFile, "", m_CurrentLine);
    return m_Tokens;
}

void Scanner::ScanToken()
{
    char c = Advance();

    switch (c)
    {
    case '{':
        AddToken(TokenType::LBrace);
        break;
    case '}':
        AddToken(TokenType::RBrace);
        break;
    case '(':
        AddToken(TokenType::LParen);
        break;
    case ')':
        AddToken(TokenType::RParen);
        break;
    case ';':
        AddToken(TokenType::Semicolon);
        break;
    case ':':
        AddToken(IsMatched('=') ? TokenType::ColonEqual : TokenType::Colon);
        break;
    case ',':
        AddToken(TokenType::Comma);
        break;
    case '.':
        AddToken(TokenType::Dot);
        break;
    case '*':
        AddToken(TokenType::Star);
        break;
    case '+':
        AddToken(IsMatched('+') ? TokenType::PlusPlus : TokenType::Plus);
        break;
    case '/':
        AddToken(TokenType::Slash);
        break;
    case '=':
        AddToken(IsMatched('=') ? TokenType::EqualEqual : TokenType::Equal);
        break;
    case '!':
        AddToken(IsMatched('=') ? TokenType::NotEqual : TokenType::Not);
        break;
    case '&':
        if (IsMatched('&'))
        {
            AddToken(TokenType::And);
        }
        else
        {
            AddToken(TokenType::Unknown);
        }
        break;
    case '|':
        if (IsMatched('|'))
        {
            AddToken(TokenType::Or);
        }
        else
        {
            AddToken(TokenType::Unknown);
        }
        break;
    case '<':
        AddToken(TokenType::Less);
        break;
    case '>':
        AddToken(TokenType::Greater);
        break;
    case '-':
        if (IsMatched('-'))
        {
            AddToken(TokenType::MinusMinus);
        }
        else if (IsMatched('>'))
        {
            AddToken(TokenType::Arrow);
        }
        else
        {
            AddToken(TokenType::Minus);
        }
        break;
    case '"':
        AddStringLiteral();
        break;
    case '\'':
        AddCharLiteral();
        break;
    default:
        if (std::isdigit(c))
        {
            AddNumber();
        }
        else if (std::isalpha(c) || c == '_')
        {
            AddIdentifier();
        }
        else
        {
            AddToken(TokenType::Unknown);
        }
    }
}

void Scanner::SkipWhitespace()
{
    while (!IsEndReached())
    {
        char c = Peek();

        if (c == ' ' || c == '\r' || c == '\t')
        {
            Advance();
        }
        else if (c == '\n')
        {
            m_CurrentLine++;
            Advance();
        }
        else if (c == '/')
        {
            if (PeekNext() == '/')
            {
                // Single-line comment: skip until end of line
                while (Peek() != '\n' && !IsEndReached())
                {
                    Advance();
                }
            }
            else if (PeekNext() == '*')
            {
                // Block comment: skip until */
                Advance(); // consume '/'
                Advance(); // consume '*'
                while (!IsEndReached())
                {
                    if (Peek() == '*' && PeekNext() == '/')
                    {
                        Advance(); // consume '*'
                        Advance(); // consume '/'
                        break;
                    }
                    if (Peek() == '\n')
                    {
                        m_CurrentLine++;
                    }
                    Advance();
                }
            }
            else
            {
                break; // It's a division operator, not a comment
            }
        }
        else
        {
            break;
        }
    }
}

char Scanner::Advance()
{
    return m_Source[m_CurrentPosition++];
}

char Scanner::Peek() const
{
    return IsEndReached() ? '\0' : m_Source[m_CurrentPosition];
}

char Scanner::PeekNext() const
{
    if (m_CurrentPosition + 1 >= m_Source.length())
    {
        return '\0';
    }
    return m_Source[m_CurrentPosition + 1];
}

bool Scanner::IsMatched(char expected)
{
    if (IsEndReached() || m_Source[m_CurrentPosition] != expected)
    {
        return false;
    }

    m_CurrentPosition++;
    return true;
}

bool Scanner::IsEndReached() const
{
    return m_CurrentPosition >= m_Source.length();
}

void Scanner::AddToken(TokenType type)
{
    AddToken(type, m_Source.substr(m_Start, m_CurrentPosition - m_Start));
}

void Scanner::AddToken(TokenType type, const std::string &lexeme)
{
    m_Tokens.emplace_back(type, lexeme, m_CurrentLine);
}

void Scanner::AddIdentifier()
{
    while (std::isalnum(Peek()) || Peek() == '_')
    {
        Advance();
    }

    std::string text = m_Source.substr(m_Start, m_CurrentPosition - m_Start);
    TokenType type = IsKeywordOrIdentifier(text);

    AddToken(type, text);
}

void Scanner::AddNumber()
{
    while (std::isdigit(Peek()))
    {
        Advance();
    }

    // Check for decimal point (float literal)
    if (Peek() == '.' && std::isdigit(PeekNext()))
    {
        // Consume the '.'
        Advance();

        // Consume the fractional part
        while (std::isdigit(Peek()))
        {
            Advance();
        }

        AddToken(TokenType::FloatLiteral);
    }
    else
    {
        AddToken(TokenType::NumberLiteral);
    }
}

void Scanner::AddStringLiteral()
{
    while (Peek() != '"' && !IsEndReached())
    {
        if (Peek() == '\n')
        {
            m_CurrentLine++;
        }

        Advance();
    }

    if (IsEndReached())
    {
        return;
    }

    Advance();

    std::string value = m_Source.substr(m_Start + 1, m_CurrentPosition - m_Start - 2);

    AddToken(TokenType::StringLiteral, value);
}

void Scanner::AddCharLiteral()
{
    // Current position is after the opening '
    if (IsEndReached())
    {
        AddToken(TokenType::Unknown);
        return;
    }

    char c = Advance();

    // Handle escape sequences
    if (c == '\\' && !IsEndReached())
    {
        char escaped = Advance();
        switch (escaped)
        {
        case 'n':
            c = '\n';
            break;
        case 't':
            c = '\t';
            break;
        case 'r':
            c = '\r';
            break;
        case '\\':
            c = '\\';
            break;
        case '\'':
            c = '\'';
            break;
        case '0':
            c = '\0';
            break;
        default:
            // Unknown escape, just use the character as-is
            c = escaped;
            break;
        }
    }

    if (IsEndReached() || Peek() != '\'')
    {
        AddToken(TokenType::Unknown);
        return;
    }

    Advance(); // consume closing '

    // Store the character value as a string
    AddToken(TokenType::CharLiteral, std::string(1, c));
}

TokenType Scanner::IsKeywordOrIdentifier(const std::string &text)
{
    auto it = s_Keywords.find(text);
    return it != s_Keywords.end() ? it->second : TokenType::Identifier;
}

} // namespace jlang
