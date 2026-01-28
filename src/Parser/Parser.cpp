#include "Parser.h"

#include "../Common/Logger.h"

#include <cctype>
#include <unordered_set>

namespace jlang
{

Parser::Parser(const std::vector<Token> &tokens) : m_Tokens(tokens), m_CurrentPosition(0) {}

std::vector<std::shared_ptr<AstNode>> Parser::Parse()
{
    std::vector<std::shared_ptr<AstNode>> program;

    while (!IsEndReached())
    {
        auto declaration = ParseDeclaration();

        if (declaration)
        {
            program.push_back(declaration);
        }
    }

    return program;
}

bool Parser::IsMatched(TokenType type)
{
    if (Check(type))
    {
        Advance();
        return true;
    }

    return false;
}

bool Parser::Check(TokenType type) const
{
    if (IsEndReached())
    {
        return false;
    }

    return Peek().m_type == type;
}

const Token &Parser::Advance()
{
    if (!IsEndReached())
    {
        m_CurrentPosition++;
    }

    return Previous();
}

const Token &Parser::Peek() const
{
    return m_Tokens[m_CurrentPosition];
}

const Token &Parser::Previous() const
{
    return m_Tokens[m_CurrentPosition - 1];
}

bool Parser::IsEndReached() const
{
    return Peek().m_type == TokenType::EndOfFile;
}

std::shared_ptr<AstNode> Parser::ParseDeclaration()
{
    if (Check(TokenType::Interface))
    {
        return ParseInterface();
    }

    if (Check(TokenType::Struct))
    {
        return ParseStruct();
    }

    if (Check(TokenType::Fn))
    {
        return ParseFunction();
    }

    Advance();

    return nullptr;
}

std::shared_ptr<AstNode> Parser::ParseInterface()
{
    Advance();

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR("Expected interface name");
    }

    const std::string &name = Previous().m_lexeme;

    if (!IsMatched(TokenType::LBrace))
    {
        JLANG_ERROR("Expected '{' after interface name!");
    }

    auto interfaceDeclNode = std::make_shared<InterfaceDecl>();
    interfaceDeclNode->name = name;

    while (!Check(TokenType::RBrace) && !IsEndReached())
    {
        if (!IsMatched(TokenType::Fn))
        {
            JLANG_ERROR("Expected 'fn' in interface method");
            Advance(); // error recovery
            continue;
        }

        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected method name");
            Advance(); // error recovery
            continue;
        }

        std::string methodName = Previous().m_lexeme;

        if (!IsMatched(TokenType::LParen) || !IsMatched(TokenType::RParen) ||
            !IsMatched(TokenType::Semicolon))
        {
            JLANG_ERROR("Expected '()' and ';' after method name");
            // Skip to semicolon for error recovery
            while (!IsEndReached() && !Check(TokenType::Semicolon) && !Check(TokenType::RBrace))
            {
                Advance();
            }
            IsMatched(TokenType::Semicolon);
            continue;
        }

        interfaceDeclNode->methods.push_back(methodName);
    }

    if (!IsMatched(TokenType::RBrace))
    {
        JLANG_ERROR("Expected '}' at end of interface");
    }

    return interfaceDeclNode;
}

