#include <tam/tam.h>

#include <cassert>
#include <iostream>

namespace tam {

void TamEmulator::executeCallPrimitive(TamInstruction Instr) {
    assert(Instr.D > 0 && Instr.D < 29);
    switch (Instr.D) {}
}

void TamEmulator::primitiveGet() {
    char C = std::cin.get();
    this->pushData(C);
}

void TamEmulator::primitivePut() {
    char C = this->popData();
    std::cout << C;
}

void TamEmulator::primitiveGetint() {
    TamData N;
    std::cin >> N;
    this->pushData(N);
}

void TamEmulator::primitivePutint() {
    TamData N = this->popData();
    std::cout << N;
}

} // namespace tam
