#include "Parser.h"

namespace jlang
{

Parser::Parser(const std::vector<Token> &tokens) : m_Tokens(tokens), m_CurrentPosition(0) {}

std::vector<std::shared_ptr<AstNode>> Parser::Parse()
{
    std::vector<std::shared_ptr<AstNode>> program;

    while (!IsEndReached())
    {
        auto declaration = ParseDeclaration();

        if (declaration)
        {
            program.push_back(declaration);
        }
    }
    return program;
}

bool Parser::IsMatched(TokenType type)
{
    if (Check(m_type))
    {
        Advance();
        return true;
    }
    return false;
}

bool Parser::Check(TokenType type) const
{
    if (IsEndReached())
    {
        return false;
    }

    return Peek().m_type == type;
}

const Token &Parser::Advance()
{
    if (!IsEndReached())
    {
        m_CurrentPosition++;
    }

    return Previous();
}

const Token &Parser::Peek() const
{
    return m_Tokens[m_CurrentPosition];
}

const Token &Parser::Previous() const
{
    return m_Tokens[m_CurrentPosition - 1];
}

bool Parser::IsEndReached() const
{
    return Peek().m_type == TokenType::EndOfFile;
}

std::shared_ptr<AstNode> Parser::ParseDeclaration()
{
    return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode> Parser::ParseInterface()
{
    return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode> Parser::ParseStruct()
{
    return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode> Parser::ParseFunction()
{
    return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode> Parser::ParseStatement()
{
    return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode> Parser::ParseBlock()
{
    return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode> Parser::ParseIfStatement()
{
    return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode> Parser::ParseExpression()
{
    return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode> Parser::ParsePrimary()
{
    return std::shared_ptr<AstNode>();
}

} // namespace jlang
