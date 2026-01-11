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
    BlockStatement,
    ExprStatement,
    ReturnStatement,

    CallExpr,
    BinaryExpr,
    VarExpr,
    LiteralExpr,
    CastExpr,
    AllocExpr
};
} // namespace jlang