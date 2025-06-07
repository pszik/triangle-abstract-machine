#ifndef TAM_TAM_H__
#define TAM_TAM_H__

#include <array>
#include <cstdint>
#include <vector>

namespace tam {

struct TamInstruction {
    uint8_t Op, R, N;
    int16_t D;
};

class TamEmulator {
  private:
    std::array<uint32_t, 65536> CodeStore;
    std::array<int16_t, 65536> DataStore;
    std::array<uint16_t, 16> Registers;

    void pushData(int16_t Value);
    int16_t popData();

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

    void loadProgram(std::vector<uint32_t> &Program);
    TamInstruction fetchDecode();
    bool execute(TamInstruction Instr);
};

} // namespace tam

#endif // TAM_TAM_H__
