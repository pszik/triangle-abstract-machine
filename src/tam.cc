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
/// This file defines all methods of `TamEmulator` except for `Allocate`,
/// `free`, and the methods for executing primitive operations.
//
//===-----------------------------------------------------------------------===//

#include "tam/tam.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stack>
#include <vector>

#include "tam/error.h"

void tam::TamEmulator::LoadProgram(const std::vector<tam::TamCode> &program) {
    if (program.size() > kMemSize) throw tam::IoError("program file too large");

    this->code_store.fill(0);
    std::copy(program.begin(), program.end(), this->code_store.begin());
    this->registers[CT] = program.size();
    this->registers[PB] = program.size();
    this->registers[PT] = this->registers[PB] + 29;
}

const tam::TamInstruction tam::TamEmulator::FetchDecode() {
    TamAddr addr = this->registers[CP];
    if (addr >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, addr);

    TamCode code = this->code_store[addr];
    uint8_t op = (code & 0xf0000000) >> 28;
    assert(op <= 0xf);
    uint8_t r = (code & 0x0f000000) >> 24;
    assert(r <= 0xf);
    uint8_t n = (code & 0x00ff0000) >> 16;
    assert(n <= 0xff);
    int16_t d = code & 0x0000ffff;
    return TamInstruction{op, r, n, d};
}

void tam::TamEmulator::PushData(TamData value) {
    TamAddr addr = this->registers[ST];
    if (addr >= this->registers[HT])
        throw RuntimeError(kStackOverflow, this->registers[CP] - 1);

    this->data_store[addr] = value;
    this->registers[ST]++;
    assert(this->data_store[addr] == value);
}

tam::TamData tam::TamEmulator::PopData() {
    TamAddr addr = this->registers[ST];
    if (this->registers[ST] == 0)
        throw RuntimeError(kStackUnderflow, this->registers[CP] - 1);

    this->registers[ST]--;
    return this->data_store[this->registers[ST]];
}

bool tam::TamEmulator::Execute(const TamInstruction instr) {
    switch (instr.op) {
        case 0:  // LOAD
            this->ExecuteLoad(instr);
            break;
        case 1:  // LOADA
            this->ExecuteLoada(instr);
            break;
        case 2:  // LOADI
            this->ExecuteLoadi(instr);
            break;
        case 3:  // LOADL
            this->ExecuteLoadl(instr);
            break;
        case 4:  // STORE
            this->ExecuteStore(instr);
            break;
        case 5:  // STOREI
            this->ExecuteStorei(instr);
            break;
        case 6:  // CALL
            if (instr.r == PB && instr.d > 0 && instr.d < 29) {
                this->ExecuteCallPrimitive(instr);
            } else {
                this->ExecuteCall(instr);
            }
            break;
        case 7:  // CALLI
            this->ExecuteCalli(instr);
            break;
        case 8:  // RETURN
            this->ExecuteReturn(instr);
            break;
        case 10:  // PUSH
            this->ExecutePush(instr);
            break;
        case 11:  // POP
            this->ExecutePop(instr);
            break;
        case 12:  // JUMP
            this->ExecuteJump(instr);
            break;
        case 13:  // JUMPI
            this->ExecuteJumpi(instr);
            break;
        case 14:  // JUMPIF
            this->ExecuteJumpif(instr);
            break;
        case 15:  // HALT
            return false;
        default:
            throw RuntimeError(kUnknownOpcode, this->registers[CP] - 1);
    }
    return true;
}

const std::string tam::TamEmulator::GetSnapshot() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    ss << "stack";
    for (int I = 0; I < this->registers[ST]; ++I) {
        if (I % 8 == 0) ss << std::endl;
        ss << std::setw(4) << this->data_store[I] << " ";
    }
    ss << std::endl;

    for (auto Block : this->allocated_blocks) {
        ss << "heap " << std::setw(4) << Block.first;
        for (int I = 0; I < Block.second; ++I) {
            if (I % 8 == 0) {
                ss << std::endl;
            }
            ss << std::setw(4) << this->data_store[Block.first + I] << " ";
        }
    }

    return ss.str();
}

void tam::TamEmulator::ExecuteLoad(const TamInstruction instr) {
    TamAddr base_addr = this->registers[instr.r] + instr.d;

    for (int I = 0; I < instr.n; ++I) {
        TamAddr addr = base_addr + I;
        if (addr >= this->registers[ST] && addr <= this->registers[HT])
            throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

        TamData value = this->data_store[addr];
        this->PushData(value);
    }

    this->registers[CP]++;
}

void tam::TamEmulator::ExecuteLoada(const TamInstruction instr) {
    TamAddr addr = this->registers[instr.r] + instr.d;
    this->PushData(addr);

    this->registers[CP]++;
}

void tam::TamEmulator::ExecuteLoadi(const TamInstruction instr) {
    TamAddr base_addr = this->PopData();

    for (int I = 0; I < instr.n; ++I) {
        TamAddr addr = base_addr + I;
        if (addr >= this->registers[ST] && addr <= this->registers[HT])
            throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

        TamData value = this->data_store[addr];
        this->PushData(value);
    }

    this->registers[CP]++;
}

