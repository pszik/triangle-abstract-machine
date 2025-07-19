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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stack>
#include <tam/error.h>
#include <tam/tam.h>
#include <vector>

namespace tam {

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
    assert(Op <= 0xf);
    uint8_t R = (Code & 0x0f000000) >> 24;
    assert(R <= 0xf);
    uint8_t N = (Code & 0x00ff0000) >> 16;
    assert(N <= 0xff);
    int16_t D = Code & 0x0000ffff;
    return TamInstruction{Op, R, N, D};
}

void TamEmulator::pushData(TamData Value) {
    TamAddr Addr = this->Registers[ST];
    if (Addr >= this->Registers[HT]) {
        throw TamException(EK_StackOverflow, this->Registers[CP] - 1);
    }

    this->DataStore[Addr] = Value;
    this->Registers[ST]++;
    assert(this->DataStore[Addr] == Value);
}

TamData TamEmulator::popData() {
    TamAddr Addr = this->Registers[ST];
    if (this->Registers[ST] == 0) {
        throw TamException(EK_StackUnderflow, this->Registers[CP] - 1);
    }

    this->Registers[ST]--;
    return this->DataStore[this->Registers[ST]];
}

bool TamEmulator::execute(TamInstruction Instr) {
    assert(Instr.Op <= 15);
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
        throw TamException(EK_UnknownOpcode, this->Registers[CP] - 1);
    }
    return true;
}

void TamEmulator::executeLoad(TamInstruction Instr) {
    TamAddr BaseAddr = this->Registers[Instr.R] + Instr.D;

    for (int I = 0; I < Instr.N; ++I) {
        TamAddr Addr = BaseAddr + I;
        if (Addr >= this->Registers[ST] && Addr <= this->Registers[HT]) {
            throw TamException(EK_DataAccessViolation, this->Registers[CP] - 1);
        }

        TamData Value = this->DataStore[Addr];
        this->pushData(Value);
    }
}

void TamEmulator::executeLoada(TamInstruction Instr) {
    TamAddr Addr = this->Registers[Instr.R] + Instr.D;
    this->pushData(Addr);
}

void TamEmulator::executeLoadi(TamInstruction Instr) {
    TamAddr BaseAddr = this->popData();

    for (int I = 0; I < Instr.N; ++I) {
        TamAddr Addr = BaseAddr + I;
        if (Addr >= this->Registers[ST] && Addr <= this->Registers[HT]) {
            throw TamException(EK_DataAccessViolation, this->Registers[CP] - 1);
        }

        TamData Value = this->DataStore[Addr];
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

void TamEmulator::executeCalli(TamInstruction Instr) {
    TamAddr CallAddress = this->popData();
    TamAddr StaticLink = this->popData();
    assert(StaticLink < this->Registers[ST]);

    if (CallAddress >= this->Registers[CT]) {
        throw TamException(EK_CodeAccessViolation, this->Registers[CP] - 1);
    }

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

void TamEmulator::executeReturn(TamInstruction Instr) {
    std::stack<TamData> ReturnVal;
    for (int I = 0; I < Instr.N; ++I) {
        ReturnVal.push(this->popData());
    }

    TamAddr DynamicLink = this->DataStore[this->Registers[LB] + 1];
    TamAddr ReturnAddr = this->DataStore[this->Registers[LB] + 2];
    if (ReturnAddr >= this->Registers[CT]) {
        throw new TamException(EK_CodeAccessViolation, this->Registers[CP] - 1);
    }

    // pop stack frame
    while (this->Registers[ST] > this->Registers[LB]) {
        this->popData();
    }

    // pop arguments
    for (int I = 0; I < Instr.D; ++I) {
        this->popData();
    }

    // push result
    for (int I = 0; I < Instr.N; ++I) {
        this->pushData(ReturnVal.top());
        ReturnVal.pop();
    }
    assert(ReturnVal.empty());

    this->Registers[LB] = DynamicLink;
    assert(this->Registers[LB] == DynamicLink);
    this->Registers[CP] = ReturnAddr;
    assert(this->Registers[CP] == ReturnAddr);
}

void TamEmulator::executePush(TamInstruction Instr) {
    if (this->Registers[ST] + Instr.D >= this->Registers[HT]) {
        throw new TamException(EK_StackOverflow, this->Registers[CT] - 1);
    }
    this->Registers[ST] += Instr.D;
}

void TamEmulator::executePop(TamInstruction Instr) {
    std::stack<TamData> Data;
    for (int I = 0; I < Instr.N; ++I) {
        Data.push(this->popData());
    }

    for (int I = 0; I < Instr.D; ++I) {
        this->popData();
    }

    while (!Data.empty()) {
        this->pushData(Data.top());
        Data.pop();
    }

    assert(Data.empty());
}

void TamEmulator::executeJump(TamInstruction Instr) {
    TamAddr Addr = this->Registers[Instr.R] + Instr.D;
    if (Addr >= this->Registers[CT]) {
        throw TamException(EK_CodeAccessViolation, this->Registers[CP] - 1);
    }

    this->Registers[CP] = Addr;
    assert(this->Registers[CP] == Addr);
}

void TamEmulator::executeJumpi(TamInstruction Instr) {
    TamAddr Addr = this->popData();
    if (Addr >= this->Registers[CT]) {
        throw TamException(EK_CodeAccessViolation, this->Registers[CP] - 1);
    }

    this->Registers[CP] = Addr;
    assert(this->Registers[CP] == Addr);
}

void TamEmulator::executeJumpif(TamInstruction Instr) {
    TamData Value = this->popData();
    if (Value != Instr.N) {
        return;
    }
    assert(Value == Instr.N);

    TamAddr Addr = this->Registers[Instr.R] + Instr.D;
    if (Addr >= this->Registers[CT]) {
        throw TamException(EK_CodeAccessViolation, this->Registers[CP] - 1);
    }

    this->Registers[CP] = Addr;
    assert(this->Registers[CP] == Addr);
}
} // namespace tam
