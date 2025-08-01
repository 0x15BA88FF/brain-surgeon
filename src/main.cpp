#include "lexer.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::string readFile(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <brainfuck_file>" << std::endl;
        return 1;
    }

    try {
        std::string filename = argv[1];
        std::string source = readFile(filename);

        BrainfuckLexer lexer;
        std::vector<Token> tokens = lexer.tokenize(source);

        std::cout << "Reading file: " << filename << std::endl
                  << "File size: " << source.length() << " characters" << std::endl
                  << "Tokenized " << tokens.size() << " tokens" << std::endl
                  << std::endl;

        // Parse to AST
        BrainfuckParser parser;
        auto ast = parser.parse(tokens);

        parser.printAST(ast.get());
        parser.printErrors();
        parser.printWarnings();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