std::shared_ptr<AstNode> Parser::ParseStruct()
{
    Advance(); // consume 'struct'

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR("Expected struct name");
    }

    const std::string name = Previous().m_lexeme;

    std::string implementedInterface;

    // Use colon for interface implementation: struct Name : Interface
    if (IsMatched(TokenType::Colon))
    {
        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected interface name after ':'");
        }

        implementedInterface = Previous().m_lexeme;
    }

    if (!IsMatched(TokenType::LBrace))
    {
        JLANG_ERROR("Expected '{' after struct declaration");
    }

    auto structDeclNode = std::make_shared<StructDecl>();
    structDeclNode->name = name;
    structDeclNode->interfaceImplemented = implementedInterface;

    // Parse fields: fieldName: Type;
    while (!Check(TokenType::RBrace) && !IsEndReached())
    {
        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected field name");
            // Skip to next semicolon or brace
            while (!IsEndReached() && !Check(TokenType::Semicolon) && !Check(TokenType::RBrace))
            {
                Advance();
            }
            IsMatched(TokenType::Semicolon);
            continue;
        }

        std::string fieldName = Previous().m_lexeme;

        // Expect colon after field name
        if (!IsMatched(TokenType::Colon))
        {
            JLANG_ERROR("Expected ':' after field name");
            while (!IsEndReached() && !Check(TokenType::Semicolon) && !Check(TokenType::RBrace))
            {
                Advance();
            }
            IsMatched(TokenType::Semicolon);
            continue;
        }

        // Parse field type
        std::string typeName = ParseTypeName();
        if (typeName.empty())
        {
            JLANG_ERROR("Expected field type");
            while (!IsEndReached() && !Check(TokenType::Semicolon) && !Check(TokenType::RBrace))
            {
                Advance();
            }
            IsMatched(TokenType::Semicolon);
            continue;
        }

        bool isPointer = IsMatched(TokenType::Star);

        if (!IsMatched(TokenType::Semicolon))
        {
            JLANG_ERROR("Expected ';' after struct field");
        }

        bool isPublic = !fieldName.empty() && std::isupper(static_cast<unsigned char>(fieldName[0]));
        StructField field{fieldName, TypeRef{typeName, isPointer}, isPublic};
        structDeclNode->fields.push_back(field);
    }

    if (!IsMatched(TokenType::RBrace))
    {
        JLANG_ERROR("Expected '}' after struct body");
    }

    return structDeclNode;
}

std::shared_ptr<AstNode> Parser::ParseFunction()
{
    Advance(); // consume 'fn'

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR("Expected function name!");
    }

    const std::string &functionName = Previous().m_lexeme;

    // Parse parameter list: (name: Type, name: Type, ...)
    if (!IsMatched(TokenType::LParen))
    {
        JLANG_ERROR("Expected '(' after function name");
    }

    std::vector<Parameter> params;

    if (!Check(TokenType::RParen))
    {
        do
        {
            if (!IsMatched(TokenType::Identifier))
            {
                JLANG_ERROR("Expected parameter name");
                break;
            }
            std::string paramName = Previous().m_lexeme;

            if (!IsMatched(TokenType::Colon))
            {
                JLANG_ERROR("Expected ':' after parameter name");
                break;
            }

            std::string typeName = ParseTypeName();
            if (typeName.empty())
            {
                JLANG_ERROR("Expected parameter type");
                break;
            }

            bool isPointer = IsMatched(TokenType::Star);

            params.push_back(Parameter{paramName, TypeRef{typeName, isPointer}});

        } while (IsMatched(TokenType::Comma));
    }

    if (!IsMatched(TokenType::RParen))
    {
        JLANG_ERROR("Expected ')' after parameters");
    }

    // Parse return type (optional, after ->), default to void
    TypeRef returnType{"void", false};

    if (IsMatched(TokenType::Arrow))
    {
        std::string returnTypeName = ParseTypeName();
        if (returnTypeName.empty())
        {
            JLANG_ERROR("Expected return type after '->'");
        }
        bool isPointer = IsMatched(TokenType::Star);
        returnType = TypeRef{returnTypeName, isPointer};
    }

    auto body = ParseBlock();

    auto functionDeclNode = std::make_shared<FunctionDecl>();
    functionDeclNode->name = functionName;
    functionDeclNode->params = params;
    functionDeclNode->returnType = returnType;
    functionDeclNode->body = body;

    return functionDeclNode;
}

std::shared_ptr<AstNode> Parser::ParseBlock()
{
    if (!IsMatched(TokenType::LBrace))
    {
        JLANG_ERROR("Expected '{' at the beginning of the block");
    }

    auto blockStmt = std::make_shared<BlockStatement>();

    while (!Check(TokenType::RBrace) && !IsEndReached())
    {
        auto statement = ParseStatement();

        if (statement)
        {
            blockStmt->statements.push_back(statement);
        }
    }

    if (!IsMatched(TokenType::RBrace))
    {
        JLANG_ERROR("Expected '}' after block");
    }

    return blockStmt;
}

