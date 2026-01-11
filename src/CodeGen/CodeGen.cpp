#include "CodeGen.h"

#include "../Common/Logger.h"

#include <algorithm>
#include <iostream>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

namespace jlang
{

CodeGenerator::CodeGenerator()
    : m_Module(std::make_unique<llvm::Module>("JlangModule", m_Context)), m_IRBuilder(m_Context)
{
}

void CodeGenerator::Generate(const std::vector<std::shared_ptr<AstNode>> &program)
{
    DeclareExternalFunctions();

    for (auto &node : program)
    {
        if (node)
        {
            node->Accept(*this);
        }
    }
}

void CodeGenerator::DeclareExternalFunctions()
{
    // Declare printf: int printf(const char*, ...)
    llvm::FunctionType *printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(m_Context),
                                                             {llvm::Type::getInt8PtrTy(m_Context)}, true);
    llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", m_Module.get());

    // Declare jout as alias to printf: int jout(const char*, ...)
    llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "jout", m_Module.get());

    // Declare malloc: void* malloc(size_t)
    llvm::FunctionType *mallocType = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(m_Context),
                                                             {llvm::Type::getInt64Ty(m_Context)}, false);
    llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", m_Module.get());

    // Declare jalloc as alias to malloc
    llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "jalloc", m_Module.get());

    // Declare free: void free(void*)
    llvm::FunctionType *freeType = llvm::FunctionType::get(llvm::Type::getVoidTy(m_Context),
                                                           {llvm::Type::getInt8PtrTy(m_Context)}, false);
    llvm::Function::Create(freeType, llvm::Function::ExternalLinkage, "free", m_Module.get());

    // Declare jfree as alias to free
    llvm::Function::Create(freeType, llvm::Function::ExternalLinkage, "jfree", m_Module.get());
}

void CodeGenerator::DumpIR()
{
    m_Module->print(llvm::outs(), nullptr);
}

void CodeGenerator::VisitFunctionDecl(FunctionDecl &node)
{
    std::vector<llvm::Type *> paramTypes;
    paramTypes.reserve(node.params.size());
    std::transform(node.params.begin(), node.params.end(), std::back_inserter(paramTypes),
                   [this](const Parameter &param) { return MapType(param.type); });

    llvm::FunctionType *funcType = llvm::FunctionType::get(MapType(node.returnType), paramTypes, false);

    llvm::Function *function =
        llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node.name, m_Module.get());

    llvm::BasicBlock *entry = llvm::BasicBlock::Create(m_Context, "entry", function);
    m_IRBuilder.SetInsertPoint(entry);

    unsigned i = 0;
    for (auto &arg : function->args())
    {
        arg.setName(node.params[i].name);
        m_namedValues[node.params[i].name] = &arg;
        ++i;
    }

    if (node.body)
    {
        node.body->Accept(*this);
    }

    if (node.returnType.name == "void")
    {
        m_IRBuilder.CreateRetVoid();
    }

    llvm::verifyFunction(*function);
}

void CodeGenerator::VisitInterfaceDecl(InterfaceDecl &) {}

void CodeGenerator::VisitStructDecl(StructDecl &) {}

void CodeGenerator::VisitVariableDecl(VariableDecl &node)
{
    llvm::Type *varType = MapType(node.varType);

    llvm::AllocaInst *alloca = m_IRBuilder.CreateAlloca(varType, nullptr, node.name);

    if (node.initializer)
    {
        node.initializer->Accept(*this);
        if (m_LastValue)
        {
            if (m_LastValue->getType() != varType && varType->isPointerTy() &&
                m_LastValue->getType()->isPointerTy())
            {
                m_LastValue = m_IRBuilder.CreateBitCast(m_LastValue, varType, "cast");
            }
            m_IRBuilder.CreateStore(m_LastValue, alloca);
        }
    }

    m_namedValues[node.name] = alloca;
}

void CodeGenerator::VisitIfStatement(IfStatement &node)
{
    node.condition->Accept(*this);
    llvm::Value *isConditionalValue = m_LastValue;

    if (!isConditionalValue)
    {
        JLANG_ERROR("Invalid condition in if statement");
    }

    if (isConditionalValue->getType()->isIntegerTy(32))
    {
        isConditionalValue = m_IRBuilder.CreateICmpNE(
            isConditionalValue, llvm::ConstantInt::get(isConditionalValue->getType(), 0), "ifcond");
    }

    llvm::Function *parentFunction = m_IRBuilder.GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(m_Context, "then", parentFunction);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(m_Context, "else");
    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(m_Context, "ifcont");

    m_IRBuilder.CreateCondBr(isConditionalValue, thenBlock, elseBlock);

    m_IRBuilder.SetInsertPoint(thenBlock);
    node.thenBranch->Accept(*this);
    m_IRBuilder.CreateBr(mergeBlock);

    parentFunction->getBasicBlockList().push_back(elseBlock);
    m_IRBuilder.SetInsertPoint(elseBlock);
    if (node.elseBranch)
    {
        node.elseBranch->Accept(*this);
    }
    m_IRBuilder.CreateBr(mergeBlock);

    parentFunction->getBasicBlockList().push_back(mergeBlock);
    m_IRBuilder.SetInsertPoint(mergeBlock);
}

void CodeGenerator::VisitBlockStatement(BlockStatement &node)
{
    for (auto &statement : node.statements)
    {
        if (statement)
        {
            statement->Accept(*this);
        }
    }
}

void CodeGenerator::VisitExprStatement(ExprStatement &node)
{
    if (node.expression)
    {
        node.expression->Accept(*this);
    }
}

