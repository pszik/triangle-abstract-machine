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
/// @file tam.h
/// This file declares the core of the TAM interface. It declares the
/// `TamEmulator` class itself, along with the `TamInstruction` class used to
/// pass instructions.
//
//===-----------------------------------------------------------------------===//

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
constexpr const int MEM_SIZE = 65536;
/// Index of highest-addressed word in memory.
///
constexpr const int MAX_ADDR = MEM_SIZE - 1;

/// Index of code top register
///
constexpr const uint8_t CT = 1;
/// Index of primitive base register
///
constexpr const uint8_t PB = 2;
/// Index of primitive top register
///
constexpr const uint8_t PT = 3;
/// Index of stack base register
///
constexpr const uint8_t SB = 4;
/// Index of stack top register
///
constexpr const uint8_t ST = 5;
/// Index of heap base register
///
constexpr const uint8_t HB = 6;
/// Index of heap top register
///
constexpr const uint8_t HT = 7;
/// Index of local base register
///
constexpr const uint8_t LB = 8;
/// Index of code pointer register
///
constexpr const uint8_t CP = 15;

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
/// The emulator class is responsible for simulating all operations that would
/// happen on a physical CPU. Methods are provided for the fetch-decode-execute
/// cycle, and all methods will directly update registers and memory
/// appropriately.
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

    void executeLoad(const TamInstruction Instr);
    void executeLoada(const TamInstruction Instr);
    void executeLoadi(const TamInstruction Instr);
    void executeLoadl(const TamInstruction Instr);
    void executeStore(const TamInstruction Instr);
    void executeStorei(const TamInstruction Instr);
    void executeCall(const TamInstruction Instr);
    void executeCallPrimitive(const TamInstruction Instr);
    void executeCalli(const TamInstruction Instr);
    void executeReturn(const TamInstruction Instr);
    void executePush(const TamInstruction Instr);
    void executePop(const TamInstruction Instr);
    void executeJump(const TamInstruction Instr);
    void executeJumpi(const TamInstruction Instr);
    void executeJumpif(const TamInstruction Instr);

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
    /// values. Registers default to 0 except for `HB`` and `HT`, which default
    /// to the highest address.
    TamEmulator() {
        this->CodeStore.fill(0);
        this->DataStore.fill(0);
        this->Registers.fill(0);

        this->Registers[HB] = MAX_ADDR;
        this->Registers[HT] = MAX_ADDR;
    }

    /// Sets the program to be run by this emulator.
    ///
    /// Code words are copied into the code store, and CT, PB, and PT registers
    /// are set accordingly.
    ///
    /// @param Program program code to load
    /// @throws std::runtime_exception if the provided program is too large to
    /// fit in memory
    void loadProgram(const std::vector<TamCode> &Program);

    /// Obtains the next instruction to execute.
    ///
    /// This method increments the code pointer as part of its operation.
    ///
    /// @return the instruction
    /// @throws std::runtime_exception if the code pointer pointed outside of
    /// allocated code memory
    const TamInstruction fetchDecode();

    /// Executes the given instruction.
    ///
    /// @param Instr instruction to execute
    /// @return `true` if execution should continue, `false` if not
    /// @throws std::runtime_exception if any error occurred during execution
    bool execute(const TamInstruction Instr);

    /// Return a string representing the current contents of the stack and any
    /// allocated blocks on the heap.
    ///
    /// @return the stack and heap contents
    const std::string getSnapshot() const;
};

} // namespace tam

#endif // TAM_TAM_H__