std::shared_ptr<AstNode> Parser::ParseStatement()
{
    if (Check(TokenType::If))
    {
        return ParseIfStatement();
    }

    if (Check(TokenType::While))
    {
        return ParseWhileStatement();
    }

    if (Check(TokenType::For))
    {
        return ParseForStatement();
    }

    if (Check(TokenType::Var) || Check(TokenType::Val))
    {
        return ParseVarDecl();
    }

    if (Check(TokenType::Return))
    {
        return ParseReturnStatement();
    }

    if (Check(TokenType::LBrace))
    {
        return ParseBlock();
    }

    return ParseExprStatement();
}

std::shared_ptr<AstNode> Parser::ParseReturnStatement()
{
    Advance(); // consume 'return'

    std::shared_ptr<AstNode> value = nullptr;

    // Check if there's a return value (not just "return;")
    if (!Check(TokenType::Semicolon))
    {
        value = ParseExpression();
    }

    if (!IsMatched(TokenType::Semicolon))
    {
        JLANG_ERROR("Expected ';' after return statement");
    }

    auto returnStmt = std::make_shared<ReturnStatement>();
    returnStmt->value = value;

    return returnStmt;
}

std::shared_ptr<AstNode> Parser::ParseVarDecl()
{
    bool isMutable = Check(TokenType::Var);
    Advance(); // consume 'var' or 'val'

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR("Expected variable name");
        Advance(); // error recovery
        return nullptr;
    }

    std::string varName = Previous().m_lexeme;

    std::string typeName;
    bool isPointer = false;
    std::shared_ptr<AstNode> initializer = nullptr;

    // Check for type inference syntax: var x := expr;
    if (IsMatched(TokenType::ColonEqual))
    {
        // Type inference - parse initializer, type will be inferred in CodeGen
        initializer = ParseExpression();
        if (!initializer)
        {
            JLANG_ERROR("Expected initializer expression after ':='");
            return nullptr;
        }
    }
    else
    {
        // Explicit type syntax: var x: Type = expr;
        if (!IsMatched(TokenType::Colon))
        {
            JLANG_ERROR("Expected ':' or ':=' after variable name");
            return nullptr;
        }

        // Parse type
        typeName = ParseTypeName();
        if (typeName.empty())
        {
            JLANG_ERROR("Expected variable type");
            return nullptr;
        }

        isPointer = IsMatched(TokenType::Star);

        if (IsMatched(TokenType::Equal))
        {
            initializer = ParseExpression();
        }
    }

    if (!IsMatched(TokenType::Semicolon))
    {
        JLANG_ERROR("Expected ';' after variable declaration");
    }

    auto varDecl = std::make_shared<VariableDecl>();
    varDecl->name = varName;
    varDecl->varType = TypeRef{typeName, isPointer};
    varDecl->initializer = initializer;
    varDecl->isMutable = isMutable;

    return varDecl;
}

std::shared_ptr<AstNode> Parser::ParseIfStatement()
{
    Advance();

    if (!IsMatched(TokenType::LParen))
    {
        JLANG_ERROR("Expected '(' after 'if'");
    }

    auto condition = ParseExpression();

    if (!IsMatched(TokenType::RParen))
    {
        JLANG_ERROR("Expected ')' after condition");
    }

    auto thenBranch = ParseStatement();

    std::shared_ptr<AstNode> elseBranch = nullptr;
    if (IsMatched(TokenType::Else))
    {
        elseBranch = ParseStatement();
    }

    auto node = std::make_shared<IfStatement>();

    node->condition = condition;
    node->thenBranch = thenBranch;
    node->elseBranch = elseBranch;

    return node;
}

std::shared_ptr<AstNode> Parser::ParseWhileStatement()
{
    Advance(); // consume 'while'

    if (!IsMatched(TokenType::LParen))
    {
        JLANG_ERROR("Expected '(' after 'while'");
    }

    auto condition = ParseExpression();

    if (!IsMatched(TokenType::RParen))
    {
        JLANG_ERROR("Expected ')' after condition");
    }

    auto body = ParseStatement();

    auto node = std::make_shared<WhileStatement>();
    node->condition = condition;
    node->body = body;

    return node;
}

