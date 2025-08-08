//===-----------------------------------------------------------------------===//
//
// This file is part of tam-cpp, copyright (c) Ian Knight 2025.
//
// tam-cpp is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// tam-cpp is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with tam-cpp. If not, see <https://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//
//
/// @file heap.cc
/// This file defines the `allocate` and `free` methods of `TamEmulator`.
//
//===-----------------------------------------------------------------------===//

#include <tam/tam.h>

#include <tam/error.h>

#include <cassert>
#include <map>

using namespace tam;

/// Iterates through all existing free blocks in the heap to try and find one of
/// the correct size. If none is found, the heap is expanded.
tam::TamAddr tam::TamEmulator::allocate(int N) {
    // try to find unallocated space inside heap
    for (auto BlockIter = this->FreeBlocks.begin();
         BlockIter != this->FreeBlocks.end(); ++BlockIter) {
        assert(BlockIter->first > this->Registers[HT]);

        if (BlockIter->second < N) // block not big enough
            continue;

        TamAddr BlockStart = BlockIter->first;

        this->AllocatedBlocks.emplace(BlockStart, N);
        this->FreeBlocks.erase(BlockIter);
        if (BlockIter->second > N)
            this->FreeBlocks.emplace(BlockStart + N, BlockIter->second - N);

        return BlockStart;
    }

    // expand heap
    this->Registers[HT] -= N;
    if (this->Registers[HT] <= this->Registers[ST])
        throw runtimeError(EK_HeapOverflow, this->Registers[CP] - 1);

    this->AllocatedBlocks.emplace(this->Registers[HT] + 1, N);
    return this->Registers[HT] + 1;
}

/// Attempts to locate an allocated block of the given address and size.
/// If the block was at the end of the heap then the heap is contracted,
/// otherwise the freed block is added to the list of available blocks.
void tam::TamEmulator::free(TamAddr Addr, TamData Size) {
    if (Addr <= this->Registers[HT])
        throw runtimeError(EK_DataAccessViolation, this->Registers[CP] - 1);

    if (!this->AllocatedBlocks.count(Addr))
        throw runtimeError(EK_DataAccessViolation, this->Registers[CP] - 1);

    for (auto BlockIter = this->AllocatedBlocks.begin(),
              E = this->AllocatedBlocks.end();
         BlockIter != E; ++BlockIter) {
        assert(BlockIter->first > this->Registers[HT]);

        if (BlockIter->first != Addr) // not the specified block
            continue;

        if (BlockIter->second != Size) // block does not have specified size
            throw runtimeError(EK_DataAccessViolation, this->Registers[CP] - 1);

        this->AllocatedBlocks.erase(BlockIter);
        if (Addr == this->Registers[HT] + 1) {
            // block on top of heap, shrink heap
            this->Registers[HT] += BlockIter->second;
        } else {
            // mark block as available
            this->FreeBlocks.emplace(BlockIter->first, BlockIter->second);
        }
        break;
    }
}
