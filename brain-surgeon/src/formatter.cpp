#include "formatter_config.hpp"
#include "parser.hpp"
#include <sstream>
#include <string>

class BrainfuckFormatter {
private:
    const FormatterConfig& config;
    std::ostringstream output;
    int current_indent_level = 0;

    // Convert command token to its character representation
    std::string command_to_string(TokenType type) const {
        switch (type) {
            case TokenType::MOVE_RIGHT: return ">";
            case TokenType::MOVE_LEFT: return "<";
            case TokenType::INCREMENT: return "+";
            case TokenType::DECREMENT: return "-";
            case TokenType::OUTPUT: return ".";
            case TokenType::INPUT: return ",";
            default: return "";
        }
    }

    // Check if two commands are in the same group (same operation type)
    bool are_same_group(TokenType a, TokenType b) const {
        // Movement group: < and >
        if ((a == TokenType::MOVE_LEFT || a == TokenType::MOVE_RIGHT) && (b == TokenType::MOVE_LEFT || b == TokenType::MOVE_RIGHT)) {
            return true;
        }
        // Increment/decrement group: + and -
        if ((a == TokenType::INCREMENT || a == TokenType::DECREMENT) && (b == TokenType::INCREMENT || b == TokenType::DECREMENT)) {
            return true;
        }
        // Same exact command
        return a == b;
    }

    // Check if command is a movement command
    bool is_movement_command(TokenType type) const {
        return type == TokenType::MOVE_LEFT || type == TokenType::MOVE_RIGHT;
    }

    // Check if command is an I/O command
    bool is_io_command(TokenType type) const {
        return type == TokenType::INPUT || type == TokenType::OUTPUT;
    }

    // Generate indentation string
    std::string get_indent() const {
        if (config.tab_indent) {
            return std::string(current_indent_level, '\t');
        } else {
            return std::string(current_indent_level * config.indent_spaces, ' ');
        }
    }

    // Add spacing between character groups with tally marks
    std::string format_command_group(const std::string& commands) const {
        if (!config.tally_commands || commands.length() <= 5) {
            return commands;
        }

        std::string result;
        for (size_t i = 0; i < commands.length(); ++i) {
            if (i > 0 && i % 5 == 0) {
                result += ' ';
            }
            result += commands[i];
        }
        return result;
    }

    // Format a sequence of statements
    void format_statements(const std::vector<std::unique_ptr<ASTNode>>& statements) {
        std::string current_line = get_indent();
        std::string command_buffer;
        TokenType last_command_type = TokenType::WHITESPACE;
        bool line_has_content = false;
        std::string pending_comment;

        auto flush_command_buffer = [&]() {
            if (!command_buffer.empty()) {
                if (line_has_content && config.space_between_groups) {
                    current_line += " ";
                }
                current_line += format_command_group(command_buffer);
                command_buffer.clear();
                line_has_content = true;
            }
        };

        auto flush_current_line = [&]() {
            flush_command_buffer();
            if (line_has_content) {
                output << current_line << "\n";
            }
            current_line = get_indent();
            line_has_content = false;
        };

        auto flush_pending_comment = [&]() {
            if (!pending_comment.empty()) {
                if (config.comment_on_newline && line_has_content) {
                    flush_current_line();
                }

                // Check if comment already starts with prefix
                std::string final_comment;
                if (!config.comment_prefix.empty() && pending_comment.length() >= config.comment_prefix.length() && pending_comment.substr(0, config.comment_prefix.length()) == config.comment_prefix)
                {
                    // Comment already has prefix, use as-is
                    final_comment = pending_comment;
                } else {
                    // Comment doesn't have prefix, prepend it
                    final_comment = config.comment_prefix + pending_comment;
                }

                output << get_indent() << final_comment << "\n";
                pending_comment.clear();
            }
        };

        for (const auto& stmt: statements) {
            switch (stmt->type) {
                case NodeType::COMMAND: {
                    flush_pending_comment();

                    const auto* cmd = static_cast<const CommandNode*>(stmt.get());
                    std::string cmd_char = command_to_string(cmd->command);

                    // Check if we need to start a new command group
                    if (!command_buffer.empty() && !are_same_group(last_command_type, cmd->command)) {
                        flush_command_buffer();
                    }

                    // Handle newline BEFORE movement groups
                    if (is_movement_command(cmd->command) && config.move_on_newline && command_buffer.empty() && line_has_content) {
                        flush_current_line();
                    }

                    // Add command to buffer
                    command_buffer += cmd_char;
                    last_command_type = cmd->command;

                    // Handle newline after I/O commands
                    if (is_io_command(cmd->command) && config.end_line_at_io) {
                        flush_current_line();
                    }
                    break;
                }

                case NodeType::LOOP: {
                    flush_pending_comment();
                    flush_current_line();

                    const auto* loop = static_cast<const LoopNode*>(stmt.get());

                    if (config.loop_on_newline) {
                        output << get_indent() << "[\n";
                        current_indent_level++;
                        format_statements(loop->body);
                        current_indent_level--;
                        output << get_indent() << "]\n";
                    } else {
                        output << get_indent() << "[";
                        current_indent_level++;
                        format_statements(loop->body);
                        current_indent_level--;
                        output << "]\n";
                    }
                    break;
                }

                case NodeType::COMMENT: {
                    const auto* comment = static_cast<const CommentNode*>(stmt.get());

                    if (!pending_comment.empty()) {
                        pending_comment += " ";
                    }
                    pending_comment += comment->text;
                    break;
                }

                case NodeType::UNMATCHED_CLOSE: {
                    flush_pending_comment();
                    flush_current_line();
                    output << get_indent() << "]\n";
                    break;
                }

                default: break;
            }
        }

        // Flush any remaining content
        flush_pending_comment();
        flush_current_line();
    }

public:
    explicit BrainfuckFormatter(const FormatterConfig& cfg): config(cfg) {}

    std::string format(const ProgramNode* program) {
        output.str("");
        output.clear();
        current_indent_level = 0;

        if (program != nullptr && !program->statements.empty()) {
            format_statements(program->statements);
        }

        return output.str();
    }
};

// Public interface function
std::string format_tree(const ProgramNode* root, const FormatterConfig& config = FormatterConfig {}) {
    BrainfuckFormatter formatter(config);
    return formatter.format(root);
}
