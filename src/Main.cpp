#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace jlang;

std::string load(const std::string &path)
{
    std::ifstream in(path);

    if (!in.is_open())
    {
        throw std::runtime_error("No can do for: " + path);
    }

    std::stringstream buffer;
    buffer << in.rdbuf();

    in.close();

    return buffer.str();
}

int main()
{
    try
    {
        std::string sourceCode = load("../samples/sample.j");

        Lexer lexer(sourceCode);
        const std::vector<Token> &tokens = lexer.Tokenize();

        std::cout << "=== Tokens ===" << std::endl;
        for (const auto &token : tokens)
        {
            std::cout << token.m_CurrentLine << ": " << token.m_lexeme << " ("
                      << static_cast<int32_t>(token.m_type) << ")\n";
        }

        Parser parser(tokens);
        auto ast = parser.Parse();

        std::cout << "Parsed " << ast.size() << " top-level declarations." << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
