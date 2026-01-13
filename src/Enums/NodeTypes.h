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
    BlockStatement,
    ExprStatement,
    ReturnStatement,

    CallExpr,
    BinaryExpr,
    VarExpr,
    LiteralExpr,
    CastExpr,
    AllocExpr,
    AssignExpr
};
} // namespace jlang