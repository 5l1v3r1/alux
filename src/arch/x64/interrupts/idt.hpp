#ifndef __X64_IDT_HPP__
#define __X64_IDT_HPP__

#include <arch/x64/interrupts/types.hpp>

namespace OS {

namespace x64 {

class IDT {
private:
  IdtEntry entries[0x100] OS_ALIGNED(8);
  IdtHandler handlers[0x100];
  IdtPointer idtPtr OS_ALIGNED(8);
  
public:
  static void Initialize();
  static IDT & GetGlobal();
  
  void SetEntry(int idx, void * fn, uint8_t flags);
  void SetIST(int idx, uint8_t ist);
  void Load();
};

}

}

#endif
