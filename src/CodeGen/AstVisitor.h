#pragma once

namespace jlang
{

// Forward declarations for AST nodes
struct FunctionDecl;
struct InterfaceDecl;
struct StructDecl;
struct VariableDecl;
struct IfStatement;
struct WhileStatement;
struct BlockStatement;
struct ExprStatement;
struct ReturnStatement;
struct CallExpr;
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct VarExpr;
struct CastExpr;
struct AllocExpr;
struct AssignExpr;

class AstVisitor
{
  public:
    virtual ~AstVisitor() = default;

    virtual void VisitFunctionDecl(FunctionDecl &) = 0;
    virtual void VisitInterfaceDecl(InterfaceDecl &) = 0;
    virtual void VisitStructDecl(StructDecl &) = 0;
    virtual void VisitVariableDecl(VariableDecl &) = 0;

    virtual void VisitIfStatement(IfStatement &) = 0;
    virtual void VisitWhileStatement(WhileStatement &) = 0;
    virtual void VisitBlockStatement(BlockStatement &) = 0;
    virtual void VisitExprStatement(ExprStatement &) = 0;
    virtual void VisitReturnStatement(ReturnStatement &) = 0;

    virtual void VisitCallExpr(CallExpr &) = 0;
    virtual void VisitBinaryExpr(BinaryExpr &) = 0;
    virtual void VisitUnaryExpr(UnaryExpr &) = 0;
    virtual void VisitLiteralExpr(LiteralExpr &) = 0;
    virtual void VisitVarExpr(VarExpr &) = 0;
    virtual void VisitCastExpr(CastExpr &) = 0;
    virtual void VisitAllocExpr(AllocExpr &) = 0;
    virtual void VisitAssignExpr(AssignExpr &) = 0;
};
} // namespace jlang