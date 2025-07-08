#include "integration_test.h"
#include <gtest/gtest.h>
#include <tam/tam.h>

TEST_F(EmulatorTest, TestLoadOk) {
    std::vector<tam::TamData> Data = {1, 2, 3, 4, 5};
    this->setData(Data);

    tam::TamInstruction Instr = {0, tam::SB, 2, 0};
    ASSERT_NO_THROW({ this->execute(Instr); });

    EXPECT_EQ(7, this->Registers[tam::ST]);
    EXPECT_EQ(1, this->DataStore[5]);
    EXPECT_EQ(2, this->DataStore[6]);
}

TEST_F(EmulatorTest, TestLoadaOk) {
    std::vector<tam::TamData> Data = {1, 2, 3};
    this->setData(Data);

    tam::TamInstruction Instr = {1, tam::SB, 0, 1};
    ASSERT_NO_THROW({ this->execute(Instr); });

    EXPECT_EQ(4, this->Registers[tam::ST]);
    EXPECT_EQ(1, this->DataStore[3]);
}

TEST_F(EmulatorTest, TestLoadiOk) {
    std::vector<tam::TamData> Data = {4, 3, 2, 1, 1};
    this->setData(Data);

    tam::TamInstruction Instr = {2, tam::SB, 2, 0};
    ASSERT_NO_THROW({ this->execute(Instr); });

    EXPECT_EQ(6, this->Registers[tam::ST]);
    EXPECT_EQ(3, this->DataStore[4]);
    EXPECT_EQ(2, this->DataStore[5]);
}

TEST_F(EmulatorTest, TestLoadlOk) {
    std::vector<tam::TamData> Data = {1, 2, 3};
    this->setData(Data);

    tam::TamInstruction Instr = {3, tam::SB, 0, 10};
    ASSERT_NO_THROW({ this->execute(Instr); });

    EXPECT_EQ(4, this->Registers[tam::ST]);
    EXPECT_EQ(10, this->DataStore[3]);
}
