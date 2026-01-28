#include "CodeGen.h"

#include "../Common/Logger.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

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
    // Clear tracking for new function scope
    m_currentFunctionVariables.clear();

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
        const std::string &paramName = node.params[paramIndex].name;
        arg.setName(paramName);
        m_variables[paramName] = VariableInfo{&arg, node.params[paramIndex].type, false};
        m_currentFunctionVariables.insert(paramName);
        ++paramIndex;
    }

    if (node.body)
    {
        node.body->Accept(*this);
    }

    // Check for unused variables before finalizing the function
    CheckUnusedVariables();

    if (node.returnType.name == "void")
    {
        m_IRBuilder.CreateRetVoid();
    }

    llvm::verifyFunction(*function);

    // Clean up function-local variables
    for (const auto &varName : m_currentFunctionVariables)
    {
        m_variables.erase(varName);
    }
    m_currentFunctionVariables.clear();
}

void CodeGenerator::VisitInterfaceDecl(InterfaceDecl &) {}

void CodeGenerator::VisitStructDecl(StructDecl &node)
{
    // Create LLVM struct type
    std::vector<llvm::Type *> fieldTypes;
    fieldTypes.reserve(node.fields.size());

    StructInfo structInfo;

    for (unsigned i = 0; i < node.fields.size(); ++i)
    {
        const auto &field = node.fields[i];
        llvm::Type *fieldType = MapType(field.type);
        fieldTypes.push_back(fieldType);

        structInfo.fields[field.name] = FieldInfo{i, field.type, field.isPublic};
    }

    llvm::StructType *structType = llvm::StructType::create(m_Context, fieldTypes, node.name);
    structInfo.llvmType = structType;

    m_structTypes[node.name] = structInfo;
}

