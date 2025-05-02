#pragma once
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace jlang
{

enum class NodeType
{
    InterfaceDecl,
    StructDecl,
    FunctionDecl,
    VariableDecl,
    IfStatement,
    BlockStatement,
    ExprStatement,
    CallExpr,
    BinaryExpr,
    VarExpr,
    LiteralExpr,
    CastExpr
};

struct AstNode
{
    NodeType type;
    virtual ~AstNode() = default;
};

using AstNodePtr = std::shared_ptr<AstNode>;

struct TypeRef
{
    std::string name;
    bool isPointer = false;
};

struct InterfaceDecl : public AstNode
{
    std::string name;
    std::vector<std::string> methods;

    InterfaceDecl() { type = NodeType::InterfaceDecl; }
};

struct StructField
{
    std::string name;
    TypeRef type;
};

struct StructDecl : public AstNode
{
    std::string name;
    std::string interfaceImplemented;
    std::vector<StructField> fields;

    StructDecl() { type = NodeType::StructDecl; }
};

struct Parameter
{
    std::string name;
    TypeRef type;
};

struct FunctionDecl : public AstNode
{
    std::string name;
    std::vector<Parameter> params;
    TypeRef returnType;
    std::shared_ptr<AstNode> body;

    FunctionDecl() { type = NodeType::FunctionDecl; }
};

struct VariableDecl : public AstNode
{
    std::string name;
    TypeRef varType;
    std::shared_ptr<AstNode> initializer;

    VariableDecl() { type = NodeType::VariableDecl; }
};

struct Statement : public AstNode
{
};

struct IfStatement : public Statement
{
    std::shared_ptr<AstNode> condition;
    std::shared_ptr<AstNode> thenBranch;
    std::shared_ptr<AstNode> elseBranch;

    IfStatement() { type = NodeType::IfStatement; }
};

struct BlockStatement : public Statement
{
    std::vector<std::shared_ptr<AstNode>> statements;

    BlockStatement() { type = NodeType::BlockStatement; }
};

struct ExprStatement : public Statement
{
    std::shared_ptr<AstNode> expression;

    ExprStatement() { type = NodeType::ExprStatement; }
};

struct Expression : public AstNode
{
};

struct CallExpr : public Expression
{
    std::string callee;
    std::vector<std::shared_ptr<AstNode>> arguments;

    CallExpr() { type = NodeType::CallExpr; }
};

struct BinaryExpr : public Expression
{
    std::string op;
    std::shared_ptr<AstNode> left;
    std::shared_ptr<AstNode> right;

    BinaryExpr() { type = NodeType::BinaryExpr; }
};

struct VarExpr : public Expression
{
    std::string name;

    VarExpr() { type = NodeType::VarExpr; }
};

struct LiteralExpr : public Expression
{
    std::string value;

    LiteralExpr() { type = NodeType::LiteralExpr; }
};

struct CastExpr : public Expression
{
    TypeRef targetType;
    std::shared_ptr<AstNode> expr;

    CastExpr() { type = NodeType::CastExpr; }
};

} // namespace jlang
