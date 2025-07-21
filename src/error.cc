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
