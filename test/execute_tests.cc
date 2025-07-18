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

TEST_F(EmulatorTest, TestStoreOk) {
    std::vector<tam::TamData> Data = {1, 2, 3, 4, 5, 10, 20};
    this->setData(Data);

    tam::TamInstruction Instr = {4, tam::SB, 2, 1};
    ASSERT_NO_THROW({ this->execute(Instr); });

    EXPECT_EQ(5, this->Registers[tam::ST]);
    EXPECT_EQ(10, this->DataStore[1]);
    EXPECT_EQ(20, this->DataStore[2]);
}

TEST_F(EmulatorTest, TestStoreiOk) {
    std::vector<tam::TamData> Data = {1, 2, 3, 4, 5, 10, 20, 1};
    this->setData(Data);

    tam::TamInstruction Instr = {5, tam::SB, 2, 1};
    ASSERT_NO_THROW({ this->execute(Instr); });

    EXPECT_EQ(5, this->Registers[tam::ST]);
    EXPECT_EQ(10, this->DataStore[1]);
    EXPECT_EQ(20, this->DataStore[2]);
}

TEST_F(EmulatorTest, TestCallUserDefinedOk) {
    DataVec Data = {1, 2, 3, 4, 5};
    this->setData(Data);
    CodeVec Code = {1, 2, 3};
    this->setCode(Code);
    this->Registers[tam::CP] = 1;

    tam::TamInstruction Instr = {6, 0, 0, 2};
    ASSERT_NO_THROW({ this->execute(Instr); });

    EXPECT_EQ(0, this->DataStore[5]);
    EXPECT_EQ(0, this->DataStore[6]);
    EXPECT_EQ(1, this->DataStore[7]);
    EXPECT_EQ(8, this->Registers[tam::ST]);
    EXPECT_EQ(2, this->Registers[tam::CP]);
    EXPECT_EQ(5, this->Registers[tam::LB]);
}

TEST_F(EmulatorTest, TestReturnOk) {
    DataVec Data = {1,
                    2,
                    3,
                    5 /*arg1*/,
                    5 /*arg2*/,
                    0 /*static*/,
                    1 /*dynamic*/,
                    2 /*ret addr*/,
                    44,
                    88,
                    42};
    this->setData(Data);
    CodeVec Code = {10, 20, 30, 40, 50};
    this->setCode(Code);
    this->Registers[tam::LB] = 5;

    tam::TamInstruction Instr = {8, 0, 1, 2};
    ASSERT_NO_THROW({ this->execute(Instr); });

    EXPECT_EQ(42, this->DataStore[3]);
    EXPECT_EQ(4, this->Registers[tam::ST]);
    EXPECT_EQ(1, this->Registers[tam::LB]);
    EXPECT_EQ(2, this->Registers[tam::CP]);
}
