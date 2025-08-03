#pragma once

#include "parser.hpp"
#include <string>
#include <vector>

enum class LintSeverity { INFO, WARNING, ERROR };

inline std::string level_to_string(LintSeverity severity) {
    switch (severity) {
        case LintSeverity::INFO: return "info";
        case LintSeverity::WARNING: return "warning";
        case LintSeverity::ERROR: return "error";
        default: return "unknown";
    }
}

struct LintDiagnostic {
    size_t start_line;
    size_t start_column;
    size_t end_line;
    size_t end_column;
    std::string message;
    LintSeverity severity;
};

std::vector<LintDiagnostic> lint_tree(const ASTNode* node);
std::string lint_to_json(const ASTNode* node);
