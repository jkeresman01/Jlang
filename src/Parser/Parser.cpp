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
        JLANG_ERROR("Expected interaface name");
    }

    const std::string &name = Previous().m_lexeme;

    if (!IsMatched(TokenType::LBrace))
    {
        JLANG_ERROR("Expected '{' after interface name!");
    }

    auto interfaceDeclNode = std::make_shared<InterfaceDecl>();
    interfaceDeclNode->name = name;

    while (!Check(TokenType::RBrace) && !IsEndReached())
    {
        if (!IsMatched(TokenType::Void))
        {
            // It's kinda hardcoded for now!! -> will change that later on
            if (!IsMatched(TokenType::Void))
            {
                JLANG_ERROR("Expected 'void' in interface method");
            }

            if (!IsMatched(TokenType::Identifier))
            {
                JLANG_ERROR("Expected method name");
            }

            std::string methodName = Previous().m_lexeme;

            if (!IsMatched(TokenType::LParen) || !IsMatched(TokenType::RParen) ||
                !IsMatched(TokenType::Semicolon))
            {
                JLANG_ERROR("Expected '()' and ';' after method name");
            }

            interfaceDeclNode->methods.push_back(methodName);
        }
    }

    if (!IsMatched(TokenType::RBrace))
    {
        JLANG_ERROR("Expected '}' at end of interface");
    }

    return interfaceDeclNode;
}

std::shared_ptr<AstNode> Parser::ParseStruct()
{
    Advance();

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR("Expected struct name");
    }

    const std::string name = Previous().m_lexeme;

    std::string implementedInterface;

    if (IsMatched(TokenType::Arrow))
    {
        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected interface name after '->'");
        }

        implementedInterface = Previous().m_lexeme;
    }

    if (!IsMatched(TokenType::LBrace))
    {
        throw std::runtime_error("Expected '{' after struct declaration");
    }

    auto node = std::make_shared<StructDecl>();
    node->name = name;
    node->interfaceImplemented = implementedInterface;

    while (!Check(TokenType::RBrace) && !IsEndReached())
    {
        if (!IsMatched(TokenType::Var))
        {
            throw std::runtime_error("Expected 'var' in struct field");
        }

        if (!IsMatched(TokenType::Identifier))
        {
            throw std::runtime_error("Expected field name");
        }

        std::string fieldName = Previous().m_lexeme;

        if (!IsMatched(TokenType::Identifier))
        {
            throw std::runtime_error("Expected field type");
        }

        std::string typeName = Previous().m_lexeme;
        bool isPointer = false;

        if (IsMatched(TokenType::Star))
        {
            isPointer = true;
        }

        if (!IsMatched(TokenType::Semicolon))
        {
            throw std::runtime_error("Expected ';' after struct field");
        }

        StructField field{fieldName, TypeRef{typeName, isPointer}};
        node->fields.push_back(field);
    }

    if (!IsMatched(TokenType::RBrace))
    {
        throw std::runtime_error("Expected '}' after struct body");
    }

    return node;
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
