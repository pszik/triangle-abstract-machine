#ifndef TAM_TAM_H__
#define TAM_TAM_H__

#include <array>
#include <cstdint>
#include <vector>

namespace tam {

typedef uint32_t TamCode;
typedef int16_t TamData;
typedef uint16_t TamAddr;

struct TamInstruction {
    uint8_t Op, R, N;
    int16_t D;
};

class TamEmulator {
  private:
    std::array<TamCode, 65536> CodeStore;
    std::array<TamData, 65536> DataStore;
    std::array<TamAddr, 16> Registers;

    void pushData(TamData Value);
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

  public:
    TamEmulator() {
        this->CodeStore.fill(0);
        this->DataStore.fill(0);
        this->Registers.fill(0);
    }

    /// Sets the program to be run by this emulator.
    ///
    /// Code words are copied into the code store, and CT, PB, and PT registers
    /// are set.
    void loadProgram(std::vector<TamCode> &Program);

    /// Obtains the next instruction to execute.
    TamInstruction fetchDecode();

    /// Executes the given instruction and returns `false` if execution should
    /// halt.
    bool execute(TamInstruction Instr);
};

} // namespace tam

#endif // TAM_TAM_H__
