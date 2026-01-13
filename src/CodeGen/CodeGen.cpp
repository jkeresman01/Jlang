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
    llvm::Type *ptrType = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(m_Context));
    llvm::FunctionType *printfType =
        llvm::FunctionType::get(llvm::Type::getInt32Ty(m_Context), {ptrType}, true);
    llvm::Function::Create(printfType, llvm::Function::ExternalLinkage, "printf", m_Module.get());

    // Declare malloc: void* malloc(size_t)
    llvm::FunctionType *mallocType =
        llvm::FunctionType::get(ptrType, {llvm::Type::getInt64Ty(m_Context)}, false);
    llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", m_Module.get());

    // Declare free: void free(void*)
    llvm::FunctionType *freeType =
        llvm::FunctionType::get(llvm::Type::getVoidTy(m_Context), {ptrType}, false);
    llvm::Function::Create(freeType, llvm::Function::ExternalLinkage, "free", m_Module.get());
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

    unsigned paramIndex = 0;
    for (auto &arg : function->args())
    {
        arg.setName(node.params[paramIndex].name);
        m_namedValues[node.params[paramIndex].name] = &arg;
        ++paramIndex;
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
    llvm::Value *conditionValue = m_LastValue;

    if (!conditionValue)
    {
        JLANG_ERROR("Invalid condition in if statement");
        return;
    }

    if (conditionValue->getType()->isIntegerTy(32))
    {
        conditionValue = m_IRBuilder.CreateICmpNE(
            conditionValue, llvm::ConstantInt::get(conditionValue->getType(), 0), "ifcond");
    }

    llvm::Function *parentFunction = m_IRBuilder.GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(m_Context, "then", parentFunction);
    llvm::BasicBlock *elseBlock = llvm::BasicBlock::Create(m_Context, "else");
    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(m_Context, "ifcont");

    m_IRBuilder.CreateCondBr(conditionValue, thenBlock, elseBlock);

    m_IRBuilder.SetInsertPoint(thenBlock);
    node.thenBranch->Accept(*this);
    m_IRBuilder.CreateBr(mergeBlock);

    elseBlock->insertInto(parentFunction);
    m_IRBuilder.SetInsertPoint(elseBlock);
    if (node.elseBranch)
    {
        node.elseBranch->Accept(*this);
    }
    m_IRBuilder.CreateBr(mergeBlock);

    mergeBlock->insertInto(parentFunction);
    m_IRBuilder.SetInsertPoint(mergeBlock);
}

