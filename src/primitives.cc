#include <stack>
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
    case 13:
        this->primitiveLt();
        break;
    case 14:
        this->primitiveLe();
        break;
    case 15:
        this->primitiveGe();
        break;
    case 16:
        this->primitiveGt();
        break;
    case 17:
        this->primitiveEq();
        break;
    case 18:
        this->primitiveNe();
        break;
    case 19:
        this->primitiveEol();
        break;
    case 20:
        this->primitiveEof();
        break;
    case 21:
        this->primitiveGet();
        break;
    case 22:
        this->primitivePut();
        break;
    case 23:
        this->primitiveGeteol();
        break;
    case 24:
        this->primitivePuteol();
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
    TamData Op2 = this->popData(), Op1 = this->popData();
    this->pushData(Op1 * Op2 ? 1 : 0);
}

void TamEmulator::primitiveOr() {
    TamData Op2 = this->popData(), Op1 = this->popData();
    this->pushData(Op1 + Op2 || Op1 == -Op2 ? 1 : 0);
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
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 + Arg2);
}

void TamEmulator::primitiveSub() {
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 - Arg2);
}
void TamEmulator::primitiveMult() {
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 * Arg2);
}

void TamEmulator::primitiveDiv() {
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 / Arg2);
}

void TamEmulator::primitiveMod() {
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 % Arg2);
}

void TamEmulator::primitiveLt() {
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 < Arg2 ? 1 : 0);
}

void TamEmulator::primitiveLe() {
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 <= Arg2 ? 1 : 0);
}

void TamEmulator::primitiveGe() {
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 >= Arg2 ? 1 : 0);
}

void TamEmulator::primitiveGt() {
    TamData Arg2 = this->popData(), Arg1 = this->popData();
    this->pushData(Arg1 > Arg2 ? 1 : 0);
}

void TamEmulator::primitiveEq() {
    TamData Width = this->popData();
    std::stack<TamData> Arg1, Arg2;

    for (int I = 0; I < Width; ++I) {
        Arg2.push(this->popData());
    }

    for (int I = 0; I < Width; ++I) {
        Arg1.push(this->popData());
    }

    this->pushData(Arg1 == Arg2 ? 1 : 0);
}

void TamEmulator::primitiveNe() {
    TamData Width = this->popData();
    std::stack<TamData> Arg1, Arg2;

    for (int I = 0; I < Width; ++I) {
        Arg2.push(this->popData());
    }

    for (int I = 0; I < Width; ++I) {
        Arg1.push(this->popData());
    }

    this->pushData(Arg1 != Arg2 ? 1 : 0);
}

void TamEmulator::primitiveEol() {
    char C = std::cin.peek();
    this->pushData(C == '\n' ? 1 : 0);
}

void TamEmulator::primitiveEof() { this->pushData(std::cin.eof() ? 1 : 0); }

void TamEmulator::primitiveGet() {
    TamAddr Addr = this->popData();
    char C = std::cin.get();
    this->DataStore[Addr] = C;
}

void TamEmulator::primitivePut() {
    char C = this->popData();
    std::cout << C;
}

void TamEmulator::primitiveGeteol() {
    char C;
    while (C != '\n') {
        std::cin >> C;
    }
}

void TamEmulator::primitivePuteol() { std::cout << std::endl; }

void TamEmulator::primitiveGetint() {
    TamAddr Addr = this->popData();
    TamData N;
    std::cin >> N;
    this->DataStore[Addr] = N;
}

void TamEmulator::primitivePutint() {
    TamData N = this->popData();
    std::cout << N;
}

} // namespace tam
