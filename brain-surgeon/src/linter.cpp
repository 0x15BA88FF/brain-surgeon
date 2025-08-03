#include "linter.hpp"
#include "../include/json.hpp"
#include "parser.hpp"
#include <vector>

using json = nlohmann::json;

std::vector<LintDiagnostic> lint_tree(const ASTNode* node) {
    std::vector<LintDiagnostic> diagnostics;

    if (node == nullptr) {
        return diagnostics;
    }

    if (node->type == NodeType::PROGRAM) {
        const auto* prog = static_cast<const ProgramNode*>(node);
        const auto& stmts = prog->statements;

        if (stmts.empty()) {
            diagnostics.push_back({ 0, 0, 0, 0, "Empty file", LintSeverity::WARNING });
            return diagnostics;
        }

        for (size_t i = 0; i < stmts.size(); ++i) {
            const auto* stmt = stmts[i].get();

            if (stmt->type == NodeType::COMMENT && i > 0 && i + 1 < stmts.size()) {
                const auto* prev = stmts[i - 1].get();
                const auto* next = stmts[i + 1].get();

                if ((prev->type == NodeType::COMMAND || prev->type == NodeType::LOOP) && (next->type == NodeType::COMMAND || next->type == NodeType::LOOP)) {
                    diagnostics.push_back({ stmt->start_line, stmt->start_column, stmt->end_line, stmt->end_column, "Comment between commands", LintSeverity::WARNING });
                }
            }

            if (stmt->type == NodeType::COMMAND && i + 1 < stmts.size()) {
                const auto* next = stmts[i + 1].get();
                const auto* cmd1 = static_cast<const CommandNode*>(stmt);

                if (next->type == NodeType::COMMAND) {
                    const auto* cmd2 = static_cast<const CommandNode*>(next);

                    if ((cmd1->command == TokenType::INCREMENT && cmd2->command == TokenType::DECREMENT) || (cmd1->command == TokenType::DECREMENT && cmd2->command == TokenType::INCREMENT)
                        || (cmd1->command == TokenType::MOVE_LEFT && cmd2->command == TokenType::MOVE_RIGHT) || (cmd1->command == TokenType::MOVE_RIGHT && cmd2->command == TokenType::MOVE_LEFT))
                    {
                        diagnostics.push_back({ cmd1->start_line, cmd1->start_column, cmd2->end_line, cmd2->end_column, "Consecutive canceling commands", LintSeverity::WARNING });
                    }
                }
            }

            auto child_diags = lint_tree(stmt);
            diagnostics.insert(diagnostics.end(), child_diags.begin(), child_diags.end());
        }
    } else if (node->type == NodeType::LOOP) {
        const auto* loop = static_cast<const LoopNode*>(node);

        if (!loop->is_terminated) {
            diagnostics.push_back({ loop->start_line, loop->start_column, loop->end_line, loop->end_column, "Unmatched '[' - missing ']'", LintSeverity::ERROR });
        }

        if (loop->is_empty) {
            diagnostics.push_back({ loop->start_line, loop->start_column, loop->end_line, loop->end_column, "Empty loop (potential infinite loop)", LintSeverity::WARNING });
        }

        if (loop->has_single_statement) {
            diagnostics.push_back({ loop->start_line, loop->start_column, loop->end_line, loop->end_column, "Loop with single command (suspicious)", LintSeverity::WARNING });
        }

        for (const auto& child: loop->body) {
            auto nested_diags = lint_tree(child.get());
            diagnostics.insert(diagnostics.end(), nested_diags.begin(), nested_diags.end());
        }
    } else if (node->type == NodeType::UNMATCHED_CLOSE) {
        diagnostics.push_back({ node->start_line, node->start_column, node->end_line, node->end_column, "Unmatched ']' - missing '['", LintSeverity::ERROR });
    }

    return diagnostics;
}

std::string lint_to_json(const ASTNode* node) {
    json result = json::array();

    for (const auto& report: lint_tree(node)) {
        result.push_back(
            { { "message", report.message },
              { "level", level_to_string(report.severity) },
              { "startLine", report.start_line },
              { "startColumn", report.start_column },
              { "endLine", report.end_line },
              { "endColumn", report.end_column } }
        );
    }

    return result.dump();
}
