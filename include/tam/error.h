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
};

class TamException : std::exception {
  private:
    const ExceptionKind Kind;
    const uint16_t Addr;

  public:
    TamException(ExceptionKind Kind, uint16_t Addr) : Kind(Kind), Addr(Addr) {}

    /// Returns a human readable string describing this exception.
    std::string str() const {
        std::stringstream ss;
        switch (this->Kind) {
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
        }

        ss << " at loc " << this->Kind;
        return ss.str();
    }
};

} // namespace tam
#endif // TAM_ERROR_H__
