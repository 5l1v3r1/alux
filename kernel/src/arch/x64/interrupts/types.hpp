#ifndef __X64_INTERRUPT_TYPES_HPP__
#define __X64_INTERRUPT_TYPES_HPP__

#include <cstring>
#include <cstdint>
#include <macros>

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
class IdtHandler {
public:
  // TODO: initialize these fields with memcpy
  char code1[4];
  uint64_t argument = 0;
  char code2[7];
  uint64_t function = 0;
  char code3[5];
  
  IdtHandler() {
    memcpy(code1, "\x50\x50\x48\xB8", 4);
    memcpy(code2, "\x48\x89\x44\x24\x08\x48\xB8", 7);
    memcpy(code3, "\x48\x87\x04\x24\xC3", 5);
  }
} OS_PACKED;
static_assert(sizeof(IdtHandler) == 0x20, "IdtHandler size == 0x20");

class IdtEntry {
public:
  uint16_t lowOffset = 0;
  uint16_t codeSegment = 8;
  uint8_t ist : 3;
  uint8_t reserved1 : 5;
  uint8_t flags = 0;
  uint16_t midOffset = 0;
  uint32_t highOffset = 0;
  uint32_t reserved2 = 0;
  
  IdtEntry() : ist(0), reserved1(0) {}
  
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

#endif