void CodeGenerator::VisitVariableDecl(VariableDecl &node)
{
    llvm::Type *varType = nullptr;
    TypeRef inferredType = node.varType;

    // Check if type inference is needed (empty type name)
    if (node.varType.name.empty())
    {
        if (!node.initializer)
        {
            JLANG_ERROR(STR("Type inference requires an initializer for variable: %s", node.name.c_str()));
            return;
        }

        // Evaluate initializer first to get its type
        node.initializer->Accept(*this);
        if (!m_LastValue)
        {
            JLANG_ERROR(STR("Invalid initializer for variable: %s", node.name.c_str()));
            return;
        }

        varType = m_LastValue->getType();
        inferredType = InferTypeRef(varType);

        // Create alloca and store the already-computed value
        llvm::AllocaInst *alloca = m_IRBuilder.CreateAlloca(varType, nullptr, node.name);
        m_IRBuilder.CreateStore(m_LastValue, alloca);

        // Track variable with inferred type
        m_variables[node.name] = VariableInfo{alloca, inferredType, false, node.isMutable};
        m_currentFunctionVariables.insert(node.name);
        return;
    }

    // Explicit type - use existing logic
    varType = MapType(node.varType);

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

    // Track variable with usage info (initially unused)
    m_variables[node.name] = VariableInfo{alloca, node.varType, false, node.isMutable};
    m_currentFunctionVariables.insert(node.name);
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

void CodeGenerator::VisitForStatement(ForStatement &node)
{
    // Execute initializer (if present)
    if (node.init)
    {
        node.init->Accept(*this);
    }

    llvm::Function *parentFunction = m_IRBuilder.GetInsertBlock()->getParent();

    llvm::BasicBlock *condBlock = llvm::BasicBlock::Create(m_Context, "for.cond", parentFunction);
    llvm::BasicBlock *bodyBlock = llvm::BasicBlock::Create(m_Context, "for.body");
    llvm::BasicBlock *updateBlock = llvm::BasicBlock::Create(m_Context, "for.update");
    llvm::BasicBlock *exitBlock = llvm::BasicBlock::Create(m_Context, "for.exit");

    // Branch to condition block
    m_IRBuilder.CreateBr(condBlock);

    // Condition block
    m_IRBuilder.SetInsertPoint(condBlock);
    if (node.condition)
    {
        node.condition->Accept(*this);
        llvm::Value *condValue = m_LastValue;

        if (!condValue)
        {
            JLANG_ERROR("Invalid condition in for statement");
            return;
        }

        // Convert i32 to i1 if necessary
        if (condValue->getType()->isIntegerTy(32))
        {
            condValue = m_IRBuilder.CreateICmpNE(condValue, llvm::ConstantInt::get(condValue->getType(), 0),
                                                 "forcond");
        }

        m_IRBuilder.CreateCondBr(condValue, bodyBlock, exitBlock);
    }
    else
    {
        // No condition = infinite loop
        m_IRBuilder.CreateBr(bodyBlock);
    }

    // Body block
    bodyBlock->insertInto(parentFunction);
    m_IRBuilder.SetInsertPoint(bodyBlock);
    node.body->Accept(*this);
    m_IRBuilder.CreateBr(updateBlock);

    // Update block
    updateBlock->insertInto(parentFunction);
    m_IRBuilder.SetInsertPoint(updateBlock);
    if (node.update)
    {
        node.update->Accept(*this);
    }
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
    // Handle short-circuit operators separately - they must not evaluate RHS eagerly
    if (node.op == "&&")
    {
        // Short-circuit AND: if left is false, result is false; otherwise evaluate right
        node.left->Accept(*this);
        llvm::Value *leftVal = m_LastValue;

        if (!leftVal)
        {
            JLANG_ERROR("Invalid left operand in && expression");
            return;
        }

        llvm::Function *parentFunction = m_IRBuilder.GetInsertBlock()->getParent();

        llvm::BasicBlock *rhsBlock = llvm::BasicBlock::Create(m_Context, "and.rhs", parentFunction);
        llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(m_Context, "and.merge");

        // Convert left to i1 if needed
        llvm::Value *leftBool = leftVal;
        if (!leftVal->getType()->isIntegerTy(1))
        {
            leftBool =
                m_IRBuilder.CreateICmpNE(leftVal, llvm::ConstantInt::get(leftVal->getType(), 0), "tobool");
        }

        llvm::BasicBlock *entryBlock = m_IRBuilder.GetInsertBlock();
        m_IRBuilder.CreateCondBr(leftBool, rhsBlock, mergeBlock);

        // RHS block - only now evaluate the right operand
        m_IRBuilder.SetInsertPoint(rhsBlock);
        node.right->Accept(*this);
        llvm::Value *rightVal = m_LastValue;

        if (!rightVal)
        {
            JLANG_ERROR("Invalid right operand in && expression");
            return;
        }

        llvm::Value *rightBool = rightVal;
        if (!rightVal->getType()->isIntegerTy(1))
        {
            rightBool =
                m_IRBuilder.CreateICmpNE(rightVal, llvm::ConstantInt::get(rightVal->getType(), 0), "tobool");
        }
        llvm::BasicBlock *rhsEndBlock = m_IRBuilder.GetInsertBlock();
        m_IRBuilder.CreateBr(mergeBlock);

        // Merge block
        mergeBlock->insertInto(parentFunction);
        m_IRBuilder.SetInsertPoint(mergeBlock);

        llvm::PHINode *phi = m_IRBuilder.CreatePHI(llvm::Type::getInt1Ty(m_Context), 2, "and.result");
        phi->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt1Ty(m_Context), 0), entryBlock);
        phi->addIncoming(rightBool, rhsEndBlock);

        m_LastValue = phi;
        return;
    }

    if (node.op == "||")
    {
        // Short-circuit OR: if left is true, result is true; otherwise evaluate right
        node.left->Accept(*this);
        llvm::Value *leftVal = m_LastValue;

        if (!leftVal)
        {
            JLANG_ERROR("Invalid left operand in || expression");
            return;
        }

        llvm::Function *parentFunction = m_IRBuilder.GetInsertBlock()->getParent();

        llvm::BasicBlock *rhsBlock = llvm::BasicBlock::Create(m_Context, "or.rhs", parentFunction);
        llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(m_Context, "or.merge");

        // Convert left to i1 if needed
        llvm::Value *leftBool = leftVal;
        if (!leftVal->getType()->isIntegerTy(1))
        {
            leftBool =
                m_IRBuilder.CreateICmpNE(leftVal, llvm::ConstantInt::get(leftVal->getType(), 0), "tobool");
        }

        llvm::BasicBlock *entryBlock = m_IRBuilder.GetInsertBlock();
        m_IRBuilder.CreateCondBr(leftBool, mergeBlock, rhsBlock);

        // RHS block - only now evaluate the right operand
        m_IRBuilder.SetInsertPoint(rhsBlock);
        node.right->Accept(*this);
        llvm::Value *rightVal = m_LastValue;

        if (!rightVal)
        {
            JLANG_ERROR("Invalid right operand in || expression");
            return;
        }

        llvm::Value *rightBool = rightVal;
        if (!rightVal->getType()->isIntegerTy(1))
        {
            rightBool =
                m_IRBuilder.CreateICmpNE(rightVal, llvm::ConstantInt::get(rightVal->getType(), 0), "tobool");
        }
        llvm::BasicBlock *rhsEndBlock = m_IRBuilder.GetInsertBlock();
        m_IRBuilder.CreateBr(mergeBlock);

        // Merge block
        mergeBlock->insertInto(parentFunction);
        m_IRBuilder.SetInsertPoint(mergeBlock);

        llvm::PHINode *phi = m_IRBuilder.CreatePHI(llvm::Type::getInt1Ty(m_Context), 2, "or.result");
        phi->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt1Ty(m_Context), 1), entryBlock);
        phi->addIncoming(rightBool, rhsEndBlock);

        m_LastValue = phi;
        return;
    }

    // All other binary operators evaluate both sides
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
    else if (node.op == "<=")
    {
        m_LastValue = m_IRBuilder.CreateICmpSLE(leftVal, rightVal, "le");
    }
    else if (node.op == ">")
    {
        m_LastValue = m_IRBuilder.CreateICmpSGT(leftVal, rightVal, "gt");
    }
    else if (node.op == ">=")
    {
        m_LastValue = m_IRBuilder.CreateICmpSGE(leftVal, rightVal, "ge");
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
    else if (node.op == "%")
    {
        m_LastValue = m_IRBuilder.CreateSRem(leftVal, rightVal, "mod");
    }
    else if (node.op == "and")
    {
        // Non-short-circuit AND: both operands are always evaluated
        llvm::Value *leftBool = leftVal;
        if (!leftVal->getType()->isIntegerTy(1))
        {
            leftBool =
                m_IRBuilder.CreateICmpNE(leftVal, llvm::ConstantInt::get(leftVal->getType(), 0), "tobool");
        }

        llvm::Value *rightBool = rightVal;
        if (!rightVal->getType()->isIntegerTy(1))
        {
            rightBool =
                m_IRBuilder.CreateICmpNE(rightVal, llvm::ConstantInt::get(rightVal->getType(), 0), "tobool");
        }

        m_LastValue = m_IRBuilder.CreateAnd(leftBool, rightBool, "and.result");
    }
    else if (node.op == "or")
    {
        // Non-short-circuit OR: both operands are always evaluated
        llvm::Value *leftBool = leftVal;
        if (!leftVal->getType()->isIntegerTy(1))
        {
            leftBool =
                m_IRBuilder.CreateICmpNE(leftVal, llvm::ConstantInt::get(leftVal->getType(), 0), "tobool");
        }

        llvm::Value *rightBool = rightVal;
        if (!rightVal->getType()->isIntegerTy(1))
        {
            rightBool =
                m_IRBuilder.CreateICmpNE(rightVal, llvm::ConstantInt::get(rightVal->getType(), 0), "tobool");
        }

        m_LastValue = m_IRBuilder.CreateOr(leftBool, rightBool, "or.result");
    }
    else
    {
        JLANG_ERROR(STR("Unknown binary operator: %s", node.op.c_str()));
    }
}

