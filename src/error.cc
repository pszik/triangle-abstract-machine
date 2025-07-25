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

/// @file error.cc
/// Implements the error-creation method.

#include <tam/error.h>

#include <sstream>

namespace tam {
const std::runtime_error runtimeError(ExceptionKind Kind, uint16_t Addr) {
    std::stringstream ss;
    switch (Kind) {
    case EK_CodeAccessViolation:
        ss << "code access violation";
        break;
    case EK_DataAccessViolation:
        ss << "data access violation";
        break;
    case EK_StackOverflow:
        ss << "stack overflow";
        break;
    case EK_StackUnderflow:
        ss << "stack underflow";
        break;
    case EK_HeapOverflow:
        ss << "heap overflow";
        break;
    case EK_UnknownOpcode:
        ss << "unknown opcode";
        break;
    case EK_IOError:
        ss << "IO error";
        break;
    }

    if (Kind != EK_IOError) {
        ss << " at loc " << Addr;
    }
    return std::runtime_error(ss.str());
}
} // namespace tam
