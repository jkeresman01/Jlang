#include "CodeGen/CodeGen.h"
#include "Parser/Parser.h"
#include "Scanner/Scanner.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace jlang;

std::string Load(const std::string &path)
{
    std::ifstream in(path);

    if (!in.is_open())
    {
        std::cerr << "Error: Cannot open file: " << path << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << in.rdbuf();

    in.close();

    return buffer.str();
}

void Compile(const std::string &filePath)
{
    std::string sourceCode = Load(filePath);
    if (sourceCode.empty())
    {
        return;
    }

    Scanner scanner(sourceCode);
    const std::vector<Token> &tokens = scanner.Tokenize();

    Parser parser(tokens);
    std::vector<std::shared_ptr<AstNode>> program = parser.Parse();

    CodeGenerator codegen;
    codegen.Generate(program);
    codegen.DumpIR();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: Jlang <source_file.j>\n";
        return 1;
    }

    Compile(argv[1]);
    return 0;
}
