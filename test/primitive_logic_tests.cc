#include <vector>

#include "tam/tam.h"
#include "tam/test/integration_test.h"

#include "gtest/gtest.h"
#include <gtest/gtest.h>

std::vector<std::pair<tam::TamData, tam::TamData>> logic_values = {
    {0, 0}, {0, 1}, {1, 0}, {1, 1}};

class PrimitiveLogicTests : public EmulatorTest,
                            public testing::WithParamInterface<
                                std::pair<tam::TamData, tam::TamData>> {};

testing::AssertionResult Conjunction(int l, int r, int actual) {
    if ((l && r) == actual) {
        return testing::AssertionSuccess();
    }

    return testing::AssertionFailure()
           << "(" << l << " && " << r << ") was " << !actual;
}

TEST_P(PrimitiveLogicTests, TestAnd) {
    auto args = GetParam();
    DataVec data = {args.first, args.second};
    this->setData(data);

    ASSERT_NO_THROW(this->PrimitiveAnd());

    int result = this->data_store_[0];
    ASSERT_TRUE(Conjunction(args.first, args.second, result));
}

testing::AssertionResult Disjunction(int l, int r, int actual) {
    if ((l || r) == actual) {
        return testing::AssertionSuccess();
    }

    return testing::AssertionFailure()
           << "(" << l << " || " << r << ") was " << !actual;
}

TEST_P(PrimitiveLogicTests, TestOr) {
    auto args = GetParam();
    DataVec data = {args.first, args.second};
    this->setData(data);

    ASSERT_NO_THROW(this->PrimitiveOr());

    int result = this->data_store_[0];
    ASSERT_TRUE(Disjunction(args.first, args.second, result));
}

TEST_F(PrimitiveLogicTests, TestNotTrue) {
    this->PushData(1);

    ASSERT_NO_THROW(this->PrimitiveNot());

    int result = this->data_store_[0];
    ASSERT_EQ(0, result);
}

TEST_F(PrimitiveLogicTests, TestNotFalse) {
    this->PushData(0);

    ASSERT_NO_THROW(this->PrimitiveNot());

    int result = this->data_store_[0];
    ASSERT_EQ(1, result);
}

INSTANTIATE_TEST_SUITE_P(LogicValues, PrimitiveLogicTests,
                         testing::ValuesIn(logic_values));
