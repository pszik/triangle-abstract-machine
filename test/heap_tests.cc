#include "integration_test.h"
#include <gtest/gtest.h>
#include <map>
#include <tam/tam.h>

TEST_F(EmulatorTest, HeapAllocateExpandHeap) {
    ASSERT_NO_THROW({ this->allocate(3); });
    EXPECT_EQ(65532, this->Registers[tam::HT]);
    EXPECT_TRUE(this->AllocatedBlocks.count(this->Registers[tam::HT] + 1));
    EXPECT_EQ(3, this->AllocatedBlocks[65533]);
}

TEST_F(EmulatorTest, HeapAllocateReuseBlock) {
    this->Registers[tam::HT] = 65530;
    this->AllocatedBlocks[65531] = 2;
    this->FreeBlocks[65533] = 3;

    ASSERT_NO_THROW({ this->allocate(2); });
    EXPECT_TRUE(this->AllocatedBlocks.count(65533))
        << "Block not allocated successfully";
    EXPECT_EQ(2, this->AllocatedBlocks[65533])
        << "Allocated block has wrong size";
    EXPECT_FALSE(this->FreeBlocks.count(65533))
        << "Free block not marked as used";
    EXPECT_TRUE(this->FreeBlocks.count(65535))
        << "Leftover heap not marked as free";
    EXPECT_EQ(1, this->FreeBlocks[65535]) << "Unallocated block has wrong size";
}

TEST_F(EmulatorTest, FreeEndOfHeap) {
    this->Registers[tam::HT] = 65533;
    this->AllocatedBlocks[65534] = 2;

    ASSERT_NO_THROW({ this->free(65534); });
    EXPECT_FALSE(this->AllocatedBlocks.count(65534));
    EXPECT_EQ(65535, this->Registers[tam::HT]);
}

TEST_F(EmulatorTest, FreeMiddleOfHeap) {
    this->Registers[tam::HT] = 65530;
    this->AllocatedBlocks[65531] = 2;
    this->AllocatedBlocks[65533] = 3;

    ASSERT_NO_THROW({ this->free(65533); });
    EXPECT_EQ(65530, this->Registers[tam::HT]) << "HT unexpectedly changed";
    EXPECT_FALSE(this->AllocatedBlocks.count(65533));
    EXPECT_TRUE(this->FreeBlocks.count(65533));
    EXPECT_EQ(3, this->FreeBlocks[65533]);
}
