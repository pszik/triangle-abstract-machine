#include <algorithm>
#include <cstdint>
#include <iterator>
#include <tam/error.h>
#include <tam/tam.h>
#include <vector>

namespace tam {

const uint8_t CT = 1;
const uint8_t ST = 4;
const uint8_t HT = 6;
const uint8_t CP = 15;

void TamEmulator::loadProgram(std::vector<uint32_t> &Program) {
    std::copy(Program.begin(), Program.end(), std::begin(this->CodeStore));
}

TamInstruction TamEmulator::fetchDecode() {
    uint16_t Addr = this->Registers[CP];
    if (Addr >= this->Registers[CT]) {
        throw TamException(EK_CodeAccessViolation, Addr);
    }

    uint32_t Code = this->CodeStore[Addr];
    uint8_t Op = (Code & 0xf0000000) >> 28;
    uint8_t R = (Code & 0x0f000000) >> 24;
    uint8_t N = (Code & 0x00ff0000) >> 16;
    int16_t D = Code & 0x0000ffff;
    return TamInstruction{Op, R, N, D};
}

inline void TamEmulator::pushData(int16_t Value) {
    uint16_t Addr = this->Registers[ST];
    if (Addr >= this->Registers[HT]) {
        throw TamException(EK_StackOverflow, this->Registers[CP] - 1);
    }

    this->DataStore[Addr] = Value;
    this->Registers[ST]++;
}

inline int16_t TamEmulator::popData() {
    uint16_t Addr = this->Registers[ST];
    if (this->Registers[ST] == 0) {
        throw TamException(EK_StackUnderflow, this->Registers[CP] - 1);
    }

    this->Registers[ST]--;
    return this->DataStore[this->Registers[ST]];
}

bool TamEmulator::execute(TamInstruction Instr) {
    switch (Instr.Op) {
    case 0: // LOAD
        this->executeLoad(Instr);
        break;
    case 1: // LOADA
        this->executeLoada(Instr);
        break;
    case 15: // HALT
        return false;
    default:
        throw TamException(EK_UnknownOpcode, this->Registers[CP] - 1);
    }
    return true;
}

void TamEmulator::executeLoad(TamInstruction Instr) {
    uint16_t Addr = this->Registers[Instr.R] + Instr.D;

    for (int I = 0; I < Instr.N; ++I) {
        int16_t Value = this->DataStore[Addr + I];
        if (Value >= this->Registers[ST] && Value <= this->Registers[HT]) {
            throw TamException(EK_DataAccessViolation, this->Registers[CP] - 1);
        }

        this->pushData(Value);
    }
}

void TamEmulator::executeLoada(TamInstruction Instr) {
    uint16_t Addr = this->Registers[Instr.R] + Instr.D;
    this->pushData(Addr);
}

void TamEmulator::executeLoadi(TamInstruction Instr) {
    uint16_t Addr = this->popData();

    for (int I = 0; I < Instr.N; ++I) {
        int16_t Value = this->DataStore[Addr + I];
        if (Value >= this->Registers[ST] && Value <= this->Registers[HT]) {
            throw TamException(EK_DataAccessViolation, this->Registers[CP] - 1);
        }

        this->pushData(Value);
    }
}

void TamEmulator::executeLoadl(TamInstruction Instr) {
    this->pushData(Instr.D);
}

} // namespace tam
