#include <iostream>
#include <tam/error.h>
#include <tam/tam.h>

int main() {
    tam::TamEmulator emulator;

    bool Running = true;
    while (Running) {
        try {
            tam::TamInstruction Instr = emulator.fetchDecode();
            Running = emulator.execute(Instr);
        } catch (const tam::TamException &e) {
            std::cerr << e.str() << std::endl;
            break;
        }
    }
}
