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
/// This file defines the `Allocate` and `free` methods of `TamEmulator`.
//
//===-----------------------------------------------------------------------===//

#include <cassert>
#include <map>

#include "tam/error.h"
#include "tam/tam.h"

/// Iterates through all existing free blocks in the heap to try and find one of
/// the correct size. If none is found, the heap is expanded.
tam::TamAddr tam::TamEmulator::Allocate(int n) {
    // try to find unallocated space inside heap
    for (auto block_iter = this->free_blocks.begin(),
              iter_end = this->free_blocks.end();
         block_iter != iter_end; ++block_iter) {
        assert(block_iter->first > this->registers[HT]);

        if (block_iter->second < n)  // block not big enough
            continue;

        TamAddr block_start = block_iter->first;

        this->allocated_blocks.emplace(block_start, n);
        this->free_blocks.erase(block_iter);
        if (block_iter->second > n)
            this->free_blocks.emplace(block_start + n, block_iter->second - n);

        return block_start;
    }

    // expand heap
    this->registers[HT] -= n;
    if (this->registers[HT] <= this->registers[ST])
        throw RuntimeError(kHeapOverflow, this->registers[CP] - 1);

    this->allocated_blocks.emplace(this->registers[HT] + 1, n);
    return this->registers[HT] + 1;
}

/// Attempts to locate an allocated block of the given address and size.
/// If the block was at the end of the heap then the heap is contracted,
/// otherwise the freed block is added to the list of available blocks.
void tam::TamEmulator::Free(TamAddr addr, TamData size) {
    if (addr <= this->registers[HT])
        throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

    if (!this->allocated_blocks.count(addr))
        throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

    for (auto block_iter = this->allocated_blocks.begin(),
              iter_end = this->allocated_blocks.end();
         block_iter != iter_end; ++block_iter) {
        assert(block_iter->first > this->registers[HT]);

        if (block_iter->first != addr)  // not the specified block
            continue;

        if (block_iter->second != size)  // block does not have specified size
            throw RuntimeError(kDataAccessViolation, this->registers[CP] - 1);

        this->allocated_blocks.erase(block_iter);
        if (addr == this->registers[HT] + 1) {
            // block on top of heap, shrink heap
            this->registers[HT] += block_iter->second;
        } else {
            // mark block as available
            this->free_blocks.emplace(block_iter->first, block_iter->second);
        }
        break;
    }
}
