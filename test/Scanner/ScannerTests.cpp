#include <gtest/gtest.h>

#include "../../src/Scanner/Scanner.h"

using namespace jlang;

TEST(ScannerTest, TokenizesEmptySource)
{
    // Given
    Scanner scanner("");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].m_type, TokenType::EndOfFile);
}

TEST(ScannerTest, TokenizesSimpleKeywords)
{
    // Given
    Scanner scanner("void");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_GE(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].m_type, TokenType::Void);
    EXPECT_EQ(tokens[tokens.size() - 1].m_type, TokenType::EndOfFile);
}

TEST(ScannerTest, TokenizesBraces)
{
    // Given
    Scanner scanner("{}()");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].m_type, TokenType::LBrace);
    EXPECT_EQ(tokens[1].m_type, TokenType::RBrace);
    EXPECT_EQ(tokens[2].m_type, TokenType::LParen);
    EXPECT_EQ(tokens[3].m_type, TokenType::RParen);
}

TEST(ScannerTest, TokenizesIdentifiers)
{
    // Given
    Scanner scanner("myVar");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_GE(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "myVar");
}

TEST(ScannerTest, TokenizesNumbers)
{
    // Given
    Scanner scanner("42 123 0");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "42");
    EXPECT_EQ(tokens[1].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[1].m_lexeme, "123");
    EXPECT_EQ(tokens[2].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[2].m_lexeme, "0");
}

TEST(ScannerTest, TokenizesStringLiterals)
{
    // Given
    Scanner scanner("\"hello world\"");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "hello world");
}

// Control flow keywords
TEST(ScannerTest, TokenizesIfKeyword)
{
    // Given
    Scanner scanner("if");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::If);
    EXPECT_EQ(tokens[0].m_lexeme, "if");
}

TEST(ScannerTest, TokenizesElseKeyword)
{
    // Given
    Scanner scanner("else");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Else);
    EXPECT_EQ(tokens[0].m_lexeme, "else");
}

TEST(ScannerTest, TokenizesWhileKeyword)
{
    // Given
    Scanner scanner("while");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::While);
    EXPECT_EQ(tokens[0].m_lexeme, "while");
}

TEST(ScannerTest, TokenizesReturnKeyword)
{
    // Given
    Scanner scanner("return");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Return);
    EXPECT_EQ(tokens[0].m_lexeme, "return");
}

// Declaration keywords
TEST(ScannerTest, TokenizesFnKeyword)
{
    // Given
    Scanner scanner("fn");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Fn);
    EXPECT_EQ(tokens[0].m_lexeme, "fn");
}

TEST(ScannerTest, TokenizesVarKeyword)
{
    // Given
    Scanner scanner("var");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[0].m_lexeme, "var");
}

TEST(ScannerTest, TokenizesStructKeyword)
{
    // Given
    Scanner scanner("struct");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Struct);
    EXPECT_EQ(tokens[0].m_lexeme, "struct");
}

TEST(ScannerTest, TokenizesInterfaceKeyword)
{
    // Given
    Scanner scanner("interface");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Interface);
    EXPECT_EQ(tokens[0].m_lexeme, "interface");
}

// Memory and literal keywords
TEST(ScannerTest, TokenizesAllocKeyword)
{
    // Given
    Scanner scanner("alloc");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Alloc);
    EXPECT_EQ(tokens[0].m_lexeme, "alloc");
}

TEST(ScannerTest, TokenizesNullKeyword)
{
    // Given
    Scanner scanner("null");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Null);
    EXPECT_EQ(tokens[0].m_lexeme, "null");
}

// Boolean literals
TEST(ScannerTest, TokenizesTrueKeyword)
{
    // Given
    Scanner scanner("true");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::True);
    EXPECT_EQ(tokens[0].m_lexeme, "true");
}

TEST(ScannerTest, TokenizesFalseKeyword)
{
    // Given
    Scanner scanner("false");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::False);
    EXPECT_EQ(tokens[0].m_lexeme, "false");
}

