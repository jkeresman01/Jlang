#pragma once

namespace jlang
{
enum class NodeType
{
    InterfaceDecl,
    StructDecl,
    FunctionDecl,
    VariableDecl,

    IfStatement,
    WhileStatement,
    ForStatement,
    BlockStatement,
    ExprStatement,
    ReturnStatement,

    CallExpr,
    BinaryExpr,
    UnaryExpr,
    VarExpr,
    LiteralExpr,
    CastExpr,
    AllocExpr,
    AssignExpr,
    MemberAccessExpr,
    PrefixExpr,
    PostfixExpr
};
} // namespace jlang