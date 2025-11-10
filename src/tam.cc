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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stack>
#include <vector>
#include <format>

#include "tam/error.h"

namespace tam {
TamEmulator::TamEmulator() {
    this->code_store.fill(0);
    this->data_store.fill(0);
    this->registers.fill(0);

    this->registers[HB] = kMaxAddr;
    this->registers[HT] = kMaxAddr;

    this->instream = stdin;
    this->outstream = stdout;
}

TamEmulator::TamEmulator(FILE* instream, FILE* outstream) {
    if (!(instream && outstream)) {
        throw IoError("NULL passed for input or output");
    }

    this->code_store.fill(0);
    this->data_store.fill(0);
    this->registers.fill(0);

    this->registers[HB] = kMaxAddr;
    this->registers[HT] = kMaxAddr;

    this->instream = instream;
    this->outstream = outstream;
}

void TamEmulator::LoadProgram(const std::vector<TamCode>& program) {
    if (program.size() > kMemSize) throw IoError("program file too large");

    this->code_store.fill(0);
    std::copy(program.begin(), program.end(), this->code_store.begin());
    this->registers[CT] = program.size();
    this->registers[PB] = program.size();
    this->registers[PT] = this->registers[PB] + 29;
}

TamInstruction TamEmulator::FetchDecode() {
    TamAddr addr = this->registers[CP]++;
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

void TamEmulator::PushData(TamData value) {
    TamAddr addr = this->registers[ST];
    if (addr >= this->registers[HT])
        throw RuntimeError(kStackOverflow, this->registers[CP] - 1);

    this->data_store[addr] = value;
    this->registers[ST]++;
    assert(this->data_store[addr] == value);
}

TamData TamEmulator::PopData() {
    TamAddr addr = this->registers[ST];
    if (this->registers[ST] == 0)
        throw RuntimeError(kStackUnderflow, this->registers[CP] - 1);

    this->registers[ST]--;
    return this->data_store[this->registers[ST]];
}

bool TamEmulator::Execute(TamInstruction instr) {
    switch (instr.op) {
        case LOAD:
            this->ExecuteLoad(instr);
            break;
        case LOADA:
            this->ExecuteLoada(instr);
            break;
        case LOADI:
            this->ExecuteLoadi(instr);
            break;
        case LOADL:
            this->ExecuteLoadl(instr);
            break;
        case STORE:
            this->ExecuteStore(instr);
            break;
        case STOREI:
            this->ExecuteStorei(instr);
            break;
        case CALL:
            if (instr.r == PB && instr.d > 0 && instr.d < 29) {
                this->ExecuteCallPrimitive(instr);
            } else {
                this->ExecuteCall(instr);
            }
            break;
        case CALLI:
            this->ExecuteCalli(instr);
            break;
        case RETURN:
            this->ExecuteReturn(instr);
            break;
        case PUSH:
            this->ExecutePush(instr);
            break;
        case POP:
            this->ExecutePop(instr);
            break;
        case JUMP:
            this->ExecuteJump(instr);
            break;
        case JUMPI:
            this->ExecuteJumpi(instr);
            break;
        case JUMPIF:
            this->ExecuteJumpif(instr);
            break;
        case HALT:
            return false;
        default:
            throw RuntimeError(kUnknownOpcode, this->registers[CP] - 1);
    }
    return true;
}

const std::string TamEmulator::GetSnapshot(TamInstruction instr) const {
    std::stringstream ss;

    ss << std::endl << this->registers[CP] - 1 << ": " << GetMnemonic(instr) << std::endl;

    ss << std::hex << std::setfill('0');

    ss << "Stack";
    for (int I = 0; I < this->registers[ST]; ++I) {
        if (I % 8 == 0) ss << std::endl;
        ss << std::setw(4) << this->data_store[I] << " ";
    }
    ss << std::endl;

    for (auto Block : this->allocated_blocks) {
        ss << "Heap " << std::setw(4) << Block.first;
        for (int I = 0; I < Block.second; ++I) {
            if (I % 8 == 0) {
                ss << std::endl;
            }
            ss << std::setw(4) << this->data_store[Block.first + I] << " ";
        }
    }

    return ss.str();
}

void TamEmulator::ExecuteLoad(TamInstruction instr) {
    TamAddr base_addr = this->registers[instr.r] + instr.d;

    for (int I = 0; I < instr.n; ++I) {
        TamAddr addr = base_addr + I;
        if (addr >= this->registers[ST] && addr <= this->registers[HT])
            throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

        TamData value = this->data_store[addr];
        this->PushData(value);
    }
}

void TamEmulator::ExecuteLoada(TamInstruction instr) {
    TamAddr addr = this->registers[instr.r] + instr.d;
    this->PushData(addr);
}

void TamEmulator::ExecuteLoadi(TamInstruction instr) {
    TamAddr base_addr = this->PopData();

    for (int I = 0; I < instr.n; ++I) {
        TamAddr addr = base_addr + I;
        if (addr >= this->registers[ST] && addr <= this->registers[HT])
            throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

        TamData value = this->data_store[addr];
        this->PushData(value);
    }
}

void TamEmulator::ExecuteLoadl(TamInstruction instr) {
    this->PushData(instr.d);
}

void TamEmulator::ExecuteStore(TamInstruction instr) {
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

    assert(Data.empty());
}

void TamEmulator::ExecuteStorei(TamInstruction instr) {
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

    assert(Data.empty());
}

void TamEmulator::ExecuteCall(TamInstruction instr) {
    if (this->registers[instr.r] + instr.d >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    TamAddr static_link = this->registers[instr.n];
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

void TamEmulator::ExecuteCalli(TamInstruction instr) {
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

void TamEmulator::ExecuteReturn(TamInstruction instr) {
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

void TamEmulator::ExecutePush(TamInstruction instr) {
    if (this->registers[ST] + instr.d >= this->registers[HT])
        throw RuntimeError(kStackOverflow, this->registers[CT] - 1);

    this->registers[ST] += instr.d;
}

void TamEmulator::ExecutePop(TamInstruction instr) {
    std::stack<TamData> data;
    for (int i = 0; i < instr.n; ++i) data.push(this->PopData());

    for (int i = 0; i < instr.d; ++i) this->PopData();

    while (!data.empty()) {
        this->PushData(data.top());
        data.pop();
    }

    assert(data.empty());
}

void TamEmulator::ExecuteJump(TamInstruction instr) {
    TamAddr addr = this->registers[instr.r] + instr.d;
    if (addr >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    this->registers[CP] = addr;
    assert(this->registers[CP] == addr);
}

void TamEmulator::ExecuteJumpi(TamInstruction instr) {
    TamAddr addr = this->PopData();
    if (addr >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    this->registers[CP] = addr;
    assert(this->registers[CP] == addr);
}

void TamEmulator::ExecuteJumpif(TamInstruction instr) {
    TamData value = this->PopData();
    if (value != instr.n) return;

    assert(value == instr.n);

    TamAddr addr = this->registers[instr.r] + instr.d;
    if (addr >= this->registers[CT])
        throw RuntimeError(kCodeAccessViolation, this->registers[CP] - 1);

    this->registers[CP] = addr;
    assert(this->registers[CP] == addr);
}

std::string TamEmulator::GetMnemonic(TamInstruction instr) {
    switch (instr.op) {
        case LOAD:
        case STORE:
        case CALL:
        case JUMPIF:
            // CALL put
            if (instr.op == CALL && instr.r == PB && instr.d > 0 && instr.d < 29)
                return std::format("CALL {}", primitives_names[instr.d]);

            // OPCODE(n) d[r]
            return std::format("{}({}) {}[{}]", OpCodeName(instr.op), instr.n, instr.d, RegisterName(instr.r));

        case LOADA:
        case JUMP:
            // OPCODE d[r]
            return std::format("{} {}[{}]", OpCodeName(instr.op), instr.d, RegisterName(instr.r));

        case RETURN:
        case POP:
            // OPCODE(n) d
            return std::format("{}({}) {}", OpCodeName(instr.op), instr.n, instr.d);

        case LOADI:
        case STOREI:
            // OPCODE (n)
            return std::format("{} {}", OpCodeName(instr.op), instr.n);

        case LOADL:
        case PUSH:
            // OPCODE d
            return std::format("{} {}", OpCodeName(instr.op), instr.d);

        case CALLI:
        case JUMPI:
        case HALT:
            // OPCODE
            return OpCodeName(instr.op);

        default:
            // Invalid
            return "INVALID";
    }
}

constexpr const char* TamEmulator::OpCodeName(uint8_t op) {
    switch (op) {
        case LOAD:   return "LOAD";
        case LOADA:  return "LOADA";
        case LOADI:  return "LOADI";
        case LOADL:  return "LOADL";
        case STORE:  return "STORE";
        case STOREI: return "STOREI";
        case CALL:   return "CALL";
        case CALLI:  return "CALLI";
        case RETURN: return "RETURN";
        case PUSH:   return "PUSH";
        case POP:    return "POP";
        case JUMP:   return "JUMP";
        case JUMPI:  return "JUMPI";
        case JUMPIF: return "JUMPIF";
        case HALT:   return "HALT";
        default:     return "UNKNOWN";
    }
}

constexpr const char* TamEmulator::RegisterName(uint16_t r) {
    switch (r) {
        case CB: return "CB";
        case CT: return "CT";
        case PB: return "PB";
        case PT: return "PT";
        case SB: return "SB";
        case ST: return "ST";
        case HB: return "HB";
        case HT: return "HT";
        case LB: return "LB";
        case L1: return "L1";
        case L2: return "L2";
        case L3: return "L3";
        case L4: return "L4";
        case L5: return "L5";
        case L6: return "L6";
        case CP: return "CP";
        default: return "UNKNOWN";
    }
}

}  // namespace tam
