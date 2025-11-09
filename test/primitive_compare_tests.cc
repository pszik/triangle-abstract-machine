#include <tuple>

#include "tam/test/integration_test.h"

#include "gtest/gtest.h"

class PrimitiveCompareTests : public EmulatorTest,
                              public testing::WithParamInterface<
                                  std::tuple<tam::TamData, tam::TamData>> {};

static std::vector<tam::TamData> int_params = {0,   1,         -1,       42,
                                               -42, INT16_MAX, INT16_MIN};

TEST_P(PrimitiveCompareTests, TestLt) {
    const auto [arg1, arg2] = GetParam();
    this->PushData(arg1);
    this->PushData(arg2);

    ASSERT_NO_THROW(this->PrimitiveLt());

    int result = this->data_store_[0];
    ASSERT_EQ(arg1 < arg2, result);
}

TEST_P(PrimitiveCompareTests, TestLe) {
    const auto [arg1, arg2] = GetParam();
    this->PushData(arg1);
    this->PushData(arg2);

    ASSERT_NO_THROW(this->PrimitiveLe());

    int result = this->data_store_[0];
    ASSERT_EQ(arg1 <= arg2, result);
}

TEST_P(PrimitiveCompareTests, TestGe) {
    const auto [arg1, arg2] = GetParam();
    this->PushData(arg1);
    this->PushData(arg2);

    ASSERT_NO_THROW(this->PrimitiveGe());

    int result = this->data_store_[0];
    ASSERT_EQ(arg1 >= arg2, result);
}

TEST_P(PrimitiveCompareTests, TestGt) {
    const auto [arg1, arg2] = GetParam();
    this->PushData(arg1);
    this->PushData(arg2);

    ASSERT_NO_THROW(this->PrimitiveGt());

    int result = this->data_store_[0];
    ASSERT_EQ(arg1 > arg2, result);
}

TEST_F(PrimitiveCompareTests, TestEqTrue) {
    DataVec data = {42, 24, 42, 24, 2};
    this->setData(data);

    ASSERT_NO_THROW(this->PrimitiveEq());

    int result = this->data_store_[0];
    ASSERT_EQ(1, result);
}

TEST_F(PrimitiveCompareTests, TestEqFalse) {
    DataVec data = {42, 24, 24, 42, 2};
    this->setData(data);

    ASSERT_NO_THROW(this->PrimitiveEq());

    int result = this->data_store_[0];
    ASSERT_EQ(0, result);
}

INSTANTIATE_TEST_SUITE_P(IntPairs, PrimitiveCompareTests,
                         testing::Combine(testing::ValuesIn(int_params),
                                          testing::ValuesIn(int_params)));
