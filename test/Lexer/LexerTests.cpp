#include <gtest/gtest.h>

#include "../../src/Lexer/Lexer.h"

using namespace jlang;

TEST(LexerTest, TokenizesEmptySource)
{
    // Given
    Lexer lexer("");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesSimpleKeywords)
{
    // Given
    Lexer lexer("void");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_GE(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].m_type, TokenType::Void);
    EXPECT_EQ(tokens[tokens.size() - 1].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesBraces)
{
    // Given
    Lexer lexer("{}()");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].m_type, TokenType::LBrace);
    EXPECT_EQ(tokens[1].m_type, TokenType::RBrace);
    EXPECT_EQ(tokens[2].m_type, TokenType::LParen);
    EXPECT_EQ(tokens[3].m_type, TokenType::RParen);
}

TEST(LexerTest, TokenizesIdentifiers)
{
    // Given
    Lexer lexer("myVar");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_GE(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "myVar");
}

TEST(LexerTest, TokenizesNumbers)
{
    // Given
    Lexer lexer("42 123 0");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "42");
    EXPECT_EQ(tokens[1].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[1].m_lexeme, "123");
    EXPECT_EQ(tokens[2].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[2].m_lexeme, "0");
}

TEST(LexerTest, TokenizesStringLiterals)
{
    // Given
    Lexer lexer("\"hello world\"");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "hello world");
}

// Control flow keywords
TEST(LexerTest, TokenizesIfKeyword)
{
    // Given
    Lexer lexer("if");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::If);
    EXPECT_EQ(tokens[0].m_lexeme, "if");
}

TEST(LexerTest, TokenizesElseKeyword)
{
    // Given
    Lexer lexer("else");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Else);
    EXPECT_EQ(tokens[0].m_lexeme, "else");
}

TEST(LexerTest, TokenizesReturnKeyword)
{
    // Given
    Lexer lexer("return");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Return);
    EXPECT_EQ(tokens[0].m_lexeme, "return");
}

// Declaration keywords
TEST(LexerTest, TokenizesFnKeyword)
{
    // Given
    Lexer lexer("fn");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Fn);
    EXPECT_EQ(tokens[0].m_lexeme, "fn");
}

TEST(LexerTest, TokenizesVarKeyword)
{
    // Given
    Lexer lexer("var");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[0].m_lexeme, "var");
}

TEST(LexerTest, TokenizesStructKeyword)
{
    // Given
    Lexer lexer("struct");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Struct);
    EXPECT_EQ(tokens[0].m_lexeme, "struct");
}

TEST(LexerTest, TokenizesInterfaceKeyword)
{
    // Given
    Lexer lexer("interface");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Interface);
    EXPECT_EQ(tokens[0].m_lexeme, "interface");
}

// Memory and literal keywords
TEST(LexerTest, TokenizesAllocKeyword)
{
    // Given
    Lexer lexer("alloc");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Alloc);
    EXPECT_EQ(tokens[0].m_lexeme, "alloc");
}

TEST(LexerTest, TokenizesNullKeyword)
{
    // Given
    Lexer lexer("null");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Null);
    EXPECT_EQ(tokens[0].m_lexeme, "null");
}

// Type keywords - signed integers
TEST(LexerTest, TokenizesI8Keyword)
{
    // Given
    Lexer lexer("i8");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::I8);
    EXPECT_EQ(tokens[0].m_lexeme, "i8");
}

TEST(LexerTest, TokenizesI16Keyword)
{
    // Given
    Lexer lexer("i16");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::I16);
    EXPECT_EQ(tokens[0].m_lexeme, "i16");
}

TEST(LexerTest, TokenizesI32Keyword)
{
    // Given
    Lexer lexer("i32");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::I32);
    EXPECT_EQ(tokens[0].m_lexeme, "i32");
}

TEST(LexerTest, TokenizesI64Keyword)
{
    // Given
    Lexer lexer("i64");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::I64);
    EXPECT_EQ(tokens[0].m_lexeme, "i64");
}

// Type keywords - unsigned integers
TEST(LexerTest, TokenizesU8Keyword)
{
    // Given
    Lexer lexer("u8");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::U8);
    EXPECT_EQ(tokens[0].m_lexeme, "u8");
}

