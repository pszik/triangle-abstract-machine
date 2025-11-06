#ifndef INTEGRATION_TEST_H__
#define INTEGRATION_TEST_H__

#include <assert.h>

#include <vector>

#include "tam/tam.h"

#include <gtest/gtest.h>

typedef std::vector<tam::TamCode> CodeVec;
typedef std::vector<tam::TamData> DataVec;

class EmulatorTest : public testing::Test, public tam::TamEmulator {
   protected:
    void setCode(CodeVec& code) {
        assert(code.size() < 65536);
        this->code_store.fill(0);

        std::copy(code.begin(), code.end(), this->code_store.begin());
        for (int I = 0; I < code.size(); ++I) {
            assert(code[I] == this->code_store[I]);
        }

        this->registers[tam::CT] = code.size();
        this->registers[tam::PB] = this->registers[tam::CT];
        this->registers[tam::PT] = this->registers[tam::PB] + 29;
        assert(this->registers[tam::CT] == code.size());
    }

    void setData(DataVec& data) {
        assert(data.size() < 65536);
        this->data_store.fill(0);

        std::copy(data.begin(), data.end(), this->data_store.begin());
        for (int I = 0; I < data.size(); ++I) {
            assert(data[I] == this->data_store[I]);
        }

        this->registers[tam::ST] = data.size();
        assert(this->registers[tam::ST] == data.size());
    }

    void setInstream(FILE* instream) {
        assert(instream);
        this->instream = instream;
    }

    void setOutstream(FILE* outstream) {
        assert(outstream);
        this->outstream = outstream;
    }
};

#endif  // INTEGRATION_TEST_H__
