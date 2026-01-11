#include <gtest/gtest.h>

#include "../../src/Lexer/Lexer.h"

using namespace jlang;

TEST(LexerTest, TokenizesEmptySource)
{
    Lexer lexer("");
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesSimpleKeywords)
{
    Lexer lexer("void");
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_GE(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].m_type, TokenType::Void);
    EXPECT_EQ(tokens[tokens.size() - 1].m_type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizesBraces)
{
    Lexer lexer("{}()");
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].m_type, TokenType::LBrace);
    EXPECT_EQ(tokens[1].m_type, TokenType::RBrace);
    EXPECT_EQ(tokens[2].m_type, TokenType::LParen);
    EXPECT_EQ(tokens[3].m_type, TokenType::RParen);
}

TEST(LexerTest, TokenizesIdentifiers)
{
    Lexer lexer("myVar");
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_GE(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].m_type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].m_lexeme, "myVar");
}

TEST(LexerTest, TokenizesNumbers)
{
    Lexer lexer("42 123 0");
    std::vector<Token> tokens = lexer.Tokenize();

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
    Lexer lexer("\"hello world\"");
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].m_type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[0].m_lexeme, "hello world");
}
