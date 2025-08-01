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
    COMMENT     // everything else
};

struct Token {
    TokenType type;
    bool isValid;     // if the token is a valid BF command
    size_t position;  // Position in source
    size_t line;      // Line number
    size_t column;    // Column number
    std::string text; // The actual text content
};

class BrainfuckLexer {
  public:
    std::vector<Token> tokenize(const std::string &input);
};
