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

#include <cstdlib>
#include <optional>
#include <stack>

static bool IsHelpTok(const char* tok) {
    return (strncmp(tok, "-h", 2) == 0 || strncmp(tok, "--help", 6) == 0);
}

static bool IsTraceTok(const char* tok) {
    return (strncmp(tok, "-t", 2) == 0 || strncmp(tok, "--trace", 7) == 0);
}

static bool IsStepTok(const char* tok) {
    return (strncmp(tok, "-s", 2) == 0 || strncmp(tok, "--step", 6) == 0);
}

static bool IsTraceLvlTok(const char* tok) {
    switch (tok[0]) {
        case '0':
        case '1':
        case '2':
        case '3':
            return true;
        default:
            return false;
    }
}

enum StackSymbol {
    Cli,
    Trace,
    TraceLvl,
    TraceExt,
    tok_help,
    tok_trace,
    tok_trace_lvl,
    tok_step,
    tok_filename,
};

std::optional<CliArgs> ParseCli(int argc, const char** argv) noexcept {
    if (argc < 1) {
        return {};
    }

    CliArgs args;
    std::stack<StackSymbol> stack;
    stack.push(Cli);

    int i = 0;
    while (!stack.empty() && i < argc) {
        StackSymbol s = stack.top();
        stack.pop();
        switch (s) {
            case Cli:
                if (IsHelpTok(argv[i])) {
                    stack.push(tok_help);
                } else if (IsTraceTok(argv[i])) {
                    stack.push(TraceExt);
                    stack.push(Trace);
                } else if (IsStepTok(argv[i])) {
                    stack.push(tok_filename);
                    stack.push(Trace);
                    stack.push(tok_step);
                } else {
                    stack.push(tok_filename);
                }
                break;
            case Trace:
                stack.push(TraceLvl);
                stack.push(tok_trace);
                break;
            case TraceLvl:
                if (IsTraceLvlTok(argv[i])) {
                    stack.push(tok_trace_lvl);
                } else if (!(IsStepTok(argv[i]) || i + 1 >= argc)) {
                    args.error = true;
                }
                break;
            case TraceExt:
                if (IsStepTok(argv[i])) {
                    stack.push(tok_filename);
                    stack.push(tok_step);
                } else {
                    stack.push(tok_filename);
                }
                break;
            case tok_help:
                if (IsHelpTok(argv[i])) {
                    args.help = true;
                } else {
                    args.error = true;
                }
                i++;
                break;
            case tok_trace:
                if (IsTraceTok(argv[i])) {
                    args.trace = 0;
                } else {
                    args.error = true;
                }
                i++;
                break;
            case tok_trace_lvl:
                if (IsTraceLvlTok(argv[i])) {
                    args.trace = atoi(argv[i]);
                } else {
                    args.error = true;
                }
                i++;
                break;
            case tok_step:
                if (IsStepTok(argv[i])) {
                    args.step = true;
                } else {
                    args.error = true;
                }
                i++;
                break;
            case tok_filename:
                args.filename = argv[i];
                i++;
                break;
        }
    }

    if (!stack.empty() || args.error || (!args.help && !args.filename)) {
        return {};
    }
    return args;
}
