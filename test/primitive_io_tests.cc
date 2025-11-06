#include <stdio.h>

#include "tam/test/integration_test.h"

#include <gtest/gtest.h>

TEST_F(IoTest, GeteolTest) {
    FILE* instream = tmpfile();
    fprintf(instream, "123 \n456");
    rewind(instream);
    this->setInstream(instream);

    ASSERT_NO_THROW({ this->PrimitiveGeteol(); });
    char c = getc(instream);
    ASSERT_EQ('4', c);
}

TEST_F(IoTest, GetintTest) {
    FILE* instream = tmpfile();
    fprintf(instream, " 8008\n2");
    rewind(instream);
    this->setInstream(instream);

    DataVec data = {0};
    this->setData(data);

    ASSERT_NO_THROW({ this->PrimitiveGetint(); });
    ASSERT_EQ(8008, this->data_store[0]);
    ASSERT_EQ('2', getc(instream));
}
