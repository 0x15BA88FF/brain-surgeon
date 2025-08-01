#include "lexer.hpp"

std::vector<Token> BrainfuckLexer::tokenize(const std::string &input) {
    std::vector<Token> tokens;
    size_t i = 0;
    size_t line = 1;
    size_t column = 1;

    while (i < input.length()) {
        TokenType type;
        std::string text;
        char c = input[i];
        bool isValid = true;
        size_t startPos = i;
        size_t startColumn = column;

        switch (c) {
        case '>':
            type = TokenType::MOVE_RIGHT;
            text = ">";
            break;
        case '<':
            type = TokenType::MOVE_LEFT;
            text = "<";
            break;
        case '+':
            type = TokenType::INCREMENT;
            text = "+";
            break;
        case '-':
            type = TokenType::DECREMENT;
            text = "-";
            break;
        case '.':
            type = TokenType::OUTPUT;
            text = ".";
            break;
        case ',':
            type = TokenType::INPUT;
            text = ",";
            break;
        case '[':
            type = TokenType::LOOP_START;
            text = "[";
            break;
        case ']':
            type = TokenType::LOOP_END;
            text = "]";
            break;
        case ' ':
            type = TokenType::WHITESPACE;
            text = " ";
            isValid = false;
            break;
        case '\n':
            line++;
            column = 1;
            i++;
            continue;
        default: {
            type = TokenType::COMMENT;
            isValid = false;

            size_t commentStart = i;
            size_t commentCol = column;

            while (i < input.length()) {
                char ch = input[i];

                if (ch == ' ' || ch == '\n' || ch == '>' || ch == '<' || ch == '+' || ch == '-' ||
                    ch == '.' || ch == ',' || ch == '[' || ch == ']') {
                    break;
                }

                i++;
                column++;
            }

            text = input.substr(commentStart, i - commentStart);
            tokens.push_back({type, isValid, commentStart, line, commentCol, text});

            continue;
        }
        }

        tokens.push_back({type, isValid, startPos, line, startColumn, text});
        i++;
        column++;
    }

    return tokens;
}
