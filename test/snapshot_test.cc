#include "integration_test.h"

#include <tam/tam.h>

using namespace tam;

TEST_F(EmulatorTest, GetSnapshotTest) {
    DataVec Data = {123, 456, 789, 11112, 11415};
    this->setData(Data);

    tam::TamAddr Addr = this->Allocate(3);
    this->data_store[Addr] = 246;
    this->data_store[Addr + 1] = 8112;
    this->data_store[Addr + 2] = 1416;

    std::string Snapshot = this->GetSnapshot();
    std::string ExpectedSnapshot =
        "stack\n007b 01c8 0315 2b68 2c97 \nheap fffd\n00f6 1fb0 0588 ";
    EXPECT_EQ(ExpectedSnapshot, Snapshot);
}
