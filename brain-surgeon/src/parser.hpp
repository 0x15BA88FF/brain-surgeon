#pragma once

#include "lexer.hpp"
#include <memory>
#include <string>
#include <vector>

enum class NodeType { PROGRAM, COMMAND, LOOP, WHITESPACE, COMMENT, UNMATCHED_CLOSE };

class ASTNode {
public:
    NodeType type;
    size_t start_line;
    size_t start_column;
    size_t end_line;
    size_t end_column;

    virtual ~ASTNode() = default;
    explicit ASTNode(NodeType t, size_t sl = 0, size_t sc = 0, size_t el = 0, size_t ec = 0): type(t), start_line(sl), start_column(sc), end_line(el), end_column(ec) {}
};

class ProgramNode: public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    ProgramNode(): ASTNode(NodeType::PROGRAM) {}

    void update_end_position() {
        if (!statements.empty()) {
            auto& last_statement = statements.back();
            end_line = last_statement->end_line;
            end_column = last_statement->end_column;
        }
    }
};

class CommandNode: public ASTNode {
public:
    TokenType command;

    explicit CommandNode(TokenType cmd, size_t sl = 0, size_t sc = 0, size_t el = 0, size_t ec = 0): ASTNode(NodeType::COMMAND, sl, sc, el, ec), command(cmd) {}
};

class LoopNode: public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> body;
    bool is_empty;             // Contains no valid commands
    bool is_terminated;        // Has matching ']'
    bool has_single_statement; // Contains exactly one valid command

    explicit LoopNode(size_t sl = 0, size_t sc = 0, size_t el = 0, size_t ec = 0):
        ASTNode(NodeType::LOOP, sl, sc, el, ec),

        is_empty(true),
        is_terminated(false),
        has_single_statement(false) {}

    void update_end_position(size_t el, size_t ec) {
        end_line = el;
        end_column = ec;
        is_terminated = true;
    }

    void analyze_content() {
        size_t command_count = 0;

        for (const auto& stmt: body) {
            if (stmt->type == NodeType::COMMAND) {
                auto* cmd = static_cast<CommandNode*>(stmt.get());

                if (cmd->command != TokenType::LOOP_START && cmd->command != TokenType::LOOP_END) {
                    command_count++;
                }
            }
        }

        is_empty = (command_count == 0);
        has_single_statement = (command_count == 1);
    }
};

class WhitespaceNode: public ASTNode {
public:
    std::string text;

    explicit WhitespaceNode(const std::string& t, size_t sl = 0, size_t sc = 0, size_t el = 0, size_t ec = 0): ASTNode(NodeType::WHITESPACE, sl, sc, el, ec), text(t) {}
};

class CommentNode: public ASTNode {
public:
    std::string text;

    explicit CommentNode(const std::string& t, size_t sl = 0, size_t sc = 0, size_t el = 0, size_t ec = 0): ASTNode(NodeType::COMMENT, sl, sc, el, ec), text(t) {}
};

class UnmatchedCloseNode: public ASTNode {
public:
    explicit UnmatchedCloseNode(size_t sl = 0, size_t sc = 0, size_t el = 0, size_t ec = 0): ASTNode(NodeType::UNMATCHED_CLOSE, sl, sc, el, ec) {}
};

class BrainfuckParser {
private:
    std::vector<Token> tokens;
    size_t current;

    std::unique_ptr<LoopNode> parse_loop();
    std::unique_ptr<ASTNode> parse_statement();
    std::unique_ptr<CommentNode> parse_comment_sequence();
    std::unique_ptr<WhitespaceNode> parse_whitespace_sequence();

public:
    std::unique_ptr<ProgramNode> parse(const std::vector<Token>& token_list);

private:
    std::string get_token_name(TokenType type) const;

    bool has_unterminated_loops(const ASTNode* node) const;
    std::vector<const LoopNode*> find_unterminated_loops(const ASTNode* node) const;
};

std::string tree_to_string(const ASTNode* node, int indent = 0);
