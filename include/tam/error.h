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
#include <exception>
#include <sstream>
#include <string>

namespace tam {

enum ExceptionKind : uint8_t {
    EK_CodeAccessViolation,
    EK_DataAccessViolation,
    EK_StackUnderflow,
    EK_StackOverflow,
    EK_UnknownOpcode,
    EK_IOError,
};

class TamException : std::exception {
  private:
    std::string Message;

  public:
    TamException(ExceptionKind Kind, uint16_t Addr) : std::exception() {
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
        case EK_UnknownOpcode:
            ss << "unknown opcode";
            break;
        case EK_IOError:
            ss << "IO error";
            break;
        }

        ss << " at loc " << Addr;
        this->Message = ss.str();
    }

    const char *what() const throw() override { return this->Message.c_str(); }
};

} // namespace tam
#endif // TAM_ERROR_H__
