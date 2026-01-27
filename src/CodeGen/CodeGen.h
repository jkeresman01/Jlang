#pragma once

#include "AstVisitor.h"

#include "../AST/Ast.h"
#include "../AST/Expressions/Expressions.h"
#include "../AST/Statements/Statements.h"
#include "../AST/TopLevelDecl/TopLevelDecl.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace jlang
{

class CodeGenerator : public AstVisitor
{
  public:
    CodeGenerator();

    void Generate(const std::vector<std::shared_ptr<AstNode>> &program);
    void DumpIR();

  private:
    virtual void VisitFunctionDecl(FunctionDecl &) override;
    virtual void VisitInterfaceDecl(InterfaceDecl &) override;
    virtual void VisitStructDecl(StructDecl &) override;
    virtual void VisitVariableDecl(VariableDecl &) override;

    virtual void VisitIfStatement(IfStatement &) override;
    virtual void VisitWhileStatement(WhileStatement &) override;
    virtual void VisitBlockStatement(BlockStatement &) override;
    virtual void VisitExprStatement(ExprStatement &) override;
    virtual void VisitReturnStatement(ReturnStatement &) override;

    virtual void VisitCallExpr(CallExpr &) override;
    virtual void VisitBinaryExpr(BinaryExpr &) override;
    virtual void VisitUnaryExpr(UnaryExpr &) override;
    virtual void VisitLiteralExpr(LiteralExpr &) override;
    virtual void VisitVarExpr(VarExpr &) override;
    virtual void VisitCastExpr(CastExpr &) override;
    virtual void VisitAllocExpr(AllocExpr &) override;
    virtual void VisitAssignExpr(AssignExpr &) override;
    virtual void VisitMemberAccessExpr(MemberAccessExpr &) override;

  private:
    void DeclareExternalFunctions();
    llvm::Type *MapType(const TypeRef &typeRef);
    TypeRef InferTypeRef(llvm::Type *llvmType);

    // Struct field information
    struct FieldInfo
    {
        unsigned index;
        TypeRef type;
        bool isPublic;
    };

    struct StructInfo
    {
        llvm::StructType *llvmType;
        std::unordered_map<std::string, FieldInfo> fields;
    };

    // Track variable usage for unused variable detection
    struct VariableInfo
    {
        llvm::Value *value;
        TypeRef type;
        bool used = false;
    };

    void CheckUnusedVariables();

  private:
    llvm::LLVMContext m_Context;
    std::unique_ptr<llvm::Module> m_Module;
    llvm::IRBuilder<> m_IRBuilder;

    std::unordered_map<std::string, VariableInfo> m_variables;  // Track variables with usage info
    std::unordered_map<std::string, StructInfo> m_structTypes;  // Track struct definitions
    std::unordered_set<std::string> m_currentFunctionVariables; // Variables declared in current function
    llvm::Value *m_LastValue = nullptr;
};

} // namespace jlang
