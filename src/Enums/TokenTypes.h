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
    Return,
    Null,
    Alloc,

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
    Comma,
    Dot,
    NotEqual,
    EqualEqual,
    Less,
    Greater,
    Equal,

    // Literals and identifiers
    Identifier,
    StringLiteral,
    NumberLiteral,

    EndOfFile,
    Unknown
};

} // namespace jlang
