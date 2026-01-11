#include "Parser.h"

#include "../Common/Logger.h"

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

// Everything is kinda hardcoded for now!! -> will change that later on, just trying to get stuff rolling..
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

    if (Check(TokenType::Void) || Check(TokenType::Int32))
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
        if (!IsMatched(TokenType::Void))
        {
            JLANG_ERROR("Expected 'void' in interface method");
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
    Advance();

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR("Expected struct name");
    }

    const std::string name = Previous().m_lexeme;

    std::string implementedInterface;

    if (IsMatched(TokenType::Arrow))
    {
        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected interface name after '->'");
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

        // Field type can be an identifier or a type keyword
        std::string typeName;
        if (IsMatched(TokenType::Identifier))
        {
            typeName = Previous().m_lexeme;
        }
        else if (IsMatched(TokenType::Int32))
        {
            typeName = "int32";
        }
        else
        {
            JLANG_ERROR("Expected field type");
            // Skip to next semicolon or brace
            while (!IsEndReached() && !Check(TokenType::Semicolon) && !Check(TokenType::RBrace))
            {
                Advance();
            }
            IsMatched(TokenType::Semicolon);
            continue;
        }

        bool isPointer = false;

        if (IsMatched(TokenType::Star))
        {
            isPointer = true;
        }

        if (!IsMatched(TokenType::Semicolon))
        {
            JLANG_ERROR("Expected ';' after struct field");
        }

        StructField field{fieldName, TypeRef{typeName, isPointer}};
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
    Advance();

    TokenType returnTokenType = Previous().m_type;

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR("Expected function name!");
    }

    const std::string &functionName = Previous().m_lexeme;

    // Hardcoded for no arguments, currently ..... will change that
    if (!IsMatched(TokenType::LParen) || !IsMatched(TokenType::RParen))
    {
        JLANG_ERROR("Expected () after function name");
    }

    TypeRef returnType;

    if (returnTokenType == TokenType::Void)
    {
        returnType = TypeRef{"void", false};
    }
    else
    {
        returnType = TypeRef{Previous().m_lexeme, false};
    }

    std::vector<Parameter> params;

    if (IsMatched(TokenType::Arrow))
    {
        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected parameter type identifier '->' ");
        }

        const std::string &paramType = Previous().m_lexeme;
        bool isPointer = IsMatched(TokenType::Star);

        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected parameter name!");
        }

        const std::string &paramName = Previous().m_lexeme;

        params.push_back(Parameter{paramName, TypeRef{paramType, isPointer}});
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

    if (Check(TokenType::Var))
    {
        return ParseVarDecl();
    }

    if (Check(TokenType::LBrace))
    {
        return ParseBlock();
    }

    return ParseExprStatement();
}

std::shared_ptr<AstNode> Parser::ParseVarDecl()
{
    Advance(); // consume 'var'

    if (!IsMatched(TokenType::Identifier))
    {
        JLANG_ERROR("Expected variable name");
        Advance(); // error recovery
        return nullptr;
    }

    std::string varName = Previous().m_lexeme;

    // Type can be an identifier or a type keyword like int32, void, etc.
    std::string typeName;
    if (IsMatched(TokenType::Identifier))
    {
        typeName = Previous().m_lexeme;
    }
    else if (IsMatched(TokenType::Int32))
    {
        typeName = "int32";
    }
    else if (IsMatched(TokenType::Void))
    {
        typeName = "void";
    }
    else
    {
        JLANG_ERROR("Expected variable type");
        Advance(); // error recovery
        return nullptr;
    }

    bool isPointer = IsMatched(TokenType::Star);

    std::shared_ptr<AstNode> initializer = nullptr;

    if (IsMatched(TokenType::Equal))
    {
        initializer = ParseExpression();
    }

    if (!IsMatched(TokenType::Semicolon))
    {
        JLANG_ERROR("Expected ';' after variable declaration");
    }

    auto varDecl = std::make_shared<VariableDecl>();
    varDecl->name = varName;
    varDecl->varType = TypeRef{typeName, isPointer};
    varDecl->initializer = initializer;

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

std::shared_ptr<AstNode> Parser::ParseExpression()
{
    return ParseEquality();
}

std::shared_ptr<AstNode> Parser::ParseEquality()
{
    auto left = ParsePrimary();

    while (Check(TokenType::EqualEqual) || Check(TokenType::NotEqual))
    {
        std::string op = Peek().m_lexeme;
        Advance();
        auto right = ParsePrimary();

        auto binary = std::make_shared<BinaryExpr>();
        binary->op = op;
        binary->left = left;
        binary->right = right;
        left = binary;
    }

    return left;
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

    // Handle NULL literal
    if (Check(TokenType::Identifier) && Peek().m_lexeme == "NULL")
    {
        Advance();
        auto literal = std::make_shared<LiteralExpr>();
        literal->value = "NULL";
        return literal;
    }

    // Handle sizeof(type)
    if (Check(TokenType::Identifier) && Peek().m_lexeme == "sizeof")
    {
        Advance();

        if (!IsMatched(TokenType::LParen))
        {
            JLANG_ERROR("Expected '(' after sizeof");
            return nullptr;
        }

        IsMatched(TokenType::Struct); // optional struct keyword

        if (!IsMatched(TokenType::Identifier))
        {
            JLANG_ERROR("Expected type name in sizeof");
            return nullptr;
        }

        if (!IsMatched(TokenType::RParen))
        {
            JLANG_ERROR("Expected ')' after sizeof type");
            return nullptr;
        }

        // For now, return a literal with the sizeof value (simplified)
        auto literal = std::make_shared<LiteralExpr>();
        literal->value = "8"; // Default size, would need proper type system
        return literal;
    }

    // Handle identifiers, function calls, and member access
    if (IsMatched(TokenType::Identifier))
    {
        std::string name = Previous().m_lexeme;

        // Handle member access: p.firstName
        while (IsMatched(TokenType::Dot))
        {
            if (!IsMatched(TokenType::Identifier))
            {
                JLANG_ERROR("Expected member name after '.'");
                break;
            }
            name += "." + Previous().m_lexeme;
        }

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
        else
        {
            auto var = std::make_shared<VarExpr>();
            var->name = name;
            return var;
        }
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

    JLANG_ERROR("Expected expression");
    return nullptr;
}

} // namespace jlang
