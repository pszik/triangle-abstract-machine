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
        this->primitiveNot();
        break;
    case 3:
        this->primitiveAnd();
        break;
    case 4:
        this->primitiveOr();
        break;
    case 5:
        this->primitiveSucc();
        break;
    case 6:
        this->primitivePred();
        break;
    case 7:
        this->primitiveNeg();
        break;
    case 8:
        this->primitiveAdd();
        break;
    case 9:
        this->primitiveSub();
        break;
    case 10:
        this->primitiveMult();
        break;
    case 11:
        this->primitiveDiv();
        break;
    case 12:
        this->primitiveMod();
        break;
    case 21:
        this->primitiveGet();
        break;
    case 22:
        this->primitivePut();
        break;
    case 25:
        this->primitiveGetint();
        break;
    case 26:
        this->primitivePutint();
        break;
    }
}

void TamEmulator::primitiveNot() {
    TamData Value = this->popData();
    this->pushData(Value ? 0 : 1);
}

void TamEmulator::primitiveAnd() {
    TamData Op1 = this->popData(), Op2 = this->popData();
    this->pushData(Op1 * Op2 == 0 ? 0 : 1);
}

void TamEmulator::primitiveOr() {
    TamData Op1 = this->popData(), Op2 = this->popData();
    this->pushData(Op1 + Op2 == 0 && Op1 != -Op2 ? 0 : 1);
}

void TamEmulator::primitiveSucc() {
    TamData Op = this->popData();
    this->pushData(Op + 1);
}

void TamEmulator::primitivePred() {
    TamData Op = this->popData();
    this->pushData(Op - 1);
}

void TamEmulator::primitiveNeg() {
    TamData Op = this->popData();
    this->pushData(-Op);
}

void TamEmulator::primitiveAdd() {
    TamData Arg1 = this->popData(), Arg2 = this->popData();
    this->pushData(Arg1 + Arg2);
}

void TamEmulator::primitiveSub() {
    TamData Arg1 = this->popData(), Arg2 = this->popData();
    this->pushData(Arg1 - Arg2);
}
void TamEmulator::primitiveMult() {
    TamData Arg1 = this->popData(), Arg2 = this->popData();
    this->pushData(Arg1 * Arg2);
}

void TamEmulator::primitiveDiv() {
    TamData Arg1 = this->popData(), Arg2 = this->popData();
    this->pushData(Arg1 / Arg2);
}

void TamEmulator::primitiveMod() {
    TamData Arg1 = this->popData(), Arg2 = this->popData();
    this->pushData(Arg1 % Arg2);
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
