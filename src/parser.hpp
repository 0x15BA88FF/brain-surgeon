#pragma once

#include "lexer.hpp"
#include <memory>
#include <vector>

// AST Node types
enum class NodeType { PROGRAM, COMMAND, LOOP, WHITESPACE, COMMENT };

// Base AST Node
class ASTNode {
  public:
    NodeType type;
    size_t line;
    size_t column;

    ASTNode(NodeType t, size_t l = 0, size_t c = 0) : type(t), line(l), column(c) {}
    virtual ~ASTNode() = default;
};

// Program node (root of the AST)
class ProgramNode : public ASTNode {
  public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    ProgramNode() : ASTNode(NodeType::PROGRAM) {}
};

// Command node (single brainfuck instruction)
class CommandNode : public ASTNode {
  public:
    TokenType command;

    CommandNode(TokenType cmd, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::COMMAND, l, c), command(cmd) {}
};

// Loop node (contains a body of statements)
class LoopNode : public ASTNode {
  public:
    std::vector<std::unique_ptr<ASTNode>> body;

    LoopNode(size_t l = 0, size_t c = 0) : ASTNode(NodeType::LOOP, l, c) {}
};

class WhitespaceNode : public ASTNode {
  public:
    std::string text;

    WhitespaceNode(const std::string &t, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::WHITESPACE, l, c), text(t) {}
};

// Comment node
class CommentNode : public ASTNode {
  public:
    std::string text;

    CommentNode(const std::string &t, size_t l = 0, size_t c = 0)
        : ASTNode(NodeType::COMMENT, l, c), text(t) {}
};

struct ParseWarning {
    std::string message;
    size_t line;
    size_t column;
};

struct ParseError {
    std::string message;
    size_t line;
    size_t column;
};

// Parser class
class BrainfuckParser {
  private:
    std::vector<ParseError> errors;
    std::vector<ParseWarning> warnings;

    std::vector<Token> tokens;
    size_t current;

    std::unique_ptr<ASTNode> parseStatement(Token *&prevValidToken, size_t &prevValidIndex);
    std::unique_ptr<LoopNode> parseLoop();

  public:
    std::unique_ptr<ProgramNode> parse(const std::vector<Token> &tokenList);

    void printAST(const ASTNode *node, int indent = 0) const;
    void printErrors() const;
    void printWarnings() const;

    const std::vector<ParseError> &getErrors() const {
        return errors;
    }
    const std::vector<ParseWarning> &getWarnings() const {
        return warnings;
    }
};
