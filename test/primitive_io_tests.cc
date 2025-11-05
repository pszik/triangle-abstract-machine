#include <stdio.h>

#include <vector>

#include "integration_test.h"
#include <gtest/gtest.h>

TEST_F(IoTest, GeteolTest) {
    char input[] = " 123 \n456";
    FILE* instream = fmemopen(input, 10, "r");
    this->setInstream(instream);

    ASSERT_NO_THROW({ this->PrimitiveGeteol(); });
    char c = getc(instream);
    ASSERT_EQ('4', c);

    fclose(instream);
}

TEST_F(IoTest, GetintTest) {
    char input[] = " 8008\n2";
    FILE* instream = fmemopen(input, 7, "r");
    this->setInstream(instream);

    DataVec data = {0};
    this->setData(data);

    ASSERT_NO_THROW({ this->PrimitiveGetint(); });
    ASSERT_EQ(8008, this->data_store[0]);
    ASSERT_EQ('2', getc(instream));

    fclose(instream);
}
