#include "Parser.h"

#include "../Common/Logger.h"

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
    if (Check(type))
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
    Advance();

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR(TEXT("Expected interaface name"));
    }

    const std::string name = Previous().m_lexeme;

    if (!IsMatched(TokenType::LBrace))
    {
        JLANG_ERROR(TEXT("Expected '{' after interface name!"));
    }

    auto interfaceDeclNode = std::make_shared<InterfaceDecl>();
    interfaceDeclNode->name = name;

    while (!Check(TokenType::RBrace) && !IsEndReached())
    {
        if (!IsMatched(TokenType::Void))
        {
            // It's kinda hardcoded for now!! -> will change that later on
            if (!match(TokenType::Void))
            {
                JLANG_ERROR(TEXT(("Expected 'void' in interface method"));
            }

            if (!match(TokenType::Identifier))
            {
                JLANG_ERROR(TEXT(("Expected method name"));
            }

            std::string methodName = previous().lexeme;

            if (!match(TokenType::LParen) || !match(TokenType::RParen) || !match(TokenType::Semicolon))
            {
                JLANG_ERROR(TEXT(("Expected '()' and ';' after method name"));
            }

            node->methods.push_back(methodName);
        }
    }

    if (!match(TokenType::RBrace))
    {
        JLANG_ERROR(TEXT("Expected '}' at end of interface"));
    }

    return node;
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
