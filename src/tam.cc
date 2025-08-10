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
/// @file tam.cc
/// This file defines all methods of `TamEmulator` except for `allocate`,
/// `free`, and the methods for executing primitive operations.
//
//===-----------------------------------------------------------------------===//

#include <tam/tam.h>

#include <tam/error.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stack>
#include <vector>

using namespace tam;

void TamEmulator::loadProgram(const std::vector<TamCode> &Program) {
    if (Program.size() > MEM_SIZE)
        throw tam::ioError("program file too large");

    this->CodeStore.fill(0);
    std::copy(Program.begin(), Program.end(), this->CodeStore.begin());
    this->Registers[CT] = Program.size();
    this->Registers[PB] = Program.size();
    this->Registers[PT] = this->Registers[PB] + 29;
}

const TamInstruction tam::TamEmulator::fetchDecode() {
    TamAddr Addr = this->Registers[CP];
    if (Addr >= this->Registers[CT])
        throw runtimeError(EK_CodeAccessViolation, Addr);

    TamCode Code = this->CodeStore[Addr];
    uint8_t Op = (Code & 0xf0000000) >> 28;
    assert(Op <= 0xf);
    uint8_t R = (Code & 0x0f000000) >> 24;
    assert(R <= 0xf);
    uint8_t N = (Code & 0x00ff0000) >> 16;
    assert(N <= 0xff);
    int16_t D = Code & 0x0000ffff;
    return TamInstruction{Op, R, N, D};
}

void tam::TamEmulator::pushData(TamData Value) {
    TamAddr Addr = this->Registers[ST];
    if (Addr >= this->Registers[HT])
        throw runtimeError(EK_StackOverflow, this->Registers[CP] - 1);

    this->DataStore[Addr] = Value;
    this->Registers[ST]++;
    assert(this->DataStore[Addr] == Value);
}

TamData tam::TamEmulator::popData() {
    TamAddr Addr = this->Registers[ST];
    if (this->Registers[ST] == 0)
        throw runtimeError(EK_StackUnderflow, this->Registers[CP] - 1);

    this->Registers[ST]--;
    return this->DataStore[this->Registers[ST]];
}

bool tam::TamEmulator::execute(const TamInstruction Instr) {
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
    case 4: // STORE
        this->executeStore(Instr);
        break;
    case 5: // STOREI
        this->executeStorei(Instr);
        break;
    case 6: // CALL
        if (Instr.R == PB && Instr.D > 0 && Instr.D < 29) {
            this->executeCallPrimitive(Instr);
        } else {
            this->executeCall(Instr);
        }
        break;
    case 7: // CALLI
        this->executeCalli(Instr);
        break;
    case 8: // RETURN
        this->executeReturn(Instr);
        break;
    case 10: // PUSH
        this->executePush(Instr);
        break;
    case 11: // POP
        this->executePop(Instr);
        break;
    case 12: // JUMP
        this->executeJump(Instr);
        break;
    case 13: // JUMPI
        this->executeJumpi(Instr);
        break;
    case 14: // JUMPIF
        this->executeJumpif(Instr);
        break;
    case 15: // HALT
        return false;
    default:
        throw runtimeError(EK_UnknownOpcode, this->Registers[CP] - 1);
    }
    return true;
}

const std::string tam::TamEmulator::getSnapshot() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    ss << "stack";
    for (int I = 0; I < this->Registers[ST]; ++I) {
        if (I % 8 == 0)
            ss << std::endl;
        ss << std::setw(4) << this->DataStore[I] << " ";
    }
    ss << std::endl;

    for (auto Block : this->AllocatedBlocks) {
        ss << "heap " << std::setw(4) << Block.first;
        for (int I = 0; I < Block.second; ++I) {
            if (I % 8 == 0) {
                ss << std::endl;
            }
            ss << std::setw(4) << this->DataStore[Block.first + I] << " ";
        }
    }

    return ss.str();
}

void tam::TamEmulator::executeLoad(const TamInstruction Instr) {
    TamAddr BaseAddr = this->Registers[Instr.R] + Instr.D;

    for (int I = 0; I < Instr.N; ++I) {
        TamAddr Addr = BaseAddr + I;
        if (Addr >= this->Registers[ST] && Addr <= this->Registers[HT])
            throw runtimeError(EK_DataAccessViolation, this->Registers[CP] - 1);

        TamData Value = this->DataStore[Addr];
        this->pushData(Value);
    }
}

void tam::TamEmulator::executeLoada(const TamInstruction Instr) {
    TamAddr Addr = this->Registers[Instr.R] + Instr.D;
    this->pushData(Addr);
}

void tam::TamEmulator::executeLoadi(const TamInstruction Instr) {
    TamAddr BaseAddr = this->popData();

    for (int I = 0; I < Instr.N; ++I) {
        TamAddr Addr = BaseAddr + I;
        if (Addr >= this->Registers[ST] && Addr <= this->Registers[HT])
            throw runtimeError(EK_DataAccessViolation, this->Registers[CP] - 1);

        TamData Value = this->DataStore[Addr];
        this->pushData(Value);
    }
}

void tam::TamEmulator::executeLoadl(const TamInstruction Instr) {
    this->pushData(Instr.D);
}

void tam::TamEmulator::executeStore(const TamInstruction Instr) {
    std::stack<TamData> Data;
    for (int I = 0; I < Instr.N; ++I)
        Data.push(this->popData());

    TamAddr BaseAddr = this->Registers[Instr.R] + Instr.D;
    for (int I = 0; I < Instr.N; ++I) {
        TamAddr Addr = BaseAddr + I;
        if (Addr >= this->Registers[ST] && Addr <= this->Registers[HT])
            throw runtimeError(EK_DataAccessViolation, this->Registers[CP] - 1);

        this->DataStore[Addr] = Data.top();
        Data.pop();
    }

    assert(Data.empty());
}

