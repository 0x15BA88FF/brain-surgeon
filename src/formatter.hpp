#pragma once

#include "formatter_config.hpp"
#include "parser.hpp"
#include <string>

std::string format_tree(const ProgramNode* root, const FormatterConfig& config);
