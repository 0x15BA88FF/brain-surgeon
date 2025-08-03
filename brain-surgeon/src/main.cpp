#include "formatter.hpp"
#include "lexer.hpp"
#include "linter.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void write_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write to file: " + filename);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n"
                  << "  " << argv[0] << " lint <file.bf>    # Lint Brainfuck file\n"
                  << "  " << argv[0] << " fmt <file.bf>     # Format Brainfuck file (writes to file)\n"
                  << "  " << argv[0] << " debug <file.bf>   # Parse, print AST, lint\n";
        return 1;
    }

    std::string command = argv[1];
    std::string filepath = argv[2];

    try {
        BrainfuckLexer lexer;
        BrainfuckParser parser;
        FormatterConfig fmt_config;

        std::string source = read_file(filepath);
        std::vector<Token> tokens = lexer.tokenize(source);
        auto ast = parser.parse(tokens);

        if (command == "lint") {
            std::cout << lint_to_json(ast.get()) << std::endl;
        } else if (command == "fmt") {
            std::string formatted = format_tree(ast.get(), fmt_config);
            write_file(filepath, formatted);
            std::cout << "Formatted and wrote to " << filepath << std::endl;
        } else if (command == "debug") {
            std::cout << "AST =================" << std::endl << tree_to_string(ast.get()) << std::endl;
            std::cout << "Linting =============" << std::endl << lint_to_json(ast.get()) << std::endl;
            std::cout << "Formatting ==========" << std::endl << format_tree(ast.get(), fmt_config) << std::endl;
        } else {
            std::cerr << "Unknown command: " << command << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
