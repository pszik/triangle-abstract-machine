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

/// Parse arguments when a single argument was given.
///
/// Valid arguments in this case are the help flag, or a filename by itself.
///
/// @param Argv the arguments to parse with the program name removed
/// @returns the `CliArgs` if parsing succeeded or empty if not
static std::optional<tam::CliArgs> Parse1(const char **Argv);

/// Parse arguments when two argumens were given.
///
/// Valid arguments in this case are either the trace or step flag, followed by
/// a filename.
///
/// @param Argv the arguments to parse with the program name removed
/// @returns the `CliArgs` if parsing succeeded or empty if not
static std::optional<tam::CliArgs> Parse2(const char **Argv);

/// Parse arguments when three arguments were given.
///
/// The only valid arguments here consist of both trace and step flags
/// separately, followed by a filename.
///
/// @param Argv the arguments to parse with the program name removed
/// @returns the `CliArgs` if parsing succeeded or empty if not
static std::optional<tam::CliArgs> Parse3(const char **Argv);

/// Determines the number of arguments given and calls the appropriate
/// auxiliary function based on that.
std::optional<tam::CliArgs> tam::ParseCli(int Argc, const char **Argv) {
    switch (Argc) {
        case 2:
            return Parse1(Argv + 1);
        case 3:
            return Parse2(Argv + 1);
        case 4:
            return Parse3(Argv + 1);
        default:
            return {};
    }
}

/// Indicates if an argument string matches the trace option.
///
/// @param arg the argument
/// @returns `true` if the argument is a trace flag
static inline bool IsTraceArg(const char *arg) {
    return (strcmp(arg, "-t") == 0 || strcmp(arg, "--trace") == 0);
}

/// Indicates if an argument string matches the step option.
///
/// @param arg the argument
/// @returns `true` if the argument is a step flag
static inline bool IsStepArg(const char *arg) {
    return (strcmp(arg, "-s") == 0 || strcmp(arg, "--step") == 0);
}

static std::optional<tam::CliArgs> Parse1(const char **argv) {
    if (IsTraceArg(argv[0]) || IsStepArg(argv[0])) return {};

    if (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "--help") == 0)
        return tam::CliArgs{.filename = "", .help = true};

    return tam::CliArgs{std::string(argv[0])};
}

static std::optional<tam::CliArgs> Parse2(const char **argv) {
    if (IsTraceArg(argv[0]))
        return tam::CliArgs{.filename = std::string(argv[1]), .trace = true};

    if (IsStepArg(argv[0]))
        return tam::CliArgs{.filename = std::string(argv[1]), .step = true};

    if (strcmp(argv[0], "-ts") == 0 || strcmp(argv[0], "-st") == 0) {
        return tam::CliArgs{
            .filename = std::string(argv[2]), .trace = true, .step = true};
    }

    return {};
}

static std::optional<tam::CliArgs> Parse3(const char **argv) {
    if ((IsTraceArg(argv[0]) && IsStepArg(argv[1])) ||
        (IsStepArg(argv[0]) && IsTraceArg(argv[1]))) {
        return tam::CliArgs{
            .filename = std::string(argv[2]), .trace = true, .step = true};
    }

    return {};
}
