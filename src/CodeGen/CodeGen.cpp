#include "CodeGeneration/CodeGeneration.h"
#include <iostream>
#include <llvm/IR/Verifier.h>

namespace Jlang
{

CodeGenerator::CodeGenerator()
    : m_Module(std::make_unique<llvm::Module>("JlangModule", m_Context)), m_IRBuilder(m_Context)
{
}

void CodeGenerator::Generate(const std::vector<std::shared_ptr<AstNode>> &program)
{
    for (const auto &node : program)
    {
        if (node->type == NodeType::FunctionDecl)
        {
            GenerateFunction(std::static_pointer_cast<FunctionDecl>(node));
        }
    }
}

llvm::Function *CodeGenerator::GenerateFunction(std::shared_ptr<FunctionDecl> func)
{
    std::vector<llvm::Type *> argTypes;
    for (const auto &param : func->params)
    {
        argTypes.push_back(MapType(param.type));
    }

    llvm::FunctionType *functionType = llvm::FunctionType::get(MapType(func->returnType), argTypes, false);

    llvm::Function *function =
        llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, func->name, m_Module.get());

    llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(m_Context, "entry", function);
    m_IRBuilder.SetInsertPoint(entryBlock);

    unsigned i = 0;
    for (auto &arg : function->args())
    {
        arg.setName(func->params[i].name);
        m_namedValues[func->params[i].name] = &arg;
        ++i;
    }

    GenerateStatement(func->body);

    if (func->returnType.name == "void")
    {
        m_IRBuilder.CreateRetVoid();
    }

    llvm::verifyFunction(*function);
    return function;
}

llvm::Value *CodeGenerator::GenerateStatement(std::shared_ptr<AstNode> stmt)
{
    switch (stmt->type)
    {
    case NodeType::BlockStatement: {
        auto block = std::static_pointer_cast<BlockStatement>(stmt);
        for (const auto &s : block->statements)
        {
            GenerateStatement(s);
        }
        break;
    }
    case NodeType::ExprStatement: {
        auto exprStmt = std::static_pointer_cast<ExprStatement>(stmt);
        return GenerateExpression(exprStmt->expression);
    }
    default:
        // TODO: Add other statement types like IfStatement, ReturnStatement
        break;
    }

    return nullptr;
}

llvm::Value *CodeGenerator::GenerateExpression(std::shared_ptr<AstNode> expr)
{
    switch (expr->type)
    {
    case NodeType::VarExpr: {
        auto var = std::static_pointer_cast<VarExpr>(expr);
        return m_namedValues[var->name];
    }
    case NodeType::LiteralExpr: {
        auto literal = std::static_pointer_cast<LiteralExpr>(expr);
        return m_IRBuilder.CreateGlobalStringPtr(literal->value);
    }
    case NodeType::CallExpr: {
        auto call = std::static_pointer_cast<CallExpr>(expr);
        llvm::Function *callee = m_Module->getFunction(call->callee);

        if (!callee)
        {
            JLANG_ERROR(TEXT("Unknown function: %s", call->callee.c_str()));
            return nullptr;
        }

        std::vector<llvm::Value *> args;
        for (const auto &arg : call->arguments)
        {
            args.push_back(GenerateExpression(arg));
        }

        return m_IRBuilder.CreateCall(callee, args, call->callee + "_call");
    }
    default:
        break;
    }

    return nullptr;
}

llvm::Type *CodeGenerator::MapType(const TypeRef &typeRef)
{
    if (typeRef.name == "void")
    {
        return llvm::Type::getVoidTy(m_Context);
    }

    if (typeRef.name == "int32")
    {
        return llvm::Type::getInt32Ty(m_Context);
    }

    if (typeRef.name == "char")
    {
        return typeRef.isPointer ? llvm::Type::getInt8PtrTy(m_Context) : llvm::Type::getInt8Ty(m_Context);
    }

    return llvm::Type::getVoidTy(m_Context);
}

void CodeGenerator::DumpIR()
{
    m_Module->print(llvm::outs(), nullptr);
}

} // namespace Jlang
