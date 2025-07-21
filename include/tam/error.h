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

#ifndef TAM_ERROR_H__
#define TAM_ERROR_H__

#include <cstdint>
#include <stdexcept>

namespace tam {

enum ExceptionKind : uint8_t {
    EK_CodeAccessViolation,
    EK_DataAccessViolation,
    EK_StackUnderflow,
    EK_StackOverflow,
    EK_HeapOverflow,
    EK_UnknownOpcode,
    EK_IOError,
};

const std::runtime_error runtimeError(ExceptionKind Kind, uint16_t Addr);

} // namespace tam
#endif // TAM_ERROR_H__
