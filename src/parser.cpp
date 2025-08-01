#include "parser.hpp"
#include "lexer.hpp"
#include <iostream>

std::unique_ptr<ProgramNode> BrainfuckParser::parse(const std::vector<Token> &tokenList) {
    current = 0;
    errors.clear();
    warnings.clear();
    tokens = tokenList;

    auto program = std::make_unique<ProgramNode>();
    Token *prevValidToken = nullptr;
    size_t prevValidIndex = 0;

    while (current < tokens.size()) {
        auto stmt = parseStatement(prevValidToken, prevValidIndex);

        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
    }

    return program;
}

std::unique_ptr<ASTNode> BrainfuckParser::parseStatement(Token *&prevValidToken,
                                                         size_t &prevValidIndex) {
    if (current >= tokens.size()) {
        return nullptr;
    }

    const Token &token = tokens[current];

    if (token.type == TokenType::WHITESPACE) {
        current++;
        return std::make_unique<WhitespaceNode>(token.text, token.line, token.column);
    } else if (token.type == TokenType::COMMENT) {
        current++;
        return std::make_unique<CommentNode>(token.text, token.line, token.column);
    } else if (token.type == TokenType::LOOP_START) {
        return parseLoop();
    } else if (token.type == TokenType::LOOP_END) {
        errors.push_back({"Unmatched ']' - missing '['", token.line, token.column});
        current++;
        return nullptr;
    } else {
        if (token.isValid) {
            if (prevValidToken) {
                auto isCanceling = [](TokenType prev, TokenType curr) {
                    return (prev == TokenType::MOVE_LEFT && curr == TokenType::MOVE_RIGHT) ||
                           (prev == TokenType::MOVE_RIGHT && curr == TokenType::MOVE_LEFT) ||
                           (prev == TokenType::INCREMENT && curr == TokenType::DECREMENT) ||
                           (prev == TokenType::DECREMENT && curr == TokenType::INCREMENT);
                };

                if (isCanceling(prevValidToken->type, token.type)) {
                    warnings.push_back(
                        {"Consecutive canceling commands", token.line, token.column});
                }

                // Check for comment between prevValidToken and current
                for (size_t i = prevValidIndex + 1; i < current; ++i) {
                    if (tokens[i].type == TokenType::COMMENT && tokens[i].line == token.line) {
                        warnings.push_back(
                            {"Comment between commands", tokens[i].line, tokens[i].column});
                    }
                }
            }

            prevValidToken = &tokens[current];
            prevValidIndex = current;
        }

        current++;
        return std::make_unique<CommandNode>(token.type, token.line, token.column);
    }
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
    Token *prevValidToken = nullptr;
    size_t prevValidIndex = 0;

    while (current < tokens.size() && tokens[current].type != TokenType::LOOP_END) {
        auto stmt = parseStatement(prevValidToken, prevValidIndex);

        if (stmt) {
            loop->body.push_back(std::move(stmt));
        }
    }

    if (current >= tokens.size()) {
        errors.push_back({"Unmatched '[' - missing ']'", startToken.line, startToken.column});
    } else {
        current++; // consume ']'
    }

    // Only count valid command nodes
    size_t validCount = 0;
    TokenType singleCommand = TokenType::WHITESPACE;

    for (const auto &stmt : loop->body) {
        if (stmt->type == NodeType::COMMAND) {
            validCount++;
            singleCommand = static_cast<CommandNode *>(stmt.get())->command;
        }
    }

    if (validCount == 0) {
        warnings.push_back({"Empty loop (potential infinite loop)", loop->line, loop->column});
    } else if (validCount == 1) {
        if (singleCommand == TokenType::INCREMENT) {
            warnings.push_back({"Infinite loop with single +", loop->line, loop->column});
        } else if (singleCommand == TokenType::DECREMENT) {
            warnings.push_back({"Suspicious decrement loop", loop->line, loop->column});
        } else if (singleCommand == TokenType::MOVE_LEFT ||
                   singleCommand == TokenType::MOVE_RIGHT) {
            warnings.push_back({"Suspicious movement loop", loop->line, loop->column});
        }
    }

    return loop;
}

void BrainfuckParser::printAST(const ASTNode *node, int indent) const {
    if (!node)
        return;

    std::string indentStr(indent * 4, ' ');

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

        std::cout << indentStr << "Command: " << cmdNames[static_cast<int>(cmd->command)] << " ["
                  << cmd->line << ":" << cmd->column << "]\n";
        break;
    }
    case NodeType::LOOP: {
        std::cout << indentStr << "Loop [" << node->line << ":" << node->column << "]\n";

        const auto *loop = static_cast<const LoopNode *>(node);

        for (const auto &stmt : loop->body) {
            printAST(stmt.get(), indent + 1);
        }

        break;
    }
    case NodeType::WHITESPACE: {
        const auto *ws = static_cast<const WhitespaceNode *>(node);

        std::cout << indentStr << "Whitespace \"" << ws->text << "\" [" << ws->line << ":"
                  << ws->column << "]\n";

        break;
    }
    case NodeType::COMMENT: {
        const auto *com = static_cast<const CommentNode *>(node);

        std::cout << indentStr << "Comment \"" << com->text << "\" [" << com->line << ":"
                  << com->column << "]\n";

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
