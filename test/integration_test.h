#ifndef INTEGRATION_TEST_H__
#define INTEGRATION_TEST_H__

#include <cassert>
#include <gtest/gtest.h>
#include <tam/tam.h>
#include <vector>

typedef std::vector<tam::TamCode> CodeVec;
typedef std::vector<tam::TamData> DataVec;

class EmulatorTest : public testing::Test, protected tam::TamEmulator {
  protected:
    void setCode(CodeVec &Code) {
        assert(Code.size() < 65536);
        this->CodeStore.fill(0);

        std::copy(Code.begin(), Code.end(), this->CodeStore.begin());
        for (int I = 0; I < Code.size(); ++I) {
            assert(Code[I] == this->CodeStore[I]);
        }

        this->Registers[tam::CT] = Code.size();
        this->Registers[tam::PB] = this->Registers[tam::CT];
        this->Registers[tam::PT] = this->Registers[tam::PB] + 29;
        assert(this->Registers[tam::CT] == Code.size());
    }

    void setData(DataVec &Data) {
        assert(Data.size() < 65536);
        this->DataStore.fill(0);

        std::copy(Data.begin(), Data.end(), this->DataStore.begin());
        for (int I = 0; I < Data.size(); ++I) {
            assert(Data[I] == this->DataStore[I]);
        }

        this->Registers[tam::ST] = Data.size();
        assert(this->Registers[tam::ST] == Data.size());
    }
};

#endif // INTEGRATION_TEST_H__
