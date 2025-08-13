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
/// @file error.cc
/// This file defines the `RuntimeError` function for constructing exception
/// objects during runtime.
//
//===-----------------------------------------------------------------------===//

#include "tam/error.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

const std::runtime_error tam::RuntimeError(ExceptionKind kind, uint16_t addr) {
    std::stringstream ss;
    ss << "error: ";

    switch (kind) {
        case kCodeAccessViolation:
            ss << "code access violation";
            break;
        case kDataAccessViolation:
            ss << "data access violation";
            break;
        case kStackOverflow:
            ss << "stack overflow";
            break;
        case kStackUnderflow:
            ss << "stack underflow";
            break;
        case kHeapOverflow:
            ss << "heap overflow";
            break;
        case kUnknownOpcode:
            ss << "unknown opcode";
            break;
    }

    ss << ": error at loc " << std::hex << std::setw(4) << std::setfill('0')
       << addr;

    return std::runtime_error(ss.str());
}

const std::runtime_error tam::IoError(const char *message) {
    std::stringstream ss;
    ss << "error: IO error: " << message;
    return std::runtime_error(ss.str());
}
