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
    LOOP_END    // ]
};

struct Token {
    TokenType type;
    size_t position; // Position in source
    size_t line;     // Line number
    size_t column;   // Column number
};

class BrainfuckLexer {
  private:
    std::string source;
    std::vector<Token> tokens;

  public:
    void tokenize(const std::string &input);
    const std::vector<Token> &getTokens() const;
    void printTokens() const;
};
