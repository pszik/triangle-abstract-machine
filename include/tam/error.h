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
/// @file error.h
/// This file defines the `runtimeError` function for creating TAM-related
/// runtime errors.
//
//===-----------------------------------------------------------------------===//

#ifndef TAM_ERROR_H__
#define TAM_ERROR_H__

#include <cstdint>
#include <stdexcept>

namespace tam {

/// Enumerates all possible kinds of runtime error.
///
enum ExceptionKind : uint8_t {
    EK_CodeAccessViolation, ///< Attempt to access out-of-bounds address in code
                            ///< memory
    EK_DataAccessViolation, ///< Attempt to access out-of-bounds address in data
                            ///< memory
    EK_StackUnderflow,      ///< Stack attempted to grow into the heap
    EK_StackOverflow,       ///< Stack attempted to shrink past 0
    EK_HeapOverflow,        ///< Heap attempted to grow into the stack
    EK_UnknownOpcode,       ///< An unrecognised opcode was given to execute
};

/// Construct a runtime error.
///
/// The error message will report the kind of error and the value of the code
/// pointer when the error occurred.
///
/// @param Kind the kind of error to construct
/// @param Addr address of the instruction that caused the error
const std::runtime_error runtimeError(ExceptionKind Kind, uint16_t Addr);

/// Construct a runtime error relating to an IO problem.
///
/// This function direcly constructs a runtime error using the provided
/// message.
///
/// @param Message specific cause of the error
const std::runtime_error ioError(const char *Message);

} // namespace tam
#endif // TAM_ERROR_H__
