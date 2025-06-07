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

  public:
    TamEmulator() {
        this->CodeStore.fill(0);
        this->DataStore.fill(0);
        this->Registers.fill(0);
    }

    void loadProgram(std::vector<TamCode> &Program);
    TamInstruction fetchDecode();
    bool execute(TamInstruction Instr);
};

} // namespace tam

#endif // TAM_TAM_H__