void tam::TamEmulator::ExecuteLoadl(const TamInstruction instr) {
    this->PushData(instr.d);

    this->registers[CP]++;
}

void tam::TamEmulator::ExecuteStore(const TamInstruction instr) {
    std::stack<TamData> Data;
    for (int I = 0; I < instr.n; ++I) Data.push(this->PopData());

    TamAddr base_addr = this->registers[instr.r] + instr.d;
    for (int I = 0; I < instr.n; ++I) {
        TamAddr addr = base_addr + I;
        if (addr >= this->registers[ST] && addr <= this->registers[HT])
            throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

        this->data_store[addr] = Data.top();
        Data.pop();
    }

    this->registers[CP]++;
    assert(Data.empty());
}

void tam::TamEmulator::ExecuteStorei(const TamInstruction instr) {
    TamAddr base_addr = this->PopData();

    std::stack<TamData> Data;
    for (int I = 0; I < instr.n; ++I) Data.push(this->PopData());

    for (int I = 0; I < instr.n; ++I) {
        TamAddr addr = base_addr + I;
        if (addr >= this->registers[ST] && addr <= this->registers[HT])
            throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

        this->data_store[addr] = Data.top();
        Data.pop();
    }

    this->registers[CP]++;
    assert(Data.empty());
}

void tam::TamEmulator::ExecuteCall(const TamInstruction instr) {
    if (this->registers[instr.r] + instr.d >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    TamAddr static_link = this->registers[SB];
    assert(static_link < this->registers[ST]);
    TamAddr dynamic_link = this->registers[LB];
    assert(dynamic_link < this->registers[ST]);
    TamAddr return_addr = this->registers[CP];
    assert(return_addr < this->registers[CT]);

    this->PushData(static_link);
    this->PushData(dynamic_link);
    this->PushData(return_addr);

    this->registers[LB] = this->registers[ST] - 3;
    this->registers[CP] = this->registers[instr.r] + instr.d;
}

void tam::TamEmulator::ExecuteCalli(const TamInstruction instr) {
    TamAddr call_address = this->PopData();
    TamAddr static_link = this->PopData();
    assert(static_link < this->registers[ST]);

    if (call_address >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    TamAddr dynamic_link = this->registers[LB];
    assert(dynamic_link < this->registers[ST]);
    TamAddr return_addr = this->registers[CP];
    assert(return_addr < this->registers[CT]);

    this->PushData(static_link);
    this->PushData(dynamic_link);
    this->PushData(return_addr);

    this->registers[LB] = this->registers[ST] - 3;
    this->registers[CP] = call_address;
}

void tam::TamEmulator::ExecuteReturn(const TamInstruction instr) {
    std::stack<TamData> return_val;
    for (int I = 0; I < instr.n; ++I) return_val.push(this->PopData());

    assert(return_val.size() == instr.n);

    TamAddr dynamic_link = this->data_store[this->registers[LB] + 1];
    TamAddr return_addr = this->data_store[this->registers[LB] + 2];
    if (return_addr >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    // pop stack frame
    while (this->registers[ST] > this->registers[LB]) this->PopData();
    assert(this->registers[ST] == this->registers[LB]);

    // pop arguments
    for (int I = 0; I < instr.d; ++I) this->PopData();
    assert(this->registers[ST] == this->registers[LB] - instr.d);

    // push result
    for (int I = 0; I < instr.n; ++I) {
        assert(!return_val.empty());
        this->PushData(return_val.top());
        return_val.pop();
    }
    assert(return_val.empty());

    this->registers[LB] = dynamic_link;
    assert(this->registers[LB] == dynamic_link);
    this->registers[CP] = return_addr;
    assert(this->registers[CP] == return_addr);
}

void tam::TamEmulator::ExecutePush(const TamInstruction instr) {
    if (this->registers[ST] + instr.d >= this->registers[HT])
        throw RuntimeError(kStackOverflow, this->registers[CT] - 1);

    this->registers[ST] += instr.d;

    this->registers[CP]++;
}

void tam::TamEmulator::ExecutePop(const TamInstruction instr) {
    std::stack<TamData> data;
    for (int i = 0; i < instr.n; ++i) data.push(this->PopData());

    for (int i = 0; i < instr.d; ++i) this->PopData();

    while (!data.empty()) {
        this->PushData(data.top());
        data.pop();
    }

    this->registers[CP]++;
    assert(data.empty());
}

void tam::TamEmulator::ExecuteJump(const TamInstruction instr) {
    TamAddr addr = this->registers[instr.r] + instr.d;
    if (addr >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    this->registers[CP] = addr;
    assert(this->registers[CP] == addr);
}

void tam::TamEmulator::ExecuteJumpi(const TamInstruction instr) {
    TamAddr addr = this->PopData();
    if (addr >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    this->registers[CP] = addr;
    assert(this->registers[CP] == addr);
}

void tam::TamEmulator::ExecuteJumpif(const TamInstruction instr) {
    TamData value = this->PopData();
    if (value != instr.n) return;

    assert(value == instr.n);

    TamAddr addr = this->registers[instr.r] + instr.d;
    if (addr >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    this->registers[CP] = addr;
    assert(this->registers[CP] == addr);
}