std::shared_ptr<AstNode> Parser::ParseForStatement()
{
    Advance(); // consume 'for'

    if (!IsMatched(TokenType::LParen))
    {
        JLANG_ERROR("Expected '(' after 'for'");
    }

    // Parse initializer (var decl or expression statement, or empty)
    std::shared_ptr<AstNode> init = nullptr;
    if (Check(TokenType::Semicolon))
    {
        Advance(); // empty initializer
    }
    else if (Check(TokenType::Var) || Check(TokenType::Val))
    {
        init = ParseVarDecl(); // already consumes semicolon
    }
    else
    {
        init = ParseExprStatement(); // already consumes semicolon
    }

    // Parse condition (or empty for infinite loop)
    std::shared_ptr<AstNode> condition = nullptr;
    if (!Check(TokenType::Semicolon))
    {
        condition = ParseExpression();
    }

    if (!IsMatched(TokenType::Semicolon))
    {
        JLANG_ERROR("Expected ';' after for loop condition");
    }

    // Parse update expression (or empty)
    std::shared_ptr<AstNode> update = nullptr;
    if (!Check(TokenType::RParen))
    {
        update = ParseExpression();
    }

    if (!IsMatched(TokenType::RParen))
    {
        JLANG_ERROR("Expected ')' after for clauses");
    }

    auto body = ParseStatement();

    auto node = std::make_shared<ForStatement>();
    node->init = init;
    node->condition = condition;
    node->update = update;
    node->body = body;

    return node;
}

std::shared_ptr<AstNode> Parser::ParseExpression()
{
    auto expr = ParseLogicalOr();

    // Handle assignment: identifier = expression
    if (IsMatched(TokenType::Equal))
    {
        auto value = ParseExpression();

        // Check if left side is a variable
        if (auto varExpr = std::dynamic_pointer_cast<VarExpr>(expr))
        {
            auto assign = std::make_shared<AssignExpr>();
            assign->name = varExpr->name;
            assign->value = value;
            return assign;
        }
        else
        {
            JLANG_ERROR("Invalid assignment target");
        }
    }

    // Handle compound assignment: x += expr  =>  x = x + expr
    std::string compoundOp;
    if (IsMatched(TokenType::PlusEqual))
    {
        compoundOp = "+";
    }
    else if (IsMatched(TokenType::MinusEqual))
    {
        compoundOp = "-";
    }
    else if (IsMatched(TokenType::StarEqual))
    {
        compoundOp = "*";
    }
    else if (IsMatched(TokenType::SlashEqual))
    {
        compoundOp = "/";
    }
    else if (IsMatched(TokenType::PercentEqual))
    {
        compoundOp = "%";
    }

    if (!compoundOp.empty())
    {
        auto rhs = ParseExpression();

        if (auto varExpr = std::dynamic_pointer_cast<VarExpr>(expr))
        {
            auto binary = std::make_shared<BinaryExpr>();
            binary->op = compoundOp;
            binary->left = expr;
            binary->right = rhs;

            auto assign = std::make_shared<AssignExpr>();
            assign->name = varExpr->name;
            assign->value = binary;
            return assign;
        }
        else
        {
            JLANG_ERROR("Invalid compound assignment target");
        }
    }

    return expr;
}

