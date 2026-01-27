#pragma once

namespace jlang
{
enum class TokenType
{
    // Keywords
    Interface,
    Struct,
    Var,
    Fn,
    If,
    Else,
    While,
    Return,
    Null,
    Alloc,
    True,
    False,

    // Type keywords
    Void,
    I8,
    I16,
    I32,
    I64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    Bool,
    Char,

    // Symbols
    LBrace,
    RBrace,
    LParen,
    RParen,
    Semicolon,
    Colon,
    Arrow,
    Assign,
    Star,
    Plus,
    Minus,
    Slash,
    Comma,
    Dot,
    NotEqual,
    EqualEqual,
    Less,
    Greater,
    Equal,
    ColonEqual, // := for type inference
    And,
    Or,
    Not,
    AndKeyword, // 'and' keyword (non-short-circuit)
    OrKeyword,  // 'or' keyword (non-short-circuit)
    PlusPlus,   // ++
    MinusMinus, // --

    // Literals and identifiers
    Identifier,
    StringLiteral,
    NumberLiteral,
    FloatLiteral,
    CharLiteral,

    EndOfFile,
    Unknown
};

} // namespace jlang
