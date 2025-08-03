#pragma once

#include <string>
#include <vector>

enum class TokenType {
    MOVE_RIGHT, // >
    MOVE_LEFT,  // <
    INCREMENT,  // +
    DECREMENT,  // -
    OUTPUT,     // .
    INPUT,      // ,
    LOOP_START, // [
    LOOP_END,   // ]
    WHITESPACE, // spaces
    COMMENT,    // everything else
    NEWLINE     // newline
};

struct Token {
    TokenType type;
    bool is_valid;       // if the token is a valid command
    size_t start_line;   // Beginning line number
    size_t start_column; // Beginning column number
    size_t end_line;     // End line number
    size_t end_column;   // End column number
    std::string text;    // The actual text content
};

class BrainfuckLexer {
public:
    std::vector<Token> tokenize(const std::string& input);
};