void CodeGenerator::VisitCallExpr(CallExpr &node)
{
    llvm::Function *callee = m_Module->getFunction(node.callee);

    if (!callee)
    {
        JLANG_ERROR(STR("Unknown function: %s", node.callee.c_str()));
    }

    std::vector<llvm::Value *> args;

    for (auto &arg : node.arguments)
    {
        arg->Accept(*this);

        if (!m_LastValue)
        {
            JLANG_ERROR(STR("Invalid argument in call to %s", node.callee.c_str()));
        }
        args.push_back(m_LastValue);
    }

    m_LastValue = m_IRBuilder.CreateCall(callee, args, node.callee + "_call");
}

void CodeGenerator::VisitBinaryExpr(BinaryExpr &node)
{
    node.left->Accept(*this);
    llvm::Value *leftVal = m_LastValue;

    node.right->Accept(*this);
    llvm::Value *rightVal = m_LastValue;

    if (!leftVal || !rightVal)
    {
        JLANG_ERROR("Invalid operands in binary expression");
        return;
    }

    if (node.op == "==")
    {
        if (leftVal->getType()->isPointerTy() && rightVal->getType()->isPointerTy())
        {
            m_LastValue = m_IRBuilder.CreateICmpEQ(leftVal, rightVal, "ptreq");
        }
        else if (leftVal->getType()->isIntegerTy() && rightVal->getType()->isIntegerTy())
        {
            m_LastValue = m_IRBuilder.CreateICmpEQ(leftVal, rightVal, "eq");
        }
        else
        {
            JLANG_ERROR("Unsupported types for == comparison");
        }
    }
    else if (node.op == "!=")
    {
        if (leftVal->getType()->isPointerTy() && rightVal->getType()->isPointerTy())
        {
            m_LastValue = m_IRBuilder.CreateICmpNE(leftVal, rightVal, "ptrne");
        }
        else if (leftVal->getType()->isIntegerTy() && rightVal->getType()->isIntegerTy())
        {
            m_LastValue = m_IRBuilder.CreateICmpNE(leftVal, rightVal, "ne");
        }
        else
        {
            JLANG_ERROR("Unsupported types for != comparison");
        }
    }
    else if (node.op == "<")
    {
        m_LastValue = m_IRBuilder.CreateICmpSLT(leftVal, rightVal, "lt");
    }
    else if (node.op == ">")
    {
        m_LastValue = m_IRBuilder.CreateICmpSGT(leftVal, rightVal, "gt");
    }
    else if (node.op == "+")
    {
        m_LastValue = m_IRBuilder.CreateAdd(leftVal, rightVal, "add");
    }
    else if (node.op == "-")
    {
        m_LastValue = m_IRBuilder.CreateSub(leftVal, rightVal, "sub");
    }
    else if (node.op == "*")
    {
        m_LastValue = m_IRBuilder.CreateMul(leftVal, rightVal, "mul");
    }
    else if (node.op == "/")
    {
        m_LastValue = m_IRBuilder.CreateSDiv(leftVal, rightVal, "div");
    }
    else
    {
        JLANG_ERROR(STR("Unknown binary operator: %s", node.op.c_str()));
    }
}

void CodeGenerator::VisitLiteralExpr(LiteralExpr &node)
{
    if (node.value == "NULL" || node.value == "null" || node.value == "nullptr")
    {
        m_LastValue = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(m_Context));
    }
    else if (node.value.front() == '"' && node.value.back() == '"')
    {
        std::string strValue = node.value.substr(1, node.value.size() - 2);
        m_LastValue = m_IRBuilder.CreateGlobalStringPtr(strValue, "str");
    }
    else
    {
        try
        {
            int64_t intValue = std::stoll(node.value);
            m_LastValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_Context), intValue);
        }
        catch (...)
        {
            JLANG_ERROR(STR("Unknown literal: %s", node.value.c_str()));
        }
    }
}

void CodeGenerator::VisitVarExpr(VarExpr &node)
{
    auto it = m_namedValues.find(node.name);

    if (it == m_namedValues.end())
    {
        JLANG_ERROR(STR("Undefined variable: %s", node.name.c_str()));
        return;
    }

    llvm::Value *val = it->second;

    if (llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(val))
    {
        m_LastValue = m_IRBuilder.CreateLoad(alloca->getAllocatedType(), alloca, node.name);
    }
    else
    {
        m_LastValue = val;
    }
}

void CodeGenerator::VisitCastExpr(CastExpr &node)
{
    node.expr->Accept(*this);
    llvm::Value *valueToCast = m_LastValue;

    if (!valueToCast)
    {
        JLANG_ERROR("Invalid expression in cast");
    }

    llvm::Type *targetLLVMType = MapType(node.targetType);

    if (!targetLLVMType)
    {
        JLANG_ERROR("Unknown target type in cast");
    }

    bool isPointerToPointerCast = valueToCast->getType()->isPointerTy() && targetLLVMType->isPointerTy();

    if (isPointerToPointerCast)
    {
        m_LastValue = m_IRBuilder.CreateBitCast(valueToCast, targetLLVMType, "ptrcast");
    }
    else
    {
        JLANG_ERROR("Unsupported cast");
    }
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
        llvm::Type *charType = llvm::Type::getInt8Ty(m_Context);
        return typeRef.isPointer ? llvm::PointerType::getUnqual(charType) : charType;
    }

    return llvm::Type::getVoidTy(m_Context);
}

} // namespace jlang