void tam::TamEmulator::executeStorei(const TamInstruction Instr) {
    TamAddr BaseAddr = this->popData();

    std::stack<TamData> Data;
    for (int I = 0; I < Instr.N; ++I)
        Data.push(this->popData());

    for (int I = 0; I < Instr.N; ++I) {
        TamAddr Addr = BaseAddr + I;
        if (Addr >= this->Registers[ST] && Addr <= this->Registers[HT])
            throw runtimeError(EK_DataAccessViolation, this->Registers[CP] - 1);

        this->DataStore[Addr] = Data.top();
        Data.pop();
    }

    assert(Data.empty());
}

void tam::TamEmulator::executeCall(const TamInstruction Instr) {
    if (this->Registers[Instr.R] + Instr.D >= this->Registers[CT])
        throw runtimeError(EK_CodeAccessViolation, this->Registers[CP] - 1);

    TamAddr StaticLink = this->Registers[SB];
    assert(StaticLink < this->Registers[ST]);
    TamAddr DynamicLink = this->Registers[LB];
    assert(DynamicLink < this->Registers[ST]);
    TamAddr ReturnAddr = this->Registers[CP];
    assert(ReturnAddr < this->Registers[CT]);

    this->pushData(StaticLink);
    this->pushData(DynamicLink);
    this->pushData(ReturnAddr);

    this->Registers[LB] = this->Registers[ST] - 3;
    this->Registers[CP] = this->Registers[Instr.R] + Instr.D;
}

void tam::TamEmulator::executeCalli(const TamInstruction Instr) {
    TamAddr CallAddress = this->popData();
    TamAddr StaticLink = this->popData();
    assert(StaticLink < this->Registers[ST]);

    if (CallAddress >= this->Registers[CT])
        throw runtimeError(EK_CodeAccessViolation, this->Registers[CP] - 1);

    TamAddr DynamicLink = this->Registers[LB];
    assert(DynamicLink < this->Registers[ST]);
    TamAddr ReturnAddr = this->Registers[CP];
    assert(ReturnAddr < this->Registers[CT]);

    this->pushData(StaticLink);
    this->pushData(DynamicLink);
    this->pushData(ReturnAddr);

    this->Registers[LB] = this->Registers[ST] - 3;
    this->Registers[CP] = CallAddress;
}

void tam::TamEmulator::executeReturn(const TamInstruction Instr) {
    std::stack<TamData> ReturnVal;
    for (int I = 0; I < Instr.N; ++I)
        ReturnVal.push(this->popData());

    assert(ReturnVal.size() == Instr.N);

    TamAddr DynamicLink = this->DataStore[this->Registers[LB] + 1];
    TamAddr ReturnAddr = this->DataStore[this->Registers[LB] + 2];
    if (ReturnAddr >= this->Registers[CT])
        throw runtimeError(EK_CodeAccessViolation, this->Registers[CP] - 1);

    // pop stack frame
    while (this->Registers[ST] > this->Registers[LB])
        this->popData();
    assert(this->Registers[ST] == this->Registers[LB]);

    // pop arguments
    for (int I = 0; I < Instr.D; ++I)
        this->popData();
    assert(this->Registers[ST] == this->Registers[LB] - Instr.D);

    // push result
    for (int I = 0; I < Instr.N; ++I) {
        assert(!ReturnVal.empty());
        this->pushData(ReturnVal.top());
        ReturnVal.pop();
    }
    assert(ReturnVal.empty());

    this->Registers[LB] = DynamicLink;
    assert(this->Registers[LB] == DynamicLink);
    this->Registers[CP] = ReturnAddr;
    assert(this->Registers[CP] == ReturnAddr);
}

void tam::TamEmulator::executePush(const TamInstruction Instr) {
    if (this->Registers[ST] + Instr.D >= this->Registers[HT])
        throw runtimeError(EK_StackOverflow, this->Registers[CT] - 1);

    this->Registers[ST] += Instr.D;
}

void tam::TamEmulator::executePop(const TamInstruction Instr) {
    std::stack<TamData> Data;
    for (int I = 0; I < Instr.N; ++I)
        Data.push(this->popData());

    for (int I = 0; I < Instr.D; ++I)
        this->popData();

    while (!Data.empty()) {
        this->pushData(Data.top());
        Data.pop();
    }

    assert(Data.empty());
}

void tam::TamEmulator::executeJump(const TamInstruction Instr) {
    TamAddr Addr = this->Registers[Instr.R] + Instr.D;
    if (Addr >= this->Registers[CT])
        throw runtimeError(EK_CodeAccessViolation, this->Registers[CP] - 1);

    this->Registers[CP] = Addr;
    assert(this->Registers[CP] == Addr);
}

void tam::TamEmulator::executeJumpi(const TamInstruction Instr) {
    TamAddr Addr = this->popData();
    if (Addr >= this->Registers[CT])
        throw runtimeError(EK_CodeAccessViolation, this->Registers[CP] - 1);

    this->Registers[CP] = Addr;
    assert(this->Registers[CP] == Addr);
}

void tam::TamEmulator::executeJumpif(const TamInstruction Instr) {
    TamData Value = this->popData();
    if (Value != Instr.N)
        return;

    assert(Value == Instr.N);

    TamAddr Addr = this->Registers[Instr.R] + Instr.D;
    if (Addr >= this->Registers[CT])
        throw runtimeError(EK_CodeAccessViolation, this->Registers[CP] - 1);

    this->Registers[CP] = Addr;
    assert(this->Registers[CP] == Addr);
}