TEST(LexerTest, TokenizesU16Keyword)
{
    // Given
    Lexer lexer("u16");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::U16);
    EXPECT_EQ(tokens[0].m_lexeme, "u16");
}

TEST(LexerTest, TokenizesU32Keyword)
{
    // Given
    Lexer lexer("u32");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::U32);
    EXPECT_EQ(tokens[0].m_lexeme, "u32");
}

TEST(LexerTest, TokenizesU64Keyword)
{
    // Given
    Lexer lexer("u64");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::U64);
    EXPECT_EQ(tokens[0].m_lexeme, "u64");
}

// Type keywords - floating point
TEST(LexerTest, TokenizesF32Keyword)
{
    // Given
    Lexer lexer("f32");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::F32);
    EXPECT_EQ(tokens[0].m_lexeme, "f32");
}

TEST(LexerTest, TokenizesF64Keyword)
{
    // Given
    Lexer lexer("f64");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::F64);
    EXPECT_EQ(tokens[0].m_lexeme, "f64");
}

// Type keywords - bool and char
TEST(LexerTest, TokenizesBoolKeyword)
{
    // Given
    Lexer lexer("bool");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Bool);
    EXPECT_EQ(tokens[0].m_lexeme, "bool");
}

TEST(LexerTest, TokenizesCharKeyword)
{
    // Given
    Lexer lexer("char");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Char);
    EXPECT_EQ(tokens[0].m_lexeme, "char");
}

// Operators and symbols
TEST(LexerTest, TokenizesSemicolon)
{
    // Given
    Lexer lexer(";");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[0].m_lexeme, ";");
}

TEST(LexerTest, TokenizesColon)
{
    // Given
    Lexer lexer(":");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Colon);
    EXPECT_EQ(tokens[0].m_lexeme, ":");
}

TEST(LexerTest, TokenizesComma)
{
    // Given
    Lexer lexer(",");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Comma);
    EXPECT_EQ(tokens[0].m_lexeme, ",");
}

TEST(LexerTest, TokenizesDot)
{
    // Given
    Lexer lexer(".");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Dot);
    EXPECT_EQ(tokens[0].m_lexeme, ".");
}

TEST(LexerTest, TokenizesStar)
{
    // Given
    Lexer lexer("*");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Star);
    EXPECT_EQ(tokens[0].m_lexeme, "*");
}

TEST(LexerTest, TokenizesLess)
{
    // Given
    Lexer lexer("<");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Less);
    EXPECT_EQ(tokens[0].m_lexeme, "<");
}

TEST(LexerTest, TokenizesGreater)
{
    // Given
    Lexer lexer(">");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Greater);
    EXPECT_EQ(tokens[0].m_lexeme, ">");
}

TEST(LexerTest, TokenizesEqual)
{
    // Given
    Lexer lexer("=");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Equal);
    EXPECT_EQ(tokens[0].m_lexeme, "=");
}

TEST(LexerTest, TokenizesEqualEqual)
{
    // Given
    Lexer lexer("==");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::EqualEqual);
    EXPECT_EQ(tokens[0].m_lexeme, "==");
}

TEST(LexerTest, TokenizesNotEqual)
{
    // Given
    Lexer lexer("!=");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::NotEqual);
    EXPECT_EQ(tokens[0].m_lexeme, "!=");
}

TEST(LexerTest, TokenizesArrow)
{
    // Given
    Lexer lexer("->");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Arrow);
    EXPECT_EQ(tokens[0].m_lexeme, "->");
}

// Unknown tokens
TEST(LexerTest, TokenizesSingleExclamationAsUnknown)
{
    // Given
    Lexer lexer("!");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Unknown);
    EXPECT_EQ(tokens[0].m_lexeme, "!");
}

TEST(LexerTest, TokenizesMinus)
{
    // Given
    Lexer lexer("-");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Minus);
    EXPECT_EQ(tokens[0].m_lexeme, "-");
}

TEST(LexerTest, TokenizesUnknownCharacter)
{
    // Given
    Lexer lexer("@");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Unknown);
    EXPECT_EQ(tokens[0].m_lexeme, "@");
}

// Identifier edge cases
TEST(LexerTest, TokenizesIdentifierStartingWithUnderscore)
{
    // Given
    Lexer lexer("_privateVar");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "_privateVar");
}