void CodeGenerator::VisitUnaryExpr(UnaryExpr &node)
{
    node.operand->Accept(*this);
    llvm::Value *operandVal = m_LastValue;

    if (!operandVal)
    {
        JLANG_ERROR("Invalid operand in unary expression");
        return;
    }

    if (node.op == "!")
    {
        // Logical NOT
        llvm::Value *boolVal = operandVal;
        if (!operandVal->getType()->isIntegerTy(1))
        {
            boolVal = m_IRBuilder.CreateICmpNE(operandVal, llvm::ConstantInt::get(operandVal->getType(), 0),
                                               "tobool");
        }
        m_LastValue = m_IRBuilder.CreateXor(
            boolVal, llvm::ConstantInt::get(llvm::Type::getInt1Ty(m_Context), 1), "not");
    }
    else
    {
        JLANG_ERROR(STR("Unknown unary operator: %s", node.op.c_str()));
    }
}

void CodeGenerator::VisitLiteralExpr(LiteralExpr &node)
{
    if (node.value == "NULL" || node.value == "null" || node.value == "nullptr")
    {
        m_LastValue =
            llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(m_Context)));
    }
    else if (node.value == "true")
    {
        m_LastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(m_Context), 1);
    }
    else if (node.value == "false")
    {
        m_LastValue = llvm::ConstantInt::get(llvm::Type::getInt1Ty(m_Context), 0);
    }
    else if (node.value.front() == '"' && node.value.back() == '"')
    {
        // String literal
        std::string strValue = node.value.substr(1, node.value.size() - 2);
        m_LastValue = m_IRBuilder.CreateGlobalStringPtr(strValue, "str");
    }
    else if (node.value.front() == '\'' && node.value.back() == '\'')
    {
        // Character literal - extract the character and create an i8 constant
        char charValue = node.value[1];
        m_LastValue =
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(m_Context), static_cast<uint8_t>(charValue));
    }
    else if (node.value.find('.') != std::string::npos)
    {
        // Float literal
        try
        {
            double floatValue = std::stod(node.value);
            // Default to f64 (double) for now
            m_LastValue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(m_Context), floatValue);
        }
        catch (...)
        {
            JLANG_ERROR(STR("Invalid float literal: %s", node.value.c_str()));
        }
    }
    else
    {
        // Integer literal
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
    auto it = m_variables.find(node.name);

    if (it == m_variables.end())
    {
        JLANG_ERROR(STR("Undefined variable: %s", node.name.c_str()));
        return;
    }

    // Mark variable as used
    it->second.used = true;

    llvm::Value *storedValue = it->second.value;

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

    // Calculate size based on the type being allocated
    uint64_t allocSize = 8; // Default size

    // Check if we're allocating a struct
    auto structIt = m_structTypes.find(node.allocType.name);
    if (structIt != m_structTypes.end())
    {
        // Get the size of the struct from LLVM's data layout
        llvm::StructType *structType = structIt->second.llvmType;
        const llvm::DataLayout &dataLayout = m_Module->getDataLayout();
        allocSize = dataLayout.getTypeAllocSize(structType);
    }

    llvm::Value *size = llvm::ConstantInt::get(llvm::Type::getInt64Ty(m_Context), allocSize);

    // Call malloc
    llvm::Value *allocated = m_IRBuilder.CreateCall(mallocFunc, {size}, "alloc");

    // Cast to the appropriate pointer type
    llvm::Type *targetType = MapType(node.allocType);
    if (allocated->getType() != targetType)
    {
        allocated = m_IRBuilder.CreateBitCast(allocated, targetType, "alloc_cast");
    }

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
    auto it = m_variables.find(node.name);
    if (it == m_variables.end())
    {
        JLANG_ERROR(STR("Undefined variable in assignment: %s", node.name.c_str()));
        return;
    }

    if (!it->second.isMutable)
    {
        JLANG_ERROR(STR("Cannot assign to immutable variable '%s' (declared with 'val')", node.name.c_str()));
        return;
    }

    llvm::Value *targetVar = it->second.value;

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

void CodeGenerator::VisitMemberAccessExpr(MemberAccessExpr &node)
{
    // Get the object (should be a pointer to a struct)
    node.object->Accept(*this);
    llvm::Value *objectPtr = m_LastValue;

    if (!objectPtr)
    {
        JLANG_ERROR("Invalid object in member access");
        return;
    }

    // Determine the struct type from the object
    // We need to trace back to find the struct type
    std::string structTypeName;

    // If the object is a VarExpr, look up its type
    if (auto *varExpr = dynamic_cast<VarExpr *>(node.object.get()))
    {
        auto typeIt = m_variables.find(varExpr->name);
        if (typeIt != m_variables.end())
        {
            structTypeName = typeIt->second.type.name;
        }
    }

    if (structTypeName.empty())
    {
        JLANG_ERROR("Cannot determine struct type for member access");
        return;
    }

    // Find the struct info
    auto structIt = m_structTypes.find(structTypeName);
    if (structIt == m_structTypes.end())
    {
        JLANG_ERROR(STR("Unknown struct type: %s", structTypeName.c_str()));
        return;
    }

    const StructInfo &structInfo = structIt->second;

    // Find the field
    auto fieldIt = structInfo.fields.find(node.memberName);
    if (fieldIt == structInfo.fields.end())
    {
        JLANG_ERROR(
            STR("Unknown field '%s' in struct '%s'", node.memberName.c_str(), structTypeName.c_str()));
        return;
    }

    const FieldInfo &fieldInfo = fieldIt->second;

    // Check visibility - private fields (lowercase) can only be accessed from within the struct's methods
    // For now, we'll allow all access but emit a warning for private fields
    // A proper implementation would track the current context
    if (!fieldInfo.isPublic)
    {
        JLANG_ERROR(STR("Cannot access private field '%s' in struct '%s'", node.memberName.c_str(),
                        structTypeName.c_str()));
        return;
    }

    // Generate GEP to access the field
    llvm::Value *fieldPtr = m_IRBuilder.CreateStructGEP(structInfo.llvmType, objectPtr, fieldInfo.index,
                                                        node.memberName + "_ptr");

    // Load the field value
    llvm::Type *fieldType = MapType(fieldInfo.type);
    m_LastValue = m_IRBuilder.CreateLoad(fieldType, fieldPtr, node.memberName);
}

void CodeGenerator::VisitPrefixExpr(PrefixExpr &node)
{
    // Prefix ++/--: increment/decrement and return the NEW value
    auto *varExpr = dynamic_cast<VarExpr *>(node.operand.get());
    if (!varExpr)
    {
        JLANG_ERROR("Prefix increment/decrement requires a variable operand");
        return;
    }

    auto it = m_variables.find(varExpr->name);
    if (it == m_variables.end())
    {
        JLANG_ERROR(STR("Undefined variable: %s", varExpr->name.c_str()));
        return;
    }

    if (!it->second.isMutable)
    {
        JLANG_ERROR(
            STR("Cannot modify immutable variable '%s' (declared with 'val')", varExpr->name.c_str()));
        return;
    }

    // Mark variable as used
    it->second.used = true;

    llvm::Value *varPtr = it->second.value;
    llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(varPtr);
    if (!alloca)
    {
        JLANG_ERROR("Cannot increment/decrement non-variable");
        return;
    }

    // Load current value
    llvm::Value *currentVal = m_IRBuilder.CreateLoad(alloca->getAllocatedType(), alloca, "load");

    // Add or subtract 1
    llvm::Value *one = llvm::ConstantInt::get(currentVal->getType(), 1);
    llvm::Value *newVal;
    if (node.op == "++")
    {
        newVal = m_IRBuilder.CreateAdd(currentVal, one, "inc");
    }
    else
    {
        newVal = m_IRBuilder.CreateSub(currentVal, one, "dec");
    }

    // Store new value back
    m_IRBuilder.CreateStore(newVal, alloca);

    // Return the NEW value
    m_LastValue = newVal;
}

void CodeGenerator::VisitPostfixExpr(PostfixExpr &node)
{
    // Postfix ++/--: increment/decrement and return the ORIGINAL value
    auto *varExpr = dynamic_cast<VarExpr *>(node.operand.get());
    if (!varExpr)
    {
        JLANG_ERROR("Postfix increment/decrement requires a variable operand");
        return;
    }

    auto it = m_variables.find(varExpr->name);
    if (it == m_variables.end())
    {
        JLANG_ERROR(STR("Undefined variable: %s", varExpr->name.c_str()));
        return;
    }

    if (!it->second.isMutable)
    {
        JLANG_ERROR(
            STR("Cannot modify immutable variable '%s' (declared with 'val')", varExpr->name.c_str()));
        return;
    }

    // Mark variable as used
    it->second.used = true;

    llvm::Value *varPtr = it->second.value;
    llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(varPtr);
    if (!alloca)
    {
        JLANG_ERROR("Cannot increment/decrement non-variable");
        return;
    }

    // Load current value
    llvm::Value *currentVal = m_IRBuilder.CreateLoad(alloca->getAllocatedType(), alloca, "load");

    // Add or subtract 1
    llvm::Value *one = llvm::ConstantInt::get(currentVal->getType(), 1);
    llvm::Value *newVal;
    if (node.op == "++")
    {
        newVal = m_IRBuilder.CreateAdd(currentVal, one, "inc");
    }
    else
    {
        newVal = m_IRBuilder.CreateSub(currentVal, one, "dec");
    }

    // Store new value back
    m_IRBuilder.CreateStore(newVal, alloca);

    // Return the ORIGINAL value
    m_LastValue = currentVal;
}

void CodeGenerator::CheckUnusedVariables()
{
    // TODO: make this a cool hard error that stops compilation (like Go)
    // for now just complain a bit
    for (const auto &varName : m_currentFunctionVariables)
    {
        auto it = m_variables.find(varName);
        if (it != m_variables.end() && !it->second.used)
        {
            JLANG_ERROR(STR("Unused variable: %s", varName.c_str()));
        }
    }
}

llvm::Type *CodeGenerator::MapType(const TypeRef &typeRef)
{
    using TypeGetter = llvm::Type *(*)(llvm::LLVMContext &);

    static const std::unordered_map<std::string, TypeGetter> typeMap = {
        {"i8", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt8Ty(ctx); }},
        {"u8", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt8Ty(ctx); }},
        {"char", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt8Ty(ctx); }},
        {"i16", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt16Ty(ctx); }},
        {"u16", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt16Ty(ctx); }},
        {"i32", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt32Ty(ctx); }},
        {"u32", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt32Ty(ctx); }},
        {"i64", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt64Ty(ctx); }},
        {"u64", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt64Ty(ctx); }},
        {"f32", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getFloatTy(ctx); }},
        {"f64", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getDoubleTy(ctx); }},
        {"bool", +[](llvm::LLVMContext &ctx) -> llvm::Type * { return llvm::Type::getInt1Ty(ctx); }},
    };

    if (typeRef.name == "void")
    {
        return llvm::Type::getVoidTy(m_Context);
    }

    llvm::Type *baseType = nullptr;
    auto it = typeMap.find(typeRef.name);
    if (it != typeMap.end())
    {
        baseType = it->second(m_Context);
    }
    else
    {
        // Check if it's a registered struct type
        auto structIt = m_structTypes.find(typeRef.name);
        if (structIt != m_structTypes.end())
        {
            baseType = structIt->second.llvmType;
        }
        else
        {
            // Unknown user-defined type - use i8 as placeholder
            baseType = llvm::Type::getInt8Ty(m_Context);
        }
    }

    if (typeRef.isPointer)
    {
        return llvm::PointerType::getUnqual(baseType);
    }

    return baseType;
}

