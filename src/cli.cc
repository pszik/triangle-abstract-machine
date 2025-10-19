//===-----------------------------------------------------------------------===//
//
// This file is part of tam-cpp, copyright (c) Ian Knight 2025.
//
// tam-cpp is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// tam-cpp is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with tam-cpp. If not, see <https://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//
//
/// @file cli.cc
/// This file defines the `ParseCli` function used for processing command-line
/// arguments. It also defines some static auxiliary functions used in the
/// process.
//
//===-----------------------------------------------------------------------===//

#include "tam/cli.h"

#include <string.h>

#include <optional>
#include <stack>

#define HELP_ARG(s) \
    ((strncmp(s, "-h", 2) == 0) || (strncmp(s, "--help", 6) == 0))
#define TRACE_ARG(s) \
    ((strncmp(s, "-t", 2) == 0) || (strncmp(s, "--trace", 7) == 0))
#define STEP_ARG(s) \
    ((strncmp(s, "-s", 2) == 0) || (strncmp(s, "--step", 6) == 0))

std::optional<tam::CliArgs> tam::ParseCli(int argc, const char **argv) {
    enum StackSymbol { NT_Cli, NT_Trace, T_filename, T_help, T_trace, T_step };
    std::stack<StackSymbol> stack;
    tam::CliArgs args;
    int i = 0;

    stack.push(NT_Cli);
    while (!stack.empty()) {
        const char *token = argv[i];
        StackSymbol s = stack.top();
        stack.pop();

        switch (s) {
            case T_filename:
                args.filename = token;
                break;
            case T_help:
                if (HELP_ARG(token)) {
                    args.help = true;
                    i++;
                    break;
                } else {
                    return {};
                }
            case T_trace:
                if (TRACE_ARG(token)) {
                    args.trace = true;
                    i++;
                    break;
                } else {
                    return {};
                }
            case T_step:
                if (STEP_ARG(token)) {
                    args.step = true;
                    i++;
                    break;
                } else {
                    return {};
                }
            case NT_Cli:
                if (HELP_ARG(token)) {
                    stack.push(T_help);
                } else if (TRACE_ARG(token)) {
                    stack.push(NT_Trace);
                    stack.push(T_trace);
                } else if (STEP_ARG(token)) {
                    stack.push(T_filename);
                    stack.push(T_trace);
                    stack.push(T_step);
                } else {
                    stack.push(T_filename);
                }
                break;
            case NT_Trace:
                if (STEP_ARG(token)) {
                    stack.push(T_filename);
                    stack.push(T_step);
                } else if (HELP_ARG(token) || TRACE_ARG(token)) {
                    return {};
                } else {
                    stack.push(T_filename);
                }
                break;
        }
    }

    return args;
}
