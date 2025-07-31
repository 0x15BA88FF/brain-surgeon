#include "lexer.hpp"
#include <iostream>

void BrainfuckLexer::tokenize(const std::string &input) {
    source = input;
    tokens.clear();

    size_t line = 1;
    size_t column = 1;

    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];

        // Only parse valid brainfuck commands
        TokenType type;
        bool validToken = true;

        switch (c) {
        case '>':
            type = TokenType::MOVE_RIGHT;
            break;
        case '<':
            type = TokenType::MOVE_LEFT;
            break;
        case '+':
            type = TokenType::INCREMENT;
            break;
        case '-':
            type = TokenType::DECREMENT;
            break;
        case '.':
            type = TokenType::OUTPUT;
            break;
        case ',':
            type = TokenType::INPUT;
            break;
        case '[':
            type = TokenType::LOOP_START;
            break;
        case ']':
            type = TokenType::LOOP_END;
            break;
        default:
            validToken = false;
            break;
        }

        if (validToken) {
            tokens.push_back({type, i, line, column});
        }

        // Track line/column for error reporting
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
}

const std::vector<Token> &BrainfuckLexer::getTokens() const {
    return tokens;
}

void BrainfuckLexer::printTokens() const {
    const char *typeNames[] = {"MOVE_RIGHT", "MOVE_LEFT", "INCREMENT",  "DECREMENT",
                               "OUTPUT",     "INPUT",     "LOOP_START", "LOOP_END"};

    for (const auto &token : tokens) {
        std::cout << typeNames[static_cast<int>(token.type)] << " at " << token.line << ":"
                  << token.column << std::endl;
    }
}