TypeRef CodeGenerator::InferTypeRef(llvm::Type *llvmType)
{
    // Handle pointer types
    if (llvmType->isPointerTy())
    {
        // In opaque pointer mode (LLVM 15+), we can't get the element type
        // Default to i8* (char*) for generic pointers
        return TypeRef{"char", true};
    }

    // Handle integer types
    if (llvmType->isIntegerTy())
    {
        unsigned bitWidth = llvmType->getIntegerBitWidth();
        switch (bitWidth)
        {
        case 1:
            return TypeRef{"bool", false};
        case 8:
            return TypeRef{"i8", false};
        case 16:
            return TypeRef{"i16", false};
        case 32:
            return TypeRef{"i32", false};
        case 64:
            return TypeRef{"i64", false};
        default:
            return TypeRef{"i32", false}; // Default to i32
        }
    }

    // Handle floating point types
    if (llvmType->isFloatTy())
    {
        return TypeRef{"f32", false};
    }
    if (llvmType->isDoubleTy())
    {
        return TypeRef{"f64", false};
    }

    // Handle struct types
    if (auto *structType = llvm::dyn_cast<llvm::StructType>(llvmType))
    {
        if (structType->hasName())
        {
            return TypeRef{structType->getName().str(), false};
        }
    }

    // Default fallback
    return TypeRef{"i32", false};
}

} // namespace jlang
