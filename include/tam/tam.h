#ifndef TAM_TAM_H__
#define TAM_TAM_H__

#include <cstdint>
#include <vector>

namespace tam {

struct TamInstruction {
    uint8_t Op, R, N;
    int16_t D;
};

class TamEmulator {
  private:
    uint32_t CodeStore[65536];
    int16_t DataStore[65536];
    uint16_t Registers[16];

    void pushData(int16_t Value);
    int16_t popData();

    void executeLoad(TamInstruction Instr);
    void executeLoada(TamInstruction Instr);
    void executeLoadi(TamInstruction Instr);
    void executeLoadl(TamInstruction Instr);

  public:
    void loadProgram(std::vector<uint32_t> &Program);
    TamInstruction fetchDecode();
    bool execute(TamInstruction Instr);
};

} // namespace tam

#endif // TAM_TAM_H__