// Type keywords - signed integers
TEST(ScannerTest, TokenizesI8Keyword)
{
    // Given
    Scanner scanner("i8");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::I8);
    EXPECT_EQ(tokens[0].m_lexeme, "i8");
}

TEST(ScannerTest, TokenizesI16Keyword)
{
    // Given
    Scanner scanner("i16");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::I16);
    EXPECT_EQ(tokens[0].m_lexeme, "i16");
}

TEST(ScannerTest, TokenizesI32Keyword)
{
    // Given
    Scanner scanner("i32");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::I32);
    EXPECT_EQ(tokens[0].m_lexeme, "i32");
}

TEST(ScannerTest, TokenizesI64Keyword)
{
    // Given
    Scanner scanner("i64");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::I64);
    EXPECT_EQ(tokens[0].m_lexeme, "i64");
}

// Type keywords - unsigned integers
TEST(ScannerTest, TokenizesU8Keyword)
{
    // Given
    Scanner scanner("u8");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::U8);
    EXPECT_EQ(tokens[0].m_lexeme, "u8");
}

TEST(ScannerTest, TokenizesU16Keyword)
{
    // Given
    Scanner scanner("u16");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::U16);
    EXPECT_EQ(tokens[0].m_lexeme, "u16");
}

TEST(ScannerTest, TokenizesU32Keyword)
{
    // Given
    Scanner scanner("u32");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::U32);
    EXPECT_EQ(tokens[0].m_lexeme, "u32");
}

TEST(ScannerTest, TokenizesU64Keyword)
{
    // Given
    Scanner scanner("u64");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::U64);
    EXPECT_EQ(tokens[0].m_lexeme, "u64");
}

// Type keywords - floating point
TEST(ScannerTest, TokenizesF32Keyword)
{
    // Given
    Scanner scanner("f32");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::F32);
    EXPECT_EQ(tokens[0].m_lexeme, "f32");
}

TEST(ScannerTest, TokenizesF64Keyword)
{
    // Given
    Scanner scanner("f64");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::F64);
    EXPECT_EQ(tokens[0].m_lexeme, "f64");
}

// Type keywords - bool and char
TEST(ScannerTest, TokenizesBoolKeyword)
{
    // Given
    Scanner scanner("bool");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Bool);
    EXPECT_EQ(tokens[0].m_lexeme, "bool");
}

TEST(ScannerTest, TokenizesCharKeyword)
{
    // Given
    Scanner scanner("char");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Char);
    EXPECT_EQ(tokens[0].m_lexeme, "char");
}

// Operators and symbols
TEST(ScannerTest, TokenizesSemicolon)
{
    // Given
    Scanner scanner(";");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[0].m_lexeme, ";");
}

TEST(ScannerTest, TokenizesColon)
{
    // Given
    Scanner scanner(":");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Colon);
    EXPECT_EQ(tokens[0].m_lexeme, ":");
}

TEST(ScannerTest, TokenizesComma)
{
    // Given
    Scanner scanner(",");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Comma);
    EXPECT_EQ(tokens[0].m_lexeme, ",");
}

TEST(ScannerTest, TokenizesDot)
{
    // Given
    Scanner scanner(".");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Dot);
    EXPECT_EQ(tokens[0].m_lexeme, ".");
}

TEST(ScannerTest, TokenizesStar)
{
    // Given
    Scanner scanner("*");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Star);
    EXPECT_EQ(tokens[0].m_lexeme, "*");
}

TEST(ScannerTest, TokenizesLess)
{
    // Given
    Scanner scanner("<");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Less);
    EXPECT_EQ(tokens[0].m_lexeme, "<");
}

TEST(ScannerTest, TokenizesGreater)
{
    // Given
    Scanner scanner(">");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Greater);
    EXPECT_EQ(tokens[0].m_lexeme, ">");
}

