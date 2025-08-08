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
/// @file cli.h
/// This file defines the interface for parsing command-line arguments: the
/// `CliArgs` class which contains the arguments, and the `parseCli` function
/// for parsing arguments from `argv`.
//
//===-----------------------------------------------------------------------===//

#ifndef TAM_CLI_H__
#define TAM_CLI_H__

#include <optional>
#include <string>

namespace tam {

/// Contains the parsed CLI arguments.
///
/// The three flags all default to `false` for simplicity.
struct CliArgs {
    std::string Filename; ///< Name of binary file
    bool Trace = false,   ///< If `true`, print memory at each instruction
        Step = false,     ///< If `true`, wait for RETURN before continuing
        Help = false;     ///< If `true`, print help message
};

/// Parse the arguments given to the program.
///
/// @param Argc number of arguments
/// @param Argv the arguments
/// @return the parsed arguments, if parsing was successful
std::optional<CliArgs> parseCli(int Argc, const char **Argv);

} // namespace tam

#endif // TAM_CLI_H__
