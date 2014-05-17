#ifndef __PLATFORM_X64_IDT_STRUCTURES_X64_H__
#define __PLATFORM_X64_IDT_STRUCTURES_X64_H__

#include <utilities/common.hpp>
#include <cstdint>
#include <cstring>

namespace OS {

namespace x64 {

// NOTICE: the C++11 standard is needed for this file because it uses packed
// structures with initializers.

/**
 * Represents this x86-64 machine code:
 * 
 * pushq %rax
 * pushq %rax
 * movabsq $argument, %rax
 * movq %rax, 0x8(%rsp)
 * movabsq $function, %rax
 * xchgq %rax, (%rsp)
 * ret
 */
class IntHandler {
public:
  char code1[4];
  uint64_t argument;
  char code2[7];
  uint64_t function;
  char code3[5];
  
  IntHandler() {
    memcpy(code1, "\x50\x50\x48\xB8", 4);
    memcpy(code2, "\x48\x89\x44\x24\x08\x48\xB8", 7);
    memcpy(code3, "\x48\x87\x04\x24\xC3", 5);
    argument = function = 0;
  }
} OS_PACKED;
static_assert(sizeof(IntHandler) == 0x20, "IntHandler size == 0x20");

class IdtEntry {
public:
  uint16_t lowOffset;
  uint16_t codeSegment;
  uint8_t reserved1;
  uint8_t flags;
  uint16_t midOffset;
  uint32_t highOffset;
  uint32_t reserved2;
  
  IdtEntry() {
    codeSegment = 8;
    reserved1 = 0;
    reserved2 = 0;
  }
  
  void SetOffset(uint64_t exc) {
    lowOffset = exc & 0xffff;
    midOffset = (exc >> 16) & 0xffff;
    highOffset = exc >> 32;
  }
} OS_PACKED;
static_assert(sizeof(IdtEntry) == 0x10, "IdtEntry size == 0x10");

class IdtPointer {
public:
  uint16_t limit;
  uint64_t virtualAddress;
} OS_PACKED;
static_assert(sizeof(IdtPointer) == 0x0a, "IdtPointer size == 0x0a");

}

}

#endif

