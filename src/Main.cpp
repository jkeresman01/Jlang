#include "CodeGen/CodeGen.h"
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace jlang;

// Forward declarations
void TryLexer();
void TryParser();
void TryCodeGen();

// std::filesystem::path is better here, but don't care, it's for testing, if you have C++17
// It would have taken me less time to replace it than to type this out
std::string Load(const std::string &path)
{
    std::ifstream in(path);

    if (!in.is_open())
    {
        std::cout << "No can do for: " << path << "\r\n";
    }

    std::stringstream buffer;
    buffer << in.rdbuf();

    in.close();

    return buffer.str();
}

void TryAllThis()
{
    TryLexer();
    TryParser();
    TryCodeGen();
}

void TryLexer()
{
    std::string sourceCode = Load("../samples/simple.j");

    Lexer lexer(sourceCode);
    const std::vector<Token> &tokens = lexer.Tokenize();

    std::cout << "Tokens: \r\n";

    for (const auto &token : tokens)
    {
        std::cout << token.ToString() << "\r\n ";
    }
}

void TryParser()
{
    std::string sourceCode = Load("../samples/simple.j");

    Lexer lexer(sourceCode);
    const std::vector<Token> &tokens = lexer.Tokenize();

    Parser parser(tokens);
    std::vector<std::shared_ptr<AstNode>> program = parser.Parse();

    std::cout << "\r\nParsed " << program.size() << " top-level declarations\r\n";
}

void TryCodeGen()
{
    std::string sourceCode = Load("../samples/simple.j");

    Lexer lexer(sourceCode);
    const std::vector<Token> &tokens = lexer.Tokenize();

    Parser parser(tokens);
    std::vector<std::shared_ptr<AstNode>> program = parser.Parse();

    std::cout << "\r\n--- LLVM IR ---\r\n";

    CodeGenerator codegen;
    codegen.Generate(program);
    codegen.DumpIR();
}

int main()
{
    TryAllThis();
}