std::shared_ptr<AstNode> Parser::ParseLogicalOr()
{
    auto left = ParseLogicalAnd();

    while (Check(TokenType::Or) || Check(TokenType::OrKeyword))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto right = ParseLogicalAnd();

        auto binary = std::make_shared<BinaryExpr>();
        binary->op = op;
        binary->left = left;
        binary->right = right;
        left = binary;
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseLogicalAnd()
{
    auto left = ParseEquality();

    while (Check(TokenType::And) || Check(TokenType::AndKeyword))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto right = ParseEquality();

        auto binary = std::make_shared<BinaryExpr>();
        binary->op = op;
        binary->left = left;
        binary->right = right;
        left = binary;
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseEquality()
{
    auto left = ParseComparison();

    while (Check(TokenType::EqualEqual) || Check(TokenType::NotEqual))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto right = ParseComparison();

        auto binary = std::make_shared<BinaryExpr>();
        binary->op = op;
        binary->left = left;
        binary->right = right;
        left = binary;
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseComparison()
{
    auto left = ParseAdditive();

    while (Check(TokenType::Less) || Check(TokenType::LessEqual) || Check(TokenType::Greater) ||
           Check(TokenType::GreaterEqual))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto right = ParseAdditive();

        auto binary = std::make_shared<BinaryExpr>();
        binary->op = op;
        binary->left = left;
        binary->right = right;
        left = binary;
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseAdditive()
{
    auto left = ParseMultiplicative();

    while (Check(TokenType::Plus) || Check(TokenType::Minus))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto right = ParseMultiplicative();

        auto binary = std::make_shared<BinaryExpr>();
        binary->op = op;
        binary->left = left;
        binary->right = right;
        left = binary;
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseMultiplicative()
{
    auto left = ParseUnary();

    while (Check(TokenType::Star) || Check(TokenType::Slash) || Check(TokenType::Percent))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto right = ParseUnary();

        auto binary = std::make_shared<BinaryExpr>();
        binary->op = op;
        binary->left = left;
        binary->right = right;
        left = binary;
    }

    return left;
}

std::shared_ptr<AstNode> Parser::ParseUnary()
{
    if (Check(TokenType::Not))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto operand = ParseUnary();

        auto unary = std::make_shared<UnaryExpr>();
        unary->op = op;
        unary->operand = operand;
        return unary;
    }

    // Handle prefix increment/decrement
    if (Check(TokenType::PlusPlus) || Check(TokenType::MinusMinus))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto operand = ParseUnary();

        auto prefix = std::make_shared<PrefixExpr>();
        prefix->op = op;
        prefix->operand = operand;
        return prefix;
    }

    return ParsePostfix();
}

std::shared_ptr<AstNode> Parser::ParsePostfix()
{
    auto expr = ParsePrimary();

    // Handle postfix increment/decrement
    while (Check(TokenType::PlusPlus) || Check(TokenType::MinusMinus))
    {
        std::string op = Peek().m_lexeme;
        Advance();

        auto postfix = std::make_shared<PostfixExpr>();
        postfix->op = op;
        postfix->operand = expr;
        expr = postfix;
    }

    return expr;
}

std::shared_ptr<AstNode> Parser::ParseExprStatement()
{
    auto expression = ParseExpression();

    if (!expression)
    {
        // Skip until we find a semicolon or closing brace for error recovery
        while (!IsEndReached() && !Check(TokenType::Semicolon) && !Check(TokenType::RBrace))
        {
            Advance();
        }
        IsMatched(TokenType::Semicolon);
        return nullptr;
    }

    if (!IsMatched(TokenType::Semicolon))
    {
        JLANG_ERROR("Expected ';' after expression");
    }

    auto stmt = std::make_shared<ExprStatement>();
    stmt->expression = expression;

    return stmt;
}

std::shared_ptr<AstNode> Parser::ParsePrimary()
{
    // Handle null literal (keyword)
    if (IsMatched(TokenType::Null))
    {
        auto literal = std::make_shared<LiteralExpr>();
        literal->value = "null";
        return literal;
    }

    // Handle true literal
    if (IsMatched(TokenType::True))
    {
        auto literal = std::make_shared<LiteralExpr>();
        literal->value = "true";
        return literal;
    }

    // Handle false literal
    if (IsMatched(TokenType::False))
    {
        auto literal = std::make_shared<LiteralExpr>();
        literal->value = "false";
        return literal;
    }

    // Handle alloc<Type>() expression
    if (IsMatched(TokenType::Alloc))
    {
        if (!IsMatched(TokenType::Less))
        {
            JLANG_ERROR("Expected '<' after alloc");
            return nullptr;
        }

        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected type name in alloc<Type>");
            return nullptr;
        }

        std::string typeName = Previous().m_lexeme;

        if (!IsMatched(TokenType::Greater))
        {
            JLANG_ERROR("Expected '>' after type in alloc<Type>");
            return nullptr;
        }

        if (!IsMatched(TokenType::LParen) || !IsMatched(TokenType::RParen))
        {
            JLANG_ERROR("Expected '()' after alloc<Type>");
            return nullptr;
        }

        auto allocExpr = std::make_shared<AllocExpr>();
        allocExpr->allocType = TypeRef{typeName, true}; // always returns a pointer
        return allocExpr;
    }

    // Handle cast expressions: (struct Type*) expr or (Type*) expr
    if (IsMatched(TokenType::LParen))
    {
        // Check if this is a cast or a grouped expression
        if (Check(TokenType::Struct) || Check(TokenType::Identifier))
        {
            // This is likely a cast expression
            IsMatched(TokenType::Struct);

            if (!IsMatched(TokenType::Identifier))
            {
                JLANG_ERROR("Expected type name in cast");
                return nullptr;
            }

            std::string typeName = Previous().m_lexeme;
            bool isPointer = IsMatched(TokenType::Star);

            if (!IsMatched(TokenType::RParen))
            {
                JLANG_ERROR("Expected ')' after cast type");
                return nullptr;
            }

            auto expr = ParsePrimary();

            auto cast = std::make_shared<CastExpr>();
            cast->targetType = TypeRef{typeName, isPointer};
            cast->expr = expr;
            return cast;
        }
        else
        {
            // Grouped expression
            auto expr = ParseExpression();
            if (!IsMatched(TokenType::RParen))
            {
                JLANG_ERROR("Expected ')' after grouped expression");
            }
            return expr;
        }
    }

    // Handle identifiers, function calls, and member access
    if (IsMatched(TokenType::Identifier))
    {
        std::string name = Previous().m_lexeme;

        // Check for function call first (before member access)
        if (IsMatched(TokenType::LParen))
        {
            auto call = std::make_shared<CallExpr>();
            call->callee = name;

            if (!Check(TokenType::RParen))
            {
                do
                {
                    auto arg = ParseExpression();
                    call->arguments.push_back(arg);
                } while (IsMatched(TokenType::Comma));
            }

            if (!IsMatched(TokenType::RParen))
            {
                JLANG_ERROR("Expected ')' after arguments");
            }

            return call;
        }

        // Start with a variable expression
        std::shared_ptr<AstNode> expr = std::make_shared<VarExpr>();
        std::static_pointer_cast<VarExpr>(expr)->name = name;

        // Handle member access chain: obj.field1.field2
        while (IsMatched(TokenType::Dot))
        {
            if (!IsMatched(TokenType::Identifier))
            {
                JLANG_ERROR("Expected member name after '.'");
                break;
            }
            std::string memberName = Previous().m_lexeme;

            auto memberAccess = std::make_shared<MemberAccessExpr>();
            memberAccess->object = expr;
            memberAccess->memberName = memberName;
            expr = memberAccess;
        }

        return expr;
    }

    // Handle string literals
    if (IsMatched(TokenType::StringLiteral))
    {
        auto expression = std::make_shared<LiteralExpr>();
        expression->value = "\"" + Previous().m_lexeme + "\"";
        return expression;
    }

    // Handle number literals
    if (IsMatched(TokenType::NumberLiteral))
    {
        auto expression = std::make_shared<LiteralExpr>();
        expression->value = Previous().m_lexeme;
        return expression;
    }

    // Handle float literals
    if (IsMatched(TokenType::FloatLiteral))
    {
        auto expression = std::make_shared<LiteralExpr>();
        expression->value = Previous().m_lexeme;
        return expression;
    }

    // Handle character literals
    if (IsMatched(TokenType::CharLiteral))
    {
        auto expression = std::make_shared<LiteralExpr>();
        // Wrap in single quotes to distinguish from other literals in codegen
        expression->value = "'" + Previous().m_lexeme + "'";
        return expression;
    }

    JLANG_ERROR("Expected expression");
    return nullptr;
}

bool Parser::IsTypeKeyword() const
{
    static const std::unordered_set<TokenType> typeKeywords = {
        TokenType::Void, TokenType::I8,   TokenType::I16, TokenType::I32, TokenType::I64,
        TokenType::U8,   TokenType::U16,  TokenType::U32, TokenType::U64, TokenType::F32,
        TokenType::F64,  TokenType::Bool, TokenType::Char};
    return typeKeywords.count(Peek().m_type) > 0;
}

std::string Parser::ParseTypeName()
{
    if (IsTypeKeyword())
    {
        std::string name = Peek().m_lexeme;
        Advance();
        return name;
    }
    else if (IsMatched(TokenType::Identifier))
    {
        return Previous().m_lexeme;
    }

    return "";
}

} // namespace jlang