TEST(ScannerTest, TokenizesEqual)
{
    // Given
    Scanner scanner("=");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Equal);
    EXPECT_EQ(tokens[0].m_lexeme, "=");
}

TEST(ScannerTest, TokenizesEqualEqual)
{
    // Given
    Scanner scanner("==");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::EqualEqual);
    EXPECT_EQ(tokens[0].m_lexeme, "==");
}

TEST(ScannerTest, TokenizesNotEqual)
{
    // Given
    Scanner scanner("!=");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::NotEqual);
    EXPECT_EQ(tokens[0].m_lexeme, "!=");
}

TEST(ScannerTest, TokenizesArrow)
{
    // Given
    Scanner scanner("->");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Arrow);
    EXPECT_EQ(tokens[0].m_lexeme, "->");
}

// Logical operators
TEST(ScannerTest, TokenizesNotOperator)
{
    // Given
    Scanner scanner("!");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Not);
    EXPECT_EQ(tokens[0].m_lexeme, "!");
}

TEST(ScannerTest, TokenizesAndOperator)
{
    // Given
    Scanner scanner("&&");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::And);
    EXPECT_EQ(tokens[0].m_lexeme, "&&");
}

TEST(ScannerTest, TokenizesOrOperator)
{
    // Given
    Scanner scanner("||");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Or);
    EXPECT_EQ(tokens[0].m_lexeme, "||");
}

TEST(ScannerTest, TokenizesMinus)
{
    // Given
    Scanner scanner("-");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Minus);
    EXPECT_EQ(tokens[0].m_lexeme, "-");
}

TEST(ScannerTest, TokenizesUnknownCharacter)
{
    // Given
    Scanner scanner("@");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Unknown);
    EXPECT_EQ(tokens[0].m_lexeme, "@");
}

// Identifier edge cases
TEST(ScannerTest, TokenizesIdentifierStartingWithUnderscore)
{
    // Given
    Scanner scanner("_privateVar");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "_privateVar");
}

TEST(ScannerTest, TokenizesIdentifierWithNumbers)
{
    // Given
    Scanner scanner("var123");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "var123");
}

TEST(ScannerTest, TokenizesIdentifierWithUnderscores)
{
    // Given
    Scanner scanner("my_var_name");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "my_var_name");
}

TEST(ScannerTest, TokenizesSingleUnderscoreAsIdentifier)
{
    // Given
    Scanner scanner("_");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "_");
}

// String literal edge cases
TEST(ScannerTest, TokenizesEmptyString)
{
    // Given
    Scanner scanner("\"\"");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "");
}

TEST(ScannerTest, IgnoresUnterminatedString)
{
    // Given
    Scanner scanner("\"unterminated");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then - unterminated string produces no token, only EOF
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].m_type, TokenType::EndOfFile);
}

TEST(ScannerTest, TokenizesMultilineString)
{
    // Given
    Scanner scanner("\"hello\nworld\"");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "hello\nworld");
}

// Line number tracking
TEST(ScannerTest, TracksLineNumbersAcrossNewlines)
{
    // Given
    Scanner scanner("var\n\nfn");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[0].m_CurrentLine, 1);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
    EXPECT_EQ(tokens[1].m_CurrentLine, 3);
}

TEST(ScannerTest, TracksLineNumberInMultilineString)
{
    // Given
    std::string source = "\"line1\nline2\"\nvar";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[1].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_CurrentLine, 3);
}

// Whitespace handling
TEST(ScannerTest, HandlesTabsAsWhitespace)
{
    // Given
    Scanner scanner("var\tfn");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
}

TEST(ScannerTest, HandlesCarriageReturnAsWhitespace)
{
    // Given
    Scanner scanner("var\rfn");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
}

TEST(ScannerTest, HandlesMultipleSpaces)
{
    // Given
    Scanner scanner("var    fn");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
}

TEST(ScannerTest, HandlesMixedWhitespace)
{
    // Given
    Scanner scanner("var \t\r\n fn");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Fn);
}

