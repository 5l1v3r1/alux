/*
 * Copyright (c) 2014, Alex Nichol and Alux contributors.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "physical-alloc-x64.h"

namespace OS {

namespace x64 {
  
  static PhysicalAllocator allocator;
  
  void PhysicalAllocator::Initialize(void * mbootPtr) {
    new(&allocator) PhysicalAllocator(mbootPtr);
    allocator.Setup();
  }
  
  PhysicalAllocator & PhysicalAllocator::GetGlobal() {
    return allocator;
  }
  
  PhysicalAllocator::PhysicalAllocator(void * mbootPtr)
    : regions(mbootPtr), lock(0), totalSpace(0) {
    for (int i = 0; i < regions.GetRegionCount(); i++) {
      totalSpace += regions.GetRegions()[i].GetSize();
    }
  }
  
  void PhysicalAllocator::Setup() {
    KernelMap & kernMap = KernelMap::GetGlobal();
    StepAllocator stepper(&regions, KernelDataSize());
    kernMap.allocator = &stepper;
    kernMap.Setup();
    kernMap.Set();
    
    AllocatorList::InitInfo info(0x1000000, 0x1000, 0x1000,
                                 regions.GetRegions(),
                                 regions.GetRegionCount());
    allocators.SetInfo(info);
    allocators.GenerateDescriptions(true);
    
    // allocate enough physical memory for the bitmap data
    PhysAddr firstFree;
    VirtAddr bitmapPtr = AllocateBitmaps(stepper, firstFree);
    
    allocators.GenerateAllocators((uint8_t *)bitmapPtr);
    
    MemoryRegion reg(0, firstFree);
    allocators.Reserve(reg);
    
    kernMap.allocator = this;
    
    cout << "OS::x64::PhysicalAllocator::Setup() - firstFree="
      << firstFree << endl;
  }

  bool PhysicalAllocator::Alloc(size_t size,
                                PhysAddr & addr,
                                size_t * realSize) {
    return Align(size, 1, addr, realSize);
  }
  
  bool PhysicalAllocator::Align(size_t size,
                                size_t align,
                                PhysAddr & addr,
                                size_t * realSize) {
    ScopeLock scope(&lock);
    return allocators.AllocPointer(size, align, (uintptr_t &)addr, realSize);
  }
  
  void PhysicalAllocator::Free(PhysAddr addr) {
    ScopeLock scope(&lock);
    allocators.FreePointer(addr);
  }
  
  size_t PhysicalAllocator::Available() {
    ScopeLock scope(&lock);
    return allocators.AvailableSpace();
  }
  
  size_t PhysicalAllocator::Used() {
    return totalSpace - Available();
  }
  
  PhysAddr PhysicalAllocator::AllocPage() {
    PhysAddr result;
    bool stat = Align(0x1000, 0x1000, result, NULL);
    if (!stat) Panic("OS::x64::PhysicalAllocator::AllocPage() - failed");
    return result;
  }
  
  void PhysicalAllocator::FreePage(PhysAddr p) {
    Free(p);
  }
  
  /***********
   * Private *
   ***********/

  void PhysicalAllocator::GrabSpace(bool large,
                                    StepAllocator & alloc,
                                    bool & hasStarted,
                                    uint64_t & lastPtr,
                                    uint64_t & firstPtr) {
    // allocate the data
    KernelMap & kernMap = KernelMap::GetGlobal();
    size_t pageSize = large ? 0x200000 : 0x1000;
    PhysAddr physPtr = alloc.AllocSize(pageSize);

    // map
    if (hasStarted) {
      kernMap.MapAt(lastPtr, physPtr, pageSize, large);
      lastPtr += pageSize;
    } else {
      hasStarted = true;
      firstPtr = kernMap.Map(physPtr, pageSize, large);
      if (!firstPtr) {
        Panic("OS::x64::PhysicalAllocator::GrabSpace() - Map() failed");
      }
      lastPtr = firstPtr + pageSize;
    }
  }
  
  VirtAddr PhysicalAllocator::AllocateBitmaps(StepAllocator<0x1000> & alloc,
                                              PhysAddr & firstFree) {
    ssize_t remaining = (ssize_t)allocators.BitmapByteCount();
    
    // DEBUG: set this to make sure large pages work
    // remaining = 0x400000;
    
    bool hasStarted = false;
    uint64_t lastPtr;
    uint64_t firstPtr;
    size_t pageSize = (remaining >= 0x200000 ? 0x200000 : 0x1000);
    while (remaining > 0) {
      GrabSpace(pageSize == 0x200000, alloc, hasStarted, lastPtr, firstPtr);
      remaining -= pageSize;
    }
    firstFree = alloc.lastAddr;
    return firstPtr;
  }

}

bool PhysicalAlloc(size_t size, PhysAddr & addr, size_t * realSize) {
  return x64::PhysicalAllocator::GetGlobal().Alloc(size, addr, realSize);
}

bool PhysicalAlign(size_t size,
                   size_t align,
                   PhysAddr & addr,
                   size_t * realSize) {
  return x64::PhysicalAllocator::GetGlobal().Align(size, align, addr,
                                                   realSize);
}

void PhysicalFree(PhysAddr addr) {
  x64::PhysicalAllocator::GetGlobal().Free(addr);
}

size_t PhysicalAvailable() {
  return x64::PhysicalAllocator::GetGlobal().Available();
}

size_t PhysicalUsed() {
  return x64::PhysicalAllocator::GetGlobal().Used();
}

}
