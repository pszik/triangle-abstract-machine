#include <cstdint>

#include "tam/tam.h"
#include "tam/test/integration_test.h"

#include "gtest/gtest.h"
#include <gtest/gtest.h>

class PrimitiveArithTests : public EmulatorTest,
                            public testing::WithParamInterface<tam::TamData> {};

std::vector<tam::TamData> int_params = {0,   1,         -1,       42,
                                        -42, INT16_MAX, INT16_MIN};

TEST_P(PrimitiveArithTests, TestSucc) {
    tam::TamData arg = GetParam();
    this->PushData(arg);

    ASSERT_NO_THROW(this->PrimitiveSucc());

    int result = this->data_store_[0];
    if (arg == INT16_MAX) {
        ASSERT_EQ(INT16_MIN, result);
    } else {
        ASSERT_EQ(arg + 1, result);
    }
}

TEST_P(PrimitiveArithTests, TestPred) {
    tam::TamData arg = GetParam();
    this->PushData(arg);

    ASSERT_NO_THROW(this->PrimitivePred());

    int result = this->data_store_[0];
    if (arg == INT16_MIN) {
        ASSERT_EQ(INT16_MAX, result);
    } else {
        ASSERT_EQ(arg - 1, result);
    }
}

TEST_P(PrimitiveArithTests, TestNeg) {
    tam::TamData arg = GetParam();
    this->PushData(arg);

    ASSERT_NO_THROW(this->PrimitiveNeg());

    tam::TamData result = this->data_store_[0];
    if (arg == INT16_MIN) {
        ASSERT_EQ(INT16_MIN, result);
    } else {
        ASSERT_EQ(-arg, result);
    }
}

TEST_P(PrimitiveArithTests, TestAdd) {
    tam::TamData arg = GetParam();
    this->PushData(arg);
    this->PushData(4);

    ASSERT_NO_THROW(this->PrimitiveAdd());

    int result = this->data_store_[0];
    if (arg == INT16_MAX) {
        ASSERT_EQ(INT16_MIN + 3, result);
    } else {
        ASSERT_EQ(arg + 4, result);
    }
}

TEST_P(PrimitiveArithTests, TestSub) {
    tam::TamData arg = GetParam();
    this->PushData(arg);
    this->PushData(4);

    ASSERT_NO_THROW(this->PrimitiveSub());

    int result = this->data_store_[0];
    if (arg == INT16_MIN) {
        ASSERT_EQ(INT16_MAX - 3, result);
    } else {
        ASSERT_EQ(arg - 4, result);
    }
}

TEST_P(PrimitiveArithTests, TestMult) {
    tam::TamData arg = GetParam();
    this->PushData(arg);
    this->PushData(2);

    ASSERT_NO_THROW(this->PrimitiveMult());

    int result = this->data_store_[0];
    switch (arg) {
        case INT16_MIN:
            ASSERT_EQ(0, result);
            break;
        case INT16_MAX:
            ASSERT_EQ(-2, result);
            break;
        default:
            ASSERT_EQ(arg * 2, result);
            break;
    }
}

TEST_P(PrimitiveArithTests, TestDiv) {
    tam::TamData arg = GetParam();
    this->PushData(arg);
    this->PushData(2);

    ASSERT_NO_THROW(this->PrimitiveDiv());

    int result = this->data_store_[0];
    ASSERT_EQ(arg / 2, result);
    switch (arg) {
        case INT16_MIN:
            ASSERT_EQ(-16384, result);
            break;
        case INT16_MAX:
            ASSERT_EQ(16383, result);
            break;
        default:
            ASSERT_EQ(arg / 2, result);
            break;
    }
}

TEST_P(PrimitiveArithTests, TestMod) {
    tam::TamData arg = GetParam();
    this->PushData(arg);
    this->PushData(4);

    ASSERT_NO_THROW(this->PrimitiveMod());

    int result = this->data_store_[0];
    ASSERT_EQ(arg % 4, result);
}

INSTANTIATE_TEST_SUITE_P(Nums, PrimitiveArithTests,
                         testing::ValuesIn(int_params));
