#include "parser.hpp"
#include <iostream>
#include <sstream>

std::unique_ptr<ProgramNode> BrainfuckParser::parse(const std::vector<Token>& token_list) {
    current = 0;
    tokens = token_list;

    auto program = std::make_unique<ProgramNode>();

    while (current < tokens.size()) {
        auto stmt = parse_statement();

        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
    }

    program->update_end_position();
    return program;
}

std::unique_ptr<ASTNode> BrainfuckParser::parse_statement() {
    if (current >= tokens.size()) {
        return nullptr;
    }

    const Token& token = tokens[current];

    switch (token.type) {
        case TokenType::WHITESPACE:
        case TokenType::NEWLINE: return parse_whitespace_sequence();
        case TokenType::COMMENT: return parse_comment_sequence();
        case TokenType::LOOP_START: return parse_loop();
        case TokenType::LOOP_END: {
            auto unmatched = std::make_unique<UnmatchedCloseNode>(token.start_line, token.start_column, token.end_line, token.end_column);
            current++;

            return unmatched;
        }

        default:
            auto cmd = std::make_unique<CommandNode>(token.type, token.start_line, token.start_column, token.end_line, token.end_column);
            current++;

            return cmd;
    }
}

std::unique_ptr<LoopNode> BrainfuckParser::parse_loop() {
    if (current >= tokens.size() || tokens[current].type != TokenType::LOOP_START) {
        return nullptr;
    }

    const Token& start_token = tokens[current];
    current++;

    auto loop = std::make_unique<LoopNode>(start_token.start_line, start_token.start_column, start_token.end_line, start_token.end_column);

    while (current < tokens.size() && tokens[current].type != TokenType::LOOP_END) {
        auto stmt = parse_statement();

        if (stmt) {
            loop->body.push_back(std::move(stmt));
        }
    }

    if (current < tokens.size() && tokens[current].type == TokenType::LOOP_END) {
        const Token& end_token = tokens[current];
        loop->update_end_position(end_token.end_line, end_token.end_column);
        current++;
    }

    loop->analyze_content();

    return loop;
}

std::unique_ptr<WhitespaceNode> BrainfuckParser::parse_whitespace_sequence() {
    if (current >= tokens.size()) {
        return nullptr;
    }

    const Token& first_token = tokens[current];
    std::string combined_text;

    while (current < tokens.size() && (tokens[current].type == TokenType::WHITESPACE || tokens[current].type == TokenType::NEWLINE)) {
        combined_text += tokens[current].text;
        current++;
    }

    const Token& last_token = tokens[current - 1];

    return std::make_unique<WhitespaceNode>(combined_text, first_token.start_line, first_token.start_column, last_token.end_line, last_token.end_column);
}

std::unique_ptr<CommentNode> BrainfuckParser::parse_comment_sequence() {
    if (current >= tokens.size() || tokens[current].type != TokenType::COMMENT) {
        return nullptr;
    }

    const Token& first_token = tokens[current];
    std::string combined_text;
    size_t current_line = first_token.start_line;

    while (current < tokens.size() && tokens[current].type == TokenType::COMMENT && tokens[current].start_line == current_line) {
        combined_text += tokens[current].text;
        current++;
    }

    const Token& last_token = tokens[current - 1];

    return std::make_unique<CommentNode>(combined_text, first_token.start_line, first_token.start_column, last_token.end_line, last_token.end_column);
}

std::string tree_to_string(const ASTNode* node, int indent) {
    if (node == nullptr) {
        return "";
    }

    std::stringstream ss;
    std::string indent_str(indent * 4, ' ');

    switch (node->type) {
        case NodeType::PROGRAM: {
            ss << indent_str << "Program";
            if (node->end_line > 0) {
                ss << " [1:1 - " << node->end_line << ":" << node->end_column << "]";
            }
            ss << "\n";
            const auto* program = static_cast<const ProgramNode*>(node);
            for (const auto& stmt: program->statements) {
                ss << tree_to_string(stmt.get(), indent + 1);
            }
            break;
        }
        case NodeType::COMMAND: {
            const auto* cmd = static_cast<const CommandNode*>(node);
            const char* cmd_names[] = { "MOVE_RIGHT", "MOVE_LEFT", "INCREMENT", "DECREMENT", "OUTPUT", "INPUT", "LOOP_START", "LOOP_END", "WHITESPACE", "NEWLINE", "COMMENT" };
            int cmd_index = static_cast<int>(cmd->command);
            const char* cmd_name = (cmd_index >= 0 && cmd_index < 11) ? cmd_names[cmd_index] : "UNKNOWN";
            ss << indent_str << "Command: " << cmd_name << " [" << cmd->start_line << ":" << cmd->start_column << "]\n";
            break;
        }
        case NodeType::LOOP: {
            const auto* loop = static_cast<const LoopNode*>(node);
            ss << indent_str << "Loop [" << node->start_line << ":" << node->start_column;
            if (node->end_line != node->start_line || node->end_column != node->start_column) {
                ss << " - " << node->end_line << ":" << node->end_column;
            }
            std::vector<std::string> issues;
            if (!loop->is_terminated) {
                issues.push_back("UNTERMINATED");
            }
            if (loop->is_empty) {
                issues.push_back("EMPTY");
            }
            if (loop->has_single_statement) {
                issues.push_back("SINGLE_STATEMENT");
            }
            if (!issues.empty()) {
                ss << " - ";
                for (size_t i = 0; i < issues.size(); ++i) {
                    if (i > 0) {
                        ss << ", ";
                    }
                    ss << issues[i];
                }
            }
            ss << "]\n";
            for (const auto& stmt: loop->body) {
                ss << tree_to_string(stmt.get(), indent + 1);
            }
            break;
        }
        case NodeType::WHITESPACE: {
            const auto* ws = static_cast<const WhitespaceNode*>(node);
            std::string escaped_text = ws->text;
            size_t pos = 0;
            while ((pos = escaped_text.find('\n', pos)) != std::string::npos) {
                escaped_text.replace(pos, 1, "\\n");
                pos += 2;
            }
            while ((pos = escaped_text.find('\t', pos)) != std::string::npos) {
                escaped_text.replace(pos, 1, "\\t");
                pos += 2;
            }
            ss << indent_str << "Whitespace \"" << escaped_text << "\" [" << ws->start_line << ":" << ws->start_column << " - " << ws->end_line << ":" << ws->end_column << "]\n";
            break;
        }
        case NodeType::COMMENT: {
            const auto* comment = static_cast<const CommentNode*>(node);
            ss << indent_str << "Comment \"" << comment->text << "\" [" << comment->start_line << ":" << comment->start_column << " - " << comment->end_line << ":" << comment->end_column << "]\n";
            break;
        }
        case NodeType::UNMATCHED_CLOSE: {
            ss << indent_str << "UnmatchedClose ']' [" << node->start_line << ":" << node->start_column << "]\n";
            break;
        }
    }

    return ss.str();
}

std::string BrainfuckParser::get_token_name(TokenType type) const {
    const char* cmd_names[] = { "MOVE_RIGHT", "MOVE_LEFT", "INCREMENT", "DECREMENT", "OUTPUT", "INPUT", "LOOP_START", "LOOP_END", "WHITESPACE", "NEWLINE", "COMMENT" };

    int cmd_index = static_cast<int>(type);
    return (cmd_index >= 0 && cmd_index < 11) ? cmd_names[cmd_index] : "UNKNOWN";
}
