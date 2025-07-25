/*
 * This file is part of tam-cpp, copyright (c) Ian Knight 2025.
 *
 * tam-cpp is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * tam-cpp is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with tam-cpp. If not, see <https://www.gnu.org/licenses/>.
 */

/// @file cli.cc
/// Implements the CLI-parsing interface.

#include <cstring>
#include <optional>
#include <tam/cli.h>

namespace tam {

static std::optional<CliArgs> parse1(const char **Argv);
static std::optional<CliArgs> parse2(const char **Argv);
static std::optional<CliArgs> parse3(const char **Argv);

std::optional<CliArgs> parseCli(int Argc, const char **Argv) {
    switch (Argc) {
    case 2:
        return parse1(Argv + 1);
        break;
    case 3:
        return parse2(Argv + 1);
        break;
    case 4:
        return parse3(Argv + 1);
        break;
    default:
        return {};
    }
}

static inline bool isTraceArg(const char *Arg) {
    return (strcmp(Arg, "-t") == 0 || strcmp(Arg, "--trace") == 0);
}

static inline bool isStepArg(const char *Arg) {
    return (strcmp(Arg, "-s") == 0 || strcmp(Arg, "--step") == 0);
}

static std::optional<CliArgs> parse1(const char **Argv) {
    if (isTraceArg(Argv[0]) || isStepArg(Argv[0])) {
        return {};
    }
    if (strcmp(Argv[0], "-h") == 0 || strcmp(Argv[0], "--help") == 0) {
        return CliArgs{"", false, false, true};
    }
    return CliArgs{std::string(Argv[0]), false, false, false};
}

static std::optional<CliArgs> parse2(const char **Argv) {
    if (isTraceArg(Argv[0])) {
        return CliArgs{std::string(Argv[1]), true, false, false};
    } else if (isStepArg(Argv[0])) {
        return CliArgs{std::string(Argv[1]), false, true, false};
    } else if (strcmp(Argv[0], "-ts") == 0 || strcmp(Argv[0], "-st") == 0) {
        return CliArgs{std::string(Argv[2]), true, true, false};
    } else {
        return {};
    }
}

static std::optional<CliArgs> parse3(const char **Argv) {
    if ((isTraceArg(Argv[0]) && isStepArg(Argv[1])) ||
        (isStepArg(Argv[0]) && isTraceArg(Argv[1]))) {
        return CliArgs{std::string(Argv[2]), true, true, false};
    } else {
        return {};
    }
}

} // namespace tam
