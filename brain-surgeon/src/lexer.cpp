#include "lexer.hpp"
#include <string.h>

std::vector<Token> BrainfuckLexer::tokenize(const std::string& input) {
    size_t index = 0;
    size_t start_line = 1;
    size_t start_column = 1;
    std::vector<Token> tokens;

    while (index < input.length()) {
        TokenType type;
        std::string text;
        bool is_valid = true;
        size_t end_line = start_line;
        size_t end_column = start_line;

        switch (input[index]) {
            case ',': type = TokenType::INPUT; break;
            case '.': type = TokenType::OUTPUT; break;
            case '+': type = TokenType::INCREMENT; break;
            case '-': type = TokenType::DECREMENT; break;
            case '<': type = TokenType::MOVE_LEFT; break;
            case '>': type = TokenType::MOVE_RIGHT; break;
            case '[': type = TokenType::LOOP_START; break;
            case ']': type = TokenType::LOOP_END; break;
            case ' ':
            case '\t':
                is_valid = false;
                type = TokenType::WHITESPACE;

                break;
            case '\n':
                is_valid = false;
                type = TokenType::NEWLINE;

                tokens.push_back({ type, is_valid, start_line, start_column, end_line, end_column, text });

                index++;
                start_line++;
                start_column = 1;

                continue;
            default: {
                is_valid = false;
                type = TokenType::COMMENT;

                size_t comment_start_index = index;
                size_t comment_start_column = start_column;

                while (index < input.length()) {
                    if (strchr(" \t\n\r><+-.,[]", input[index]) != nullptr) {
                        break;
                    }

                    index++;
                    start_column++;
                }

                text = input.substr(comment_start_index, index - comment_start_index);
                tokens.push_back({ type, is_valid, start_line, comment_start_column, start_line, start_column - 1, text });

                continue;
            }
        }

        tokens.push_back({ type, is_valid, start_line, start_column, end_line, start_column, text });

        index++;
        start_column++;
    }

    return tokens;
}
