#include <gtest/gtest.h>
#include <tam/tam.h>
#include <vector>

#include "integration_test.h"

TEST_F(EmulatorTest, TestLoadProgram) {
    std::vector<tam::TamCode> Code;
    Code.push_back(0x12345678);
    Code.push_back(0x9abcdef0);
    Code.push_back(0xfedcba98);

    ASSERT_NO_THROW({ this->loadProgram(Code); });

    EXPECT_EQ(0x12345678, this->CodeStore[0]);
    EXPECT_EQ(0x9abcdef0, this->CodeStore[1]);
    EXPECT_EQ(0xfedcba98, this->CodeStore[2]);
    EXPECT_EQ(3, this->Registers[tam::CT]);
    EXPECT_EQ(3, this->Registers[tam::PB]);
    EXPECT_EQ(32, this->Registers[tam::PT]);
}

TEST_F(EmulatorTest, TestSimpleCycle) {
    this->CodeStore[0] = 0x08020000;
    this->DataStore[0] = 0x1234;
    this->DataStore[1] = 0x5678;
    this->DataStore[2] = 0x9abc;

    this->Registers[tam::CT] = 1;
    this->Registers[tam::CP] = 0;
    this->Registers[tam::ST] = 3;

    ASSERT_NO_THROW({
        tam::TamInstruction Instr = this->fetchDecode();
        this->execute(Instr);
    });

    EXPECT_EQ(0x1234, this->DataStore[3]);
    EXPECT_EQ(0x5678, this->DataStore[4]);
}
