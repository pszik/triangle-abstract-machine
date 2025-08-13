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
        this->code_store.fill(0);

        std::copy(Code.begin(), Code.end(), this->code_store.begin());
        for (int I = 0; I < Code.size(); ++I) {
            assert(Code[I] == this->code_store[I]);
        }

        this->registers[tam::CT] = Code.size();
        this->registers[tam::PB] = this->registers[tam::CT];
        this->registers[tam::PT] = this->registers[tam::PB] + 29;
        assert(this->registers[tam::CT] == Code.size());
    }

    void setData(DataVec &Data) {
        assert(Data.size() < 65536);
        this->data_store.fill(0);

        std::copy(Data.begin(), Data.end(), this->data_store.begin());
        for (int I = 0; I < Data.size(); ++I) {
            assert(Data[I] == this->data_store[I]);
        }

        this->registers[tam::ST] = Data.size();
        assert(this->registers[tam::ST] == Data.size());
    }
};

#endif // INTEGRATION_TEST_H__
