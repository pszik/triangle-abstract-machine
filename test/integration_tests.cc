#include "integration_test.h"
#include <gtest/gtest.h>
#include <vector>

#include "tam/tam.h"

using namespace tam;

TEST_F(EmulatorTest, TestLoadProgram) {
    std::vector<tam::TamCode> code{0x12345678, 0x9abcdef0, 0xfedcba98};

    ASSERT_NO_THROW({ this->LoadProgram(code); });

    EXPECT_EQ(0x12345678, this->code_store[0]);
    EXPECT_EQ(0x9abcdef0, this->code_store[1]);
    EXPECT_EQ(0xfedcba98, this->code_store[2]);
    EXPECT_EQ(3, this->registers[tam::CT]);
    EXPECT_EQ(3, this->registers[tam::PB]);
    EXPECT_EQ(32, this->registers[tam::PT]);
}

TEST_F(EmulatorTest, TestSimpleCycle) {
    this->code_store[0] = 0x08020000;
    this->data_store[0] = 0x1234;
    this->data_store[1] = 0x5678;
    this->data_store[2] = 0x9abc;

    this->registers[tam::CT] = 1;
    this->registers[tam::CP] = 0;
    this->registers[tam::ST] = 3;

    ASSERT_NO_THROW({
        tam::TamInstruction instr = this->FetchDecode();
        this->Execute(instr);
    });

    EXPECT_EQ(0x1234, this->data_store[3]);
    EXPECT_EQ(0x5678, this->data_store[4]);
}
