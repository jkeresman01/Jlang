#pragma once

#include "../AST/Ast.h"
#include "../AST/Expressions/Expressions.h"
#include "../AST/Statements/Statements.h"
#include "../AST/TopLevelDecl/TopLevelDecl.h"
#include "../Types/Token.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace jlang
{

class Parser
{
  public:
    explicit Parser(const std::vector<Token> &tokens);
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
    std::shared_ptr<AstNode> ParseWhileStatement();
    std::shared_ptr<AstNode> ParseReturnStatement();
    std::shared_ptr<AstNode> ParseVarDecl();
    std::shared_ptr<AstNode> ParseExpression();
    std::shared_ptr<AstNode> ParseLogicalOr();
    std::shared_ptr<AstNode> ParseLogicalAnd();
    std::shared_ptr<AstNode> ParseEquality();
    std::shared_ptr<AstNode> ParseComparison();
    std::shared_ptr<AstNode> ParseAdditive();
    std::shared_ptr<AstNode> ParseMultiplicative();
    std::shared_ptr<AstNode> ParseUnary();
    std::shared_ptr<AstNode> ParsePostfix();
    std::shared_ptr<AstNode> ParseExprStatement();
    std::shared_ptr<AstNode> ParsePrimary();

    bool IsTypeKeyword() const;
    std::string ParseTypeName();

  private:
    const std::vector<Token> &m_Tokens;
    size_t m_CurrentPosition;
};

} // namespace jlang