TEST(LexerTest, TokenizesIdentifierWithNumbers)
{
    // Given
    Lexer lexer("var123");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "var123");
}

TEST(LexerTest, TokenizesIdentifierWithUnderscores)
{
    // Given
    Lexer lexer("my_var_name");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "my_var_name");
}

TEST(LexerTest, TokenizesSingleUnderscoreAsIdentifier)
{
    // Given
    Lexer lexer("_");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "_");
}

// String literal edge cases
TEST(LexerTest, TokenizesEmptyString)
{
    // Given
    Lexer lexer("\"\"");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "");
}

TEST(LexerTest, IgnoresUnterminatedString)
{
    // Given
    Lexer lexer("\"unterminated");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then - unterminated string produces no token, only EOF
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesMultilineString)
{
    // Given
    Lexer lexer("\"hello\nworld\"");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "hello\nworld");
}

// Line number tracking
TEST(LexerTest, TracksLineNumbersAcrossNewlines)
{
    // Given
    Lexer lexer("var\n\nfn");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[0].m_CurrentLine, 1);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
    EXPECT_EQ(tokens[1].m_CurrentLine, 3);
}

TEST(LexerTest, TracksLineNumberInMultilineString)
{
    // Given
    std::string source = "\"line1\nline2\"\nvar";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[1].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_CurrentLine, 3);
}

// Whitespace handling
TEST(LexerTest, HandlesTabsAsWhitespace)
{
    // Given
    Lexer lexer("var\tfn");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
}

TEST(LexerTest, HandlesCarriageReturnAsWhitespace)
{
    // Given
    Lexer lexer("var\rfn");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
}

TEST(LexerTest, HandlesMultipleSpaces)
{
    // Given
    Lexer lexer("var    fn");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
}

TEST(LexerTest, HandlesMixedWhitespace)
{
    // Given
    Lexer lexer("var \t\r\n fn");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
}

// Integration tests - complex scenarios
TEST(LexerTest, TokenizesFunctionSignature)
{
    // Given
    std::string source = "fn main() -> void";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].m_type, TokenType::Fn);
    EXPECT_EQ(tokens[1].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].m_lexeme, "main");
    EXPECT_EQ(tokens[2].m_type, TokenType::LParen);
    EXPECT_EQ(tokens[3].m_type, TokenType::RParen);
    EXPECT_EQ(tokens[4].m_type, TokenType::Arrow);
    EXPECT_EQ(tokens[5].m_type, TokenType::Void);
    EXPECT_EQ(tokens[6].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesVariableDeclaration)
{
    // Given
    std::string source = "var x: i32 = 42;";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 8);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].m_lexeme, "x");
    EXPECT_EQ(tokens[2].m_type, TokenType::Colon);
    EXPECT_EQ(tokens[3].m_type, TokenType::I32);
    EXPECT_EQ(tokens[4].m_type, TokenType::Equal);
    EXPECT_EQ(tokens[5].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[5].m_lexeme, "42");
    EXPECT_EQ(tokens[6].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[7].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesIfElseStatement)
{
    // Given
    std::string source = "if x == 0 { return null; } else { return x; }";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 16);
    EXPECT_EQ(tokens[0].m_type, TokenType::If);
    EXPECT_EQ(tokens[1].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].m_type, TokenType::EqualEqual);
    EXPECT_EQ(tokens[3].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[4].m_type, TokenType::LBrace);
    EXPECT_EQ(tokens[5].m_type, TokenType::Return);
    EXPECT_EQ(tokens[6].m_type, TokenType::Null);
    EXPECT_EQ(tokens[7].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[8].m_type, TokenType::RBrace);
    EXPECT_EQ(tokens[9].m_type, TokenType::Else);
    EXPECT_EQ(tokens[10].m_type, TokenType::LBrace);
    EXPECT_EQ(tokens[11].m_type, TokenType::Return);
    EXPECT_EQ(tokens[12].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[13].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[14].m_type, TokenType::RBrace);
    EXPECT_EQ(tokens[15].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesStructDefinition)
{
    // Given
    std::string source = "struct Point { x: f32, y: f32 }";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 12);
    EXPECT_EQ(tokens[0].m_type, TokenType::Struct);
    EXPECT_EQ(tokens[1].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].m_lexeme, "Point");
    EXPECT_EQ(tokens[2].m_type, TokenType::LBrace);
    EXPECT_EQ(tokens[3].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[3].m_lexeme, "x");
    EXPECT_EQ(tokens[4].m_type, TokenType::Colon);
    EXPECT_EQ(tokens[5].m_type, TokenType::F32);
    EXPECT_EQ(tokens[6].m_type, TokenType::Comma);
    EXPECT_EQ(tokens[7].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[8].m_type, TokenType::Colon);
    EXPECT_EQ(tokens[9].m_type, TokenType::F32);
    EXPECT_EQ(tokens[10].m_type, TokenType::RBrace);
    EXPECT_EQ(tokens[11].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesPointerType)
{
    // Given
    std::string source = "var ptr: *i32 = alloc;";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 9);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].m_type, TokenType::Colon);
    EXPECT_EQ(tokens[3].m_type, TokenType::Star);
    EXPECT_EQ(tokens[4].m_type, TokenType::I32);
    EXPECT_EQ(tokens[5].m_type, TokenType::Equal);
    EXPECT_EQ(tokens[6].m_type, TokenType::Alloc);
    EXPECT_EQ(tokens[7].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[8].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesMemberAccess)
{
    // Given
    Lexer lexer("point.x");

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "point");
    EXPECT_EQ(tokens[1].m_type, TokenType::Dot);
    EXPECT_EQ(tokens[2].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].m_lexeme, "x");
}

