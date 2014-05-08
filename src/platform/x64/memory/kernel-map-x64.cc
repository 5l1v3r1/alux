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

#include "kernel-map-x64.h"

namespace OS {

namespace x64 {

KernelMap::KernelMap() {
  scratchLock = 0;
  bzero(scratchBitmaps, sizeof(scratchBitmaps));
}

PhysAddr KernelMap::Setup(PhysRegionList * regs) {
  MapSetup setup(regs);
  setup.Map();
  
  assert(ScratchPTCount <= 0x200);
  assert(setup.GetFirstUnmapped() <= 0x7FC0000000);
  
  PhysAddr scratchPDT = setup.AllocPage();
  ((uint64_t *)setup.GetPDPT())[0x1ff] = scratchPDT | 3;
  uint64_t scratchStart = (uint64_t)ScratchPTStart();
  for (int i = 0; i < ScratchPTCount; i++) {
    PhysAddr scratchPT = scratchStart + (i << 12);
    ((uint64_t *)scratchPDT)[i] = scratchPT | 3;
  }
  
  pml4 = setup.GetPML4();
  pdpt = setup.GetPDPT();
  
  return setup.GetFirstFree();
}

void KernelMap::Set() {
  __asm__("mov %0, %%cr3" : : "r" (pml4));
}

PhysAddr KernelMap::GetPDPT() {
  return pdpt;
}

VirtAddr KernelMap::Map(PhysAddr start, size_t size, bool largePages) {
  return 0;
}

VirtAddr KernelMap::AllocScratch(PhysAddr start) {
  return 0;
}

void KernelMap::FreeScratch(VirtAddr ptr) {
  
}

}

}