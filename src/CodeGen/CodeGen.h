#pragma once

#include "AST/Ast.h"

#include <memory>
#include <unordered_map>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace jlang
{

class CodeGenerator
{
  public:
    CodeGenerator();

    void Generate(const std::vector<std::shared_ptr<AstNode>> &program);
    void DumpIR();

  private:
    llvm::Value *GenerateStatement(std::shared_ptr<AstNode> stmt);
    llvm::Value *GenerateExpression(std::shared_ptr<AstNode> expr);
    llvm::Function *GenerateFunction(std::shared_ptr<FunctionDecl> func);

    llvm::Type *MapType(const TypeRef &typeRef);

  private:
    llvm::LLVMContext m_Context;
    std::unique_ptr<llvm::Module> m_Module;
    llvm::IRBuilder<> m_IRBuilder;
    std::unordered_map<std::string, llvm::Value *> m_namedValues;
};

} // namespace jlang
