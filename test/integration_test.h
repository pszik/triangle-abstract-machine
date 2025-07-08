#ifndef INTEGRATION_TEST_H__
#define INTEGRATION_TEST_H__

#include <gtest/gtest.h>
#include <tam/tam.h>
#include <vector>

class EmulatorTest : public testing::Test, protected tam::TamEmulator {
  protected:
    void setCode(std::vector<tam::TamCode> &Code) {
        this->CodeStore.fill(0);
        std::copy(Code.begin(), Code.end(), this->CodeStore.begin());
        this->Registers[tam::CT] = Code.size();
        this->Registers[tam::PB] = this->Registers[tam::CT];
        this->Registers[tam::PT] = this->Registers[tam::PB] + 29;
    }

    void setData(std::vector<tam::TamData> &Data) {
        this->DataStore.fill(0);
        std::copy(Data.begin(), Data.end(), this->DataStore.begin());
        this->Registers[tam::ST] = Data.size();
    }
};

#endif // INTEGRATION_TEST_H__
