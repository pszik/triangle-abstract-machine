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

/// @file tam.h
/// Defines the interface for TAM instructions and the emulator itself.

#ifndef TAM_TAM_H__
#define TAM_TAM_H__

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace tam {

typedef uint32_t TamCode;
typedef int16_t TamData;
typedef uint16_t TamAddr;

/// Number of addressable words in memory.
///
const int MEM_SIZE = 65536;
/// Index of highest-addressed word in memory.
///
const int MAX_ADDR = MEM_SIZE - 1;

/// Index of code top register
///
const uint8_t CT = 1;
/// Index of primitive base register
///
const uint8_t PB = 2;
/// Index of primitive top register
///
const uint8_t PT = 3;
/// Index of stack base register
///
const uint8_t SB = 4;
/// Index of stack top register
///
const uint8_t ST = 5;
/// Index of heap base register
///
const uint8_t HB = 6;
/// Index of heap top register
///
const uint8_t HT = 7;
/// Index of local base register
///
const uint8_t LB = 8;
/// Index of code pointer register
///
const uint8_t CP = 15;

/// A single TAM instruction.
///
struct TamInstruction {
    uint8_t Op; ///< Opcode
    uint8_t R;  ///< Register
    uint8_t N;  ///< Unsigned operand
    int16_t D;  ///< Signed operand
};

/// A TAM emulator.
///
class TamEmulator {
  protected:
    std::array<TamCode, MEM_SIZE> CodeStore; ///< Stores code words
    std::array<TamData, MEM_SIZE> DataStore; ///< Stores data words
    std::array<TamAddr, 16> Registers;       ///< Stores register values

    std::map<TamAddr, int>
        AllocatedBlocks, ///< Records blocks of heap memory in use
        FreeBlocks;      ///< Records blocks of unused heap memory

    /// Attempt to allocate some memory on the heap.
    ///
    /// @param N size of requested block
    /// @return address of first word in the block
    TamAddr allocate(int N);

    /// Attempt to free the allocated block beginning at `Addr`.
    ///
    /// @param Addr start address of block
    /// @param Size expected size of block
    void free(TamAddr Addr, TamData Size);

    /// Push a value to the top of the stack.
    ///
    /// This method updates the stack top register.
    /// @param Value value to push
    void pushData(TamData Value);

    /// Remove and return the top value of the stack.
    ///
    /// This method updates the stack top register, but does not actually delete
    /// the value that was stored. It will remain until overwritten by a future
    /// call.
    /// @return the data
    TamData popData();

    void executeLoad(TamInstruction Instr);
    void executeLoada(TamInstruction Instr);
    void executeLoadi(TamInstruction Instr);
    void executeLoadl(TamInstruction Instr);
    void executeStore(TamInstruction Instr);
    void executeStorei(TamInstruction Instr);
    void executeCall(TamInstruction Instr);
    void executeCallPrimitive(TamInstruction Instr);
    void executeCalli(TamInstruction Instr);
    void executeReturn(TamInstruction Instr);
    void executePush(TamInstruction Instr);
    void executePop(TamInstruction Instr);
    void executeJump(TamInstruction Instr);
    void executeJumpi(TamInstruction Instr);
    void executeJumpif(TamInstruction Instr);

    void primitiveNot();
    void primitiveAnd();
    void primitiveOr();
    void primitiveSucc();
    void primitivePred();
    void primitiveNeg();
    void primitiveAdd();
    void primitiveSub();
    void primitiveMult();
    void primitiveDiv();
    void primitiveMod();
    void primitiveLt();
    void primitiveLe();
    void primitiveGe();
    void primitiveGt();
    void primitiveEq();
    void primitiveNe();
    void primitiveEol();
    void primitiveEof();
    void primitiveGet();
    void primitivePut();
    void primitiveGeteol();
    void primitivePuteol();
    void primitiveGetint();
    void primitivePutint();
    void primitiveNew();
    void primitiveDispose();

  public:
    /// Construct a new emulator.
    ///
    /// On creation, all memory is zeroed and registers are set to default
    /// values. Registers default to 0 except for HB and HT, which default to
    /// the highest address.
    TamEmulator() {
        this->CodeStore.fill(0);
        this->DataStore.fill(0);
        this->Registers.fill(0);

        this->Registers[HB] = 65535;
        this->Registers[HT] = 65535;
    }

    /// Sets the program to be run by this emulator.
    ///
    /// Code words are copied into the code store, and CT, PB, and PT registers
    /// are set.
    ///
    /// @param Program program code to load
    void loadProgram(const std::vector<TamCode> &Program);

    /// Obtains the next instruction to execute.
    ///
    /// This method also increments the code pointer register.
    ///
    /// @return the instruction
    TamInstruction fetchDecode();

    /// Executes the given instruction and returns `false` if execution should
    /// halt.
    ///
    /// @param Instr instruction to execute
    bool execute(TamInstruction Instr);

    /// Return a string representing the current contents of the stack and any
    /// allocated blocks on the heap.
    ///
    /// @return the stack and heap contents
    const std::string getSnapshot();
};

} // namespace tam

#endif // TAM_TAM_H__
