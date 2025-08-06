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
/// This file defines the `parseCli` function used for processing command-line
/// arguments. It also defines some static auxiliary functions used in the
/// process.
//
//===-----------------------------------------------------------------------===//

#include <tam/cli.h>

#include <cstring>
#include <optional>

using namespace tam;

/// Parse arguments when a single argument was given.
///
/// Valid arguments in this case are the help flag, or a filename by itself.
///
/// @param Argv the arguments to parse with the program name removed
/// @returns the `CliArgs` if parsing succeeded or empty if not
static std::optional<CliArgs> parse1(const char **Argv);

/// Parse arguments when two argumens were given.
///
/// Valid arguments in this case are either the trace or step flag, followed by
/// a filename.
///
/// @param Argv the arguments to parse with the program name removed
/// @returns the `CliArgs` if parsing succeeded or empty if not
static std::optional<CliArgs> parse2(const char **Argv);

/// Parse arguments when three arguments were given.
///
/// The only valid arguments here consist of both trace and step flags
/// separately, followed by a filename.
///
/// @param Argv the arguments to parse with the program name removed
/// @returns the `CliArgs` if parsing succeeded or empty if not
static std::optional<CliArgs> parse3(const char **Argv);

/// Determines the number of arguments given and calls the appropriate
/// auxiliary function based on that.
std::optional<CliArgs> tam::parseCli(int Argc, const char **Argv) {
    switch (Argc) {
    case 2:
        return parse1(Argv + 1);
    case 3:
        return parse2(Argv + 1);
    case 4:
        return parse3(Argv + 1);
    default:
        return {};
    }
}

/// Indicates if an argument string matches the trace option.
///
/// @param Arg the argument
/// @returns `true` if the argument is a trace flag
static inline bool isTraceArg(const char *Arg) {
    return (strcmp(Arg, "-t") == 0 || strcmp(Arg, "--trace") == 0);
}

/// Indicates if an argument string matches the step option.
///
/// @param Arg the argument
/// @returns `true` if the argument is a step flag
static inline bool isStepArg(const char *Arg) {
    return (strcmp(Arg, "-s") == 0 || strcmp(Arg, "--step") == 0);
}

static std::optional<CliArgs> parse1(const char **Argv) {
    if (isTraceArg(Argv[0]) || isStepArg(Argv[0]))
        return {};

    if (strcmp(Argv[0], "-h") == 0 || strcmp(Argv[0], "--help") == 0)
        return CliArgs{.Filename = "", .Help = true};

    return CliArgs{std::string(Argv[0])};
}

static std::optional<CliArgs> parse2(const char **Argv) {
    if (isTraceArg(Argv[0]))
        return CliArgs{.Filename = std::string(Argv[1]), .Trace = true};

    if (isStepArg(Argv[0]))
        return CliArgs{.Filename = std::string(Argv[1]), .Step = true};

    if (strcmp(Argv[0], "-ts") == 0 || strcmp(Argv[0], "-st") == 0) {
        return CliArgs{
            .Filename = std::string(Argv[2]), .Trace = true, .Step = true};
    }

    return {};
}

static std::optional<CliArgs> parse3(const char **Argv) {
    if ((isTraceArg(Argv[0]) && isStepArg(Argv[1])) ||
        (isStepArg(Argv[0]) && isTraceArg(Argv[1]))) {
        return CliArgs{
            .Filename = std::string(Argv[2]), .Trace = true, .Step = true};
    }

    return {};
}
