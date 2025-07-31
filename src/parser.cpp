#include "parser.hpp"
#include <iostream>

std::unique_ptr<ProgramNode> BrainfuckParser::parse(const std::vector<Token> &tokenList) {
    tokens = tokenList;
    current = 0;
    errors.clear(); // Clear any previous errors

    auto program = std::make_unique<ProgramNode>();

    while (current < tokens.size()) {
        auto stmt = parseStatement();

        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
    }

    return program;
}

std::unique_ptr<ASTNode> BrainfuckParser::parseStatement() {
    if (current >= tokens.size()) {
        return nullptr;
    }

    const Token &token = tokens[current];

    if (token.type == TokenType::LOOP_END) {
        errors.push_back({"Unmatched ']' - missing '['", token.line, token.column});
        current++;
        return nullptr;
    }

    if (token.type == TokenType::LOOP_START) {
        return parseLoop();
    }

    // Inline canceling pair check
    if (current > 0) {
        const Token &prev = tokens[current - 1];
        const Token &curr = token;

        bool cancels = (prev.type == TokenType::MOVE_LEFT && curr.type == TokenType::MOVE_RIGHT) ||
                       (prev.type == TokenType::MOVE_RIGHT && curr.type == TokenType::MOVE_LEFT) ||
                       (prev.type == TokenType::INCREMENT && curr.type == TokenType::DECREMENT) ||
                       (prev.type == TokenType::DECREMENT && curr.type == TokenType::INCREMENT);

        if (cancels) {
            warnings.push_back({"Consecutive canceling commands", curr.line, curr.column});
        }
    }

    current++;
    return std::make_unique<CommandNode>(token.type, token.line, token.column);
}

std::unique_ptr<LoopNode> BrainfuckParser::parseLoop() {
    if (current >= tokens.size() || tokens[current].type != TokenType::LOOP_START) {
        errors.push_back({"Expected '[' at start of loop",
                          current < tokens.size() ? tokens[current].line : 0,
                          current < tokens.size() ? tokens[current].column : 0});
        return nullptr;
    }

    const Token &startToken = tokens[current];
    current++; // consume '['

    auto loop = std::make_unique<LoopNode>(startToken.line, startToken.column);

    while (current < tokens.size() && tokens[current].type != TokenType::LOOP_END) {
        auto stmt = parseStatement();
        if (stmt) {
            loop->body.push_back(std::move(stmt));
        }
    }

    if (current >= tokens.size()) {
        errors.push_back({"Unmatched '[' - missing ']'", startToken.line, startToken.column});
    } else {
        current++; // consume ']'
    }

    // Inline infinite loop pattern check
    if (loop->body.empty()) {
        warnings.push_back({"Empty loop (potential infinite loop)", loop->line, loop->column});
    } else if (loop->body.size() == 1 && loop->body[0]->type == NodeType::COMMAND) {
        TokenType cmd = static_cast<CommandNode *>(loop->body[0].get())->command;
        if (cmd == TokenType::INCREMENT || cmd == TokenType::DECREMENT) {
            warnings.push_back({"Suspicious loop with single increment/decrement (likely infinite)",
                                loop->line, loop->column});
        }
    }

    return loop;
}

void BrainfuckParser::printAST(const ASTNode *node, int indent) const {
    if (!node)
        return;

    std::string indentStr(indent * 2, ' ');

    switch (node->type) {
    case NodeType::PROGRAM: {
        std::cout << indentStr << "Program\n";
        const auto *program = static_cast<const ProgramNode *>(node);
        for (const auto &stmt : program->statements) {
            printAST(stmt.get(), indent + 1);
        }
        break;
    }

    case NodeType::COMMAND: {
        const auto *cmd = static_cast<const CommandNode *>(node);
        const char *cmdNames[] = {"MOVE_RIGHT", "MOVE_LEFT", "INCREMENT",  "DECREMENT",
                                  "OUTPUT",     "INPUT",     "LOOP_START", "LOOP_END"};
        std::cout << indentStr << "Command: " << cmdNames[static_cast<int>(cmd->command)] << " ("
                  << cmd->line << ":" << cmd->column << ")\n";
        break;
    }

    case NodeType::LOOP: {
        std::cout << indentStr << "Loop (" << node->line << ":" << node->column << ")\n";
        const auto *loop = static_cast<const LoopNode *>(node);
        for (const auto &stmt : loop->body) {
            printAST(stmt.get(), indent + 1);
        }
        break;
    }
    }
}

void BrainfuckParser::printWarnings() const {
    for (const auto &warning : warnings) {
        std::cout << "Warning at " << warning.line << ":" << warning.column << " - "
                  << warning.message << "\n";
    }
}

void BrainfuckParser::printErrors() const {
    for (const auto &error : errors) {
        std::cout << "Error at " << error.line << ":" << error.column << " - " << error.message
                  << "\n";
    }
}
