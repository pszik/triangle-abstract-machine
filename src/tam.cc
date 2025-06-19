#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stack>
#include <tam/error.h>
#include <tam/tam.h>
#include <vector>

namespace tam {

/// Index of code top register
const uint8_t CT = 1;
/// Index of primitive base register
const uint8_t PB = 2;
/// Index of primitive top register
const uint8_t PT = 3;
/// Index of stack base register
const uint8_t SB = 4;
/// Index of stack top register
const uint8_t ST = 5;
/// Index of heap top register
const uint8_t HT = 7;
/// Index of local base register
const uint8_t LB = 8;
/// Index of code pointer register
const uint8_t CP = 15;

void TamEmulator::loadProgram(std::vector<TamCode> &Program) {
    this->CodeStore.fill(0);
    std::copy(Program.begin(), Program.end(), this->CodeStore.begin());
    this->Registers[CT] = Program.size();
    this->Registers[PB] = Program.size();
    this->Registers[PT] = this->Registers[PB] + 29;
}

TamInstruction TamEmulator::fetchDecode() {
    TamAddr Addr = this->Registers[CP];
    if (Addr >= this->Registers[CT]) {
        throw TamException(EK_CodeAccessViolation, Addr);
    }

    TamCode Code = this->CodeStore[Addr];
    uint8_t Op = (Code & 0xf0000000) >> 28;
    uint8_t R = (Code & 0x0f000000) >> 24;
    uint8_t N = (Code & 0x00ff0000) >> 16;
    int16_t D = Code & 0x0000ffff;
    return TamInstruction{Op, R, N, D};
}

inline void TamEmulator::pushData(TamData Value) {
    TamAddr Addr = this->Registers[ST];
    if (Addr >= this->Registers[HT]) {
        throw TamException(EK_StackOverflow, this->Registers[CP] - 1);
    }

    this->DataStore[Addr] = Value;
    this->Registers[ST]++;
}

inline TamData TamEmulator::popData() {
    TamAddr Addr = this->Registers[ST];
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
    case 2: // LOADI
        this->executeLoadi(Instr);
        break;
    case 3: // LOADL
        this->executeLoadl(Instr);
        break;
    case 6: // CALL
        if (Instr.R == PB && Instr.D > 0 && Instr.D < 29) {
            this->executeCallPrimitive(Instr);
        } else {
            this->executeCall(Instr);
        }
        break;
    case 15: // HALT
        return false;
    default:
        throw TamException(EK_UnknownOpcode, this->Registers[CP] - 1);
    }
    return true;
}

void TamEmulator::executeLoad(TamInstruction Instr) {
    TamAddr Addr = this->Registers[Instr.R] + Instr.D;

    for (int I = 0; I < Instr.N; ++I) {
        TamData Value = this->DataStore[Addr + I];
        if (Value >= this->Registers[ST] && Value <= this->Registers[HT]) {
            throw TamException(EK_DataAccessViolation, this->Registers[CP] - 1);
        }

        this->pushData(Value);
    }
}

void TamEmulator::executeLoada(TamInstruction Instr) {
    TamAddr Addr = this->Registers[Instr.R] + Instr.D;
    this->pushData(Addr);
}

void TamEmulator::executeLoadi(TamInstruction Instr) {
    TamAddr Addr = this->popData();

    for (int I = 0; I < Instr.N; ++I) {
        TamData Value = this->DataStore[Addr + I];
        if (Value >= this->Registers[ST] && Value <= this->Registers[HT]) {
            throw TamException(EK_DataAccessViolation, this->Registers[CP] - 1);
        }

        this->pushData(Value);
    }
}

void TamEmulator::executeLoadl(TamInstruction Instr) {
    this->pushData(Instr.D);
}

void TamEmulator::executeStore(TamInstruction Instr) {
    std::stack<TamData> Data;
    for (int I = 0; I < Instr.N; ++I) {
        Data.push(this->popData());
    }

    TamAddr BaseAddr = this->Registers[Instr.R] + Instr.D;
    for (int I = 0; I < Instr.N; ++I) {
        TamAddr Addr = BaseAddr + I;
        if (Addr >= this->Registers[ST] && Addr <= this->Registers[HT]) {
            throw TamException(EK_DataAccessViolation, this->Registers[CP] - 1);
        }
        this->DataStore[Addr] = Data.top();
        Data.pop();
    }

    assert(Data.empty());
}

void TamEmulator::executeStorei(TamInstruction Instr) {
    TamAddr BaseAddr = this->popData();

    std::stack<TamData> Data;
    for (int I = 0; I < Instr.N; ++I) {
        Data.push(this->popData());
    }

    for (int I = 0; I < Instr.N; ++I) {
        TamAddr Addr = BaseAddr + I;
        if (Addr >= this->Registers[ST] && Addr <= this->Registers[HT]) {
            throw TamException(EK_DataAccessViolation, this->Registers[CP] - 1);
        }
        this->DataStore[Addr] = Data.top();
        Data.pop();
    }

    assert(Data.empty());
}

void TamEmulator::executeCall(TamInstruction Instr) {
    if (this->Registers[Instr.R] + Instr.D >= this->Registers[CT]) {
        throw TamException(EK_CodeAccessViolation, this->Registers[CP] - 1);
    }

    TamAddr StaticLink = this->Registers[SB];
    TamAddr DynamicLink = this->Registers[LB];
    TamAddr ReturnAddr = this->Registers[CP];

    this->pushData(StaticLink);
    this->pushData(DynamicLink);
    this->pushData(ReturnAddr);

    this->Registers[LB] = this->Registers[ST] - 3;
    this->Registers[CP] = this->Registers[Instr.R] + Instr.D;
}

} // namespace tam
