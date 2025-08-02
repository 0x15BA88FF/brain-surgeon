#pragma once

#include <string>

struct FormatterConfig {
    bool space_between_groups = true; // add a space after a uniqe command group
    std::string comment_prefix = "#"; // prefix used before a comment starts
    bool comment_on_newline = false;  // start every comment on a newline
    bool loop_on_newline = true;      // put start and end [/] of a loop on a newline
    bool move_on_newline = true;      // put move statement goup </> on a newline
    bool end_line_at_io = true;       // add a newline after evert read or write .,
    bool tally_commands = true;       // put a single space beteen every five characters in a group
    bool tab_indent = false;          // use \t instead or spaces in indents
    int indent_spaces = 4;            // number of indents to insert
};