// Integration tests - complex scenarios
TEST(ScannerTest, TokenizesFunctionSignature)
{
    // Given
    std::string source = "fn main() -> void";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

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

TEST(ScannerTest, TokenizesVariableDeclaration)
{
    // Given
    std::string source = "var x: i32 = 42;";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

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

TEST(ScannerTest, TokenizesIfElseStatement)
{
    // Given
    std::string source = "if x == 0 { return null; } else { return x; }";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

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

TEST(ScannerTest, TokenizesStructDefinition)
{
    // Given
    std::string source = "struct Point { x: f32, y: f32 }";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

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

TEST(ScannerTest, TokenizesPointerType)
{
    // Given
    std::string source = "var ptr: *i32 = alloc;";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

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

TEST(ScannerTest, TokenizesMemberAccess)
{
    // Given
    Scanner scanner("point.x");

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "point");
    EXPECT_EQ(tokens[1].m_type, TokenType::Dot);
    EXPECT_EQ(tokens[2].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].m_lexeme, "x");
}

TEST(ScannerTest, TokenizesComparisonOperators)
{
    // Given
    std::string source = "a < b > c != d == e";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

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

TEST(ScannerTest, TokenizesInterfaceDefinition)
{
    // Given
    std::string source = "interface Drawable { fn draw() -> void; }";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

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

TEST(ScannerTest, TokenizesAllTypeKeywordsTogether)
{
    // Given
    std::string source = "i8 i16 i32 i64 u8 u16 u32 u64 f32 f64 bool char void";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

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

TEST(ScannerTest, TokenizesBooleanExpression)
{
    // Given
    std::string source = "true && false || !x";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].m_type, TokenType::True);
    EXPECT_EQ(tokens[1].m_type, TokenType::And);
    EXPECT_EQ(tokens[2].m_type, TokenType::False);
    EXPECT_EQ(tokens[3].m_type, TokenType::Or);
    EXPECT_EQ(tokens[4].m_type, TokenType::Not);
    EXPECT_EQ(tokens[5].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[6].m_type, TokenType::EndOfFile);
}

TEST(ScannerTest, TokenizesWhileLoop)
{
    // Given
    std::string source = "while x < 10 { x = x + 1; }";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 13);
    EXPECT_EQ(tokens[0].m_type, TokenType::While);
    EXPECT_EQ(tokens[1].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].m_type, TokenType::Less);
    EXPECT_EQ(tokens[3].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[4].m_type, TokenType::LBrace);
    EXPECT_EQ(tokens[5].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[6].m_type, TokenType::Equal);
    EXPECT_EQ(tokens[7].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[8].m_type, TokenType::Plus);
    EXPECT_EQ(tokens[9].m_type, TokenType::NumberLiteral);
    EXPECT_EQ(tokens[10].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[11].m_type, TokenType::RBrace);
    EXPECT_EQ(tokens[12].m_type, TokenType::EndOfFile);
}

TEST(ScannerTest, TokenizesBooleanVariableDeclaration)
{
    // Given
    std::string source = "var isActive: bool = true;";
    Scanner scanner(source);

    // When
    std::vector<Token> tokens = scanner.Tokenize();

    // Then
    ASSERT_EQ(tokens.size(), 8);
    EXPECT_EQ(tokens[0].m_type, TokenType::Var);
    EXPECT_EQ(tokens[1].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].m_lexeme, "isActive");
    EXPECT_EQ(tokens[2].m_type, TokenType::Colon);
    EXPECT_EQ(tokens[3].m_type, TokenType::Bool);
    EXPECT_EQ(tokens[4].m_type, TokenType::Equal);
    EXPECT_EQ(tokens[5].m_type, TokenType::True);
    EXPECT_EQ(tokens[6].m_type, TokenType::Semicolon);
    EXPECT_EQ(tokens[7].m_type, TokenType::EndOfFile);
}
