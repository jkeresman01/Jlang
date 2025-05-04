#pragma once

#include "../AST/Ast.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace jlang
{

enum class TokenType
{
    // Keywords
    Interface,
    Struct,
    Var,
    Void,
    Int32,
    If,
    Else,
    Return,

    // Symbols
    LBrace,
    RBrace,
    LParen,
    RParen,
    Semicolon,
    Colon,
    Arrow,
    Assign,
    Star,
    Comma,
    Dot,
    NotEqual,
    EqualEqual,
    Less,
    Greater,
    Equal,

    // Literals and identifiers
    Identifier,
    StringLiteral,
    NumberLiteral,

    EndOfFile,
    Unknown
};

struct Token
{
    TokenType m_type;
    std::string m_lexeme;
    uint32_t m_CurrentLine;

    Token(const TokenType type, const std::string lexeme, uint32_t const currentLine)
        : m_type(type), m_lexeme(std::move(lexeme)), m_CurrentLine(currentLine)
    {
    }
};

class Parser
{
  public:
    explicit Parser(const std::vector<Token> &m_Tokens);
    std::vector<std::shared_ptr<AstNode>> Parse();

  private:
    bool IsMatched(TokenType type);
    bool Check(TokenType type) const;
    const Token &Advance();
    const Token &Peek() const;
    const Token &Previous() const;
    bool IsEndReached() const;

    std::shared_ptr<AstNode> ParseDeclaration();
    std::shared_ptr<AstNode> ParseInterface();
    std::shared_ptr<AstNode> ParseStruct();
    std::shared_ptr<AstNode> ParseFunction();
    std::shared_ptr<AstNode> ParseStatement();
    std::shared_ptr<AstNode> ParseBlock();
    std::shared_ptr<AstNode> ParseIfStatement();
    std::shared_ptr<AstNode> ParseExpression();
    std::shared_ptr<AstNode> ParseExprStatement();
    std::shared_ptr<AstNode> ParsePrimary();

  private:
    const std::vector<Token> &m_Tokens;
    size_t m_CurrentPosition;
};

} // namespace jlang
