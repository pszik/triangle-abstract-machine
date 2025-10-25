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

#include <memory>
#include <stack>

#define HELP_ARG(s) \
    ((strncmp(s, "-h", 2) == 0) || (strncmp(s, "--help", 6) == 0))
#define TRACE_ARG(s) \
    ((strncmp(s, "-t", 2) == 0) || (strncmp(s, "--trace", 7) == 0))
#define STEP_ARG(s) \
    ((strncmp(s, "-s", 2) == 0) || (strncmp(s, "--step", 6) == 0))
#define COMBO_ARG(s) \
    ((strncmp(s, "-st", 3) == 0) || (strncmp(s, "-ts", 3) == 0))

std::unique_ptr<CliArgs> ParseCli(int argc, const char **argv) noexcept {
    enum StackSymbol {
        kNtCli,
        kNtTrace,
        kTokFilename,
        kTokHelp,
        kTokTrace,
        kTokStep,
        kTokCombo,
    };
    std::stack<StackSymbol> stack;
    std::unique_ptr<CliArgs> args(new CliArgs);
    int i = 0;

    stack.push(kNtCli);
    while (!stack.empty() && i < argc) {
        const char *token = argv[i];
        StackSymbol s = stack.top();
        stack.pop();

        switch (s) {
            case kTokFilename:
                args->filename = token;
                i++;
                break;
            case kTokHelp:
                if (!HELP_ARG(token)) {
                    return nullptr;
                }

                args->help = true;
                i++;
                break;
            case kTokTrace:
                if (!TRACE_ARG(token)) {
                    return nullptr;
                }

                args->trace = true;
                i++;
                break;
            case kTokStep:
                if (!STEP_ARG(token)) {
                    return nullptr;
                }

                args->step = true;
                i++;
                break;
            case kTokCombo:
                if (!COMBO_ARG(token)) {
                    return nullptr;
                }

                args->step = true;
                args->trace = true;
                i++;
                break;
            case kNtCli:
                if (HELP_ARG(token)) {
                    stack.push(kTokHelp);
                } else if (TRACE_ARG(token)) {
                    stack.push(kNtTrace);
                    stack.push(kTokTrace);
                } else if (STEP_ARG(token)) {
                    stack.push(kTokFilename);
                    stack.push(kTokTrace);
                    stack.push(kTokStep);
                } else if (COMBO_ARG(token)) {
                    stack.push(kTokFilename);
                    stack.push(kTokCombo);
                } else {
                    stack.push(kTokFilename);
                }
                break;
            case kNtTrace:
                if (HELP_ARG(token) || TRACE_ARG(token)) {
                    return nullptr;
                }

                if (STEP_ARG(token)) {
                    stack.push(kTokFilename);
                    stack.push(kTokStep);
                } else {
                    stack.push(kTokFilename);
                }
                break;
        }
    }

    if (i < argc - 1) {
        return nullptr;
    };
    if (!args->help && args->filename == "") {
        return nullptr;
    }
    return args;
}