void CodeGenerator::VisitWhileStatement(WhileStatement &node)
{
    llvm::Function *parentFunction = m_IRBuilder.GetInsertBlock()->getParent();

    llvm::BasicBlock *condBlock = llvm::BasicBlock::Create(m_Context, "while.cond", parentFunction);
    llvm::BasicBlock *bodyBlock = llvm::BasicBlock::Create(m_Context, "while.body");
    llvm::BasicBlock *exitBlock = llvm::BasicBlock::Create(m_Context, "while.exit");

    // Branch to condition block
    m_IRBuilder.CreateBr(condBlock);

    // Condition block
    m_IRBuilder.SetInsertPoint(condBlock);
    node.condition->Accept(*this);
    llvm::Value *condValue = m_LastValue;

    if (!condValue)
    {
        JLANG_ERROR("Invalid condition in while statement");
        return;
    }

    // Convert i32 to i1 if necessary
    if (condValue->getType()->isIntegerTy(32))
    {
        condValue =
            m_IRBuilder.CreateICmpNE(condValue, llvm::ConstantInt::get(condValue->getType(), 0), "whilecond");
    }

    m_IRBuilder.CreateCondBr(condValue, bodyBlock, exitBlock);

    // Body block
    bodyBlock->insertInto(parentFunction);
    m_IRBuilder.SetInsertPoint(bodyBlock);
    node.body->Accept(*this);
    m_IRBuilder.CreateBr(condBlock);

    // Exit block
    exitBlock->insertInto(parentFunction);
    m_IRBuilder.SetInsertPoint(exitBlock);
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

void CodeGenerator::VisitReturnStatement(ReturnStatement &node)
{
    if (node.value)
    {
        node.value->Accept(*this);
        m_IRBuilder.CreateRet(m_LastValue);
    }
    else
    {
        m_IRBuilder.CreateRetVoid();
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
        m_LastValue =
            llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(m_Context)));
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

    llvm::Value *storedValue = it->second;

    if (llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(storedValue))
    {
        m_LastValue = m_IRBuilder.CreateLoad(alloca->getAllocatedType(), alloca, node.name);
    }
    else
    {
        m_LastValue = storedValue;
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

void CodeGenerator::VisitAllocExpr(AllocExpr &node)
{
    // Get malloc function
    llvm::Function *mallocFunc = m_Module->getFunction("malloc");
    if (!mallocFunc)
    {
        JLANG_ERROR("malloc not declared");
        return;
    }

    // Calculate size - for now, use 8 bytes as default struct size
    // TODO: Implement proper struct size calculation based on registered struct types
    llvm::Value *size = llvm::ConstantInt::get(llvm::Type::getInt64Ty(m_Context), 8);

    // Call malloc
    llvm::Value *allocated = m_IRBuilder.CreateCall(mallocFunc, {size}, "alloc");

    // The result is already a pointer, store it directly
    m_LastValue = allocated;
}

void CodeGenerator::VisitAssignExpr(AssignExpr &node)
{
    // Evaluate the right-hand side
    node.value->Accept(*this);
    llvm::Value *valueToStore = m_LastValue;

    if (!valueToStore)
    {
        JLANG_ERROR("Invalid value in assignment");
        return;
    }

    // Find the variable
    auto it = m_namedValues.find(node.name);
    if (it == m_namedValues.end())
    {
        JLANG_ERROR(STR("Undefined variable in assignment: %s", node.name.c_str()));
        return;
    }

    llvm::Value *targetVar = it->second;

    if (llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(targetVar))
    {
        m_IRBuilder.CreateStore(valueToStore, alloca);
        m_LastValue = valueToStore;
    }
    else
    {
        JLANG_ERROR("Cannot assign to non-variable");
    }
}

llvm::Type *CodeGenerator::MapType(const TypeRef &typeRef)
{
    llvm::Type *baseType = nullptr;

    if (typeRef.name == "void")
    {
        return llvm::Type::getVoidTy(m_Context);
    }
    else if (typeRef.name == "i8")
    {
        baseType = llvm::Type::getInt8Ty(m_Context);
    }
    else if (typeRef.name == "i16")
    {
        baseType = llvm::Type::getInt16Ty(m_Context);
    }
    else if (typeRef.name == "i32" || typeRef.name == "int32")
    {
        baseType = llvm::Type::getInt32Ty(m_Context);
    }
    else if (typeRef.name == "i64")
    {
        baseType = llvm::Type::getInt64Ty(m_Context);
    }
    else if (typeRef.name == "u8")
    {
        baseType = llvm::Type::getInt8Ty(m_Context);
    }
    else if (typeRef.name == "u16")
    {
        baseType = llvm::Type::getInt16Ty(m_Context);
    }
    else if (typeRef.name == "u32")
    {
        baseType = llvm::Type::getInt32Ty(m_Context);
    }
    else if (typeRef.name == "u64")
    {
        baseType = llvm::Type::getInt64Ty(m_Context);
    }
    else if (typeRef.name == "f32")
    {
        baseType = llvm::Type::getFloatTy(m_Context);
    }
    else if (typeRef.name == "f64")
    {
        baseType = llvm::Type::getDoubleTy(m_Context);
    }
    else if (typeRef.name == "bool")
    {
        baseType = llvm::Type::getInt1Ty(m_Context);
    }
    else if (typeRef.name == "char")
    {
        baseType = llvm::Type::getInt8Ty(m_Context);
    }
    else
    {
        // User-defined type (struct) - use opaque pointer for now
        baseType = llvm::Type::getInt8Ty(m_Context);
    }

    if (typeRef.isPointer)
    {
        return llvm::PointerType::getUnqual(baseType);
    }

    return baseType;
}

} // namespace jlang