TEST(LexerTest, TokenizesComparisonOperators)
{
    // Given
    std::string source = "a < b > c != d == e";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 10);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].m_type, TokenType::Less);
    EXPECT_EQ(tokens[2].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[3].m_type, TokenType::Greater);
    EXPECT_EQ(tokens[4].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[5].m_type, TokenType::NotEqual);
    EXPECT_EQ(tokens[6].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[7].m_type, TokenType::EqualEqual);
    EXPECT_EQ(tokens[8].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[9].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesInterfaceDefinition)
{
    // Given
    std::string source = "interface Drawable { fn draw() -> void; }";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 12);
    EXPECT_EQ(tokens[0].m_type, TokenType::Interface);
    EXPECT_EQ(tokens[1].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].m_lexeme, "Drawable");
    EXPECT_EQ(tokens[2].m_type, TokenType::LBrace);
    EXPECT_EQ(tokens[3].m_type, TokenType::Fn);
    EXPECT_EQ(tokens[4].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[5].m_type, TokenType::LParen);
    EXPECT_EQ(tokens[6].m_type, TokenType::RParen);
    EXPECT_EQ(tokens[7].m_type, TokenType::Arrow);
    EXPECT_EQ(tokens[8].m_type, TokenType::Void);
    EXPECT_EQ(tokens[9].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[10].m_type, TokenType::RBrace);
    EXPECT_EQ(tokens[11].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesAllTypeKeywordsTogether)
{
    // Given
    std::string source = "i8 i16 i32 i64 u8 u16 u32 u64 f32 f64 bool char void";
    Lexer lexer(source);

    // When
    std::vector<Token> tokens = lexer.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 14);
    EXPECT_EQ(tokens[0].m_type, TokenType::I8);
    EXPECT_EQ(tokens[1].m_type, TokenType::I16);
    EXPECT_EQ(tokens[2].m_type, TokenType::I32);
    EXPECT_EQ(tokens[3].m_type, TokenType::I64);
    EXPECT_EQ(tokens[4].m_type, TokenType::U8);
    EXPECT_EQ(tokens[5].m_type, TokenType::U16);
    EXPECT_EQ(tokens[6].m_type, TokenType::U32);
    EXPECT_EQ(tokens[7].m_type, TokenType::U64);
    EXPECT_EQ(tokens[8].m_type, TokenType::F32);
    EXPECT_EQ(tokens[9].m_type, TokenType::F64);
    EXPECT_EQ(tokens[10].m_type, TokenType::Bool);
    EXPECT_EQ(tokens[11].m_type, TokenType::Char);
    EXPECT_EQ(tokens[12].m_type, TokenType::Void);
    EXPECT_EQ(tokens[13].m_type, TokenType::EndOfFile);
}
