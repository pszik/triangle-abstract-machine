#include <tam/tam.h>

#include <cassert>
#include <iostream>

namespace tam {

void TamEmulator::executeCallPrimitive(TamInstruction Instr) {
    assert(Instr.D > 0 && Instr.D < 29);
    switch (Instr.D) {
    case 1:
        break;
    case 2:
        primitiveNot();
        break;
    case 3:
        primitiveAnd();
        break;
    case 4:
        primitiveOr();
        break;
    case 5:
        primitiveSucc();
        break;
    case 6:
        primitivePred();
        break;
    }
}

void TamEmulator::primitiveNot() {
    TamData Value = this->popData();
    this->pushData(-Value);
}

void TamEmulator::primitiveAnd() {
    TamData op1 = this->popData(), op2 = this->popData();
    this->pushData(op1 * op2 == 0 ? 0 : 1);
}

void TamEmulator::primitiveOr() {
    TamData op1 = this->popData(), op2 = this->popData();
    this->pushData(op1 + op2 == 0 && op1 != -op2 ? 0 : 1);
}

void TamEmulator::primitiveSucc() {
    TamData op = this->popData();
    this->pushData(op + 1);
}

void TamEmulator::primitivePred() {
    TamData op = this->popData();
    this->pushData(op - 1);
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
