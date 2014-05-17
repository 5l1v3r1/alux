#include "int-handlers-x64.hpp"

extern "C" {

void InterruptCoded(void * caller, uint64_t vector, uint64_t code) {
  OS::cout << "OS::x64::InterruptCoded() - caller=" << (uintptr_t)caller
    << " vector=" << vector << " code=" << code << OS::endl;
  OS::Panic("nothing to do in interrupt handler");
}

void InterruptRegular(void * caller, uint64_t vector) {
  OS::cout << "OS::x64::InterruptRegular() - caller=" << (uintptr_t)caller
    << " vector=" << vector << OS::endl;
  OS::Panic("nothing to do in interrupt handler");
}

void InterruptDummy(void * caller, uint64_t vector) {
  // send EOI to the PIC so it backs off
  (void)caller;
  if (vector > 0xf) {
    OS::x64::OutB(0xa0, 0x20);
  }
  OS::x64::OutB(0x20, 0x20);
}

void _InterruptCoded(void * caller, uint64_t vector, uint64_t code) {
  InterruptCoded(caller, vector, code);
}

void _InterruptRegular(void * caller, uint64_t vector) {
  InterruptRegular(caller, vector);
}

void _InterruptDummy(void * caller, uint64_t vector) {
  InterruptDummy(caller, vector);
}

}

namespace OS {

namespace x64 {

static InterruptTable globalIdt;

void InitializeIDT() {
  new(&globalIdt) InterruptTable();
}

InterruptTable & GetGlobalIDT() {
  return globalIdt;
}

void ConfigureDummyIDT() {
  for (int i = 0; i < 0x100; i++) {
    globalIdt.SetHandler(i, (void *)RawDummyHandler, 0x8e);
  }
}

void ConfigureRealIDT() {
  for (int i = 0; i < 0x100; i++) {
    bool hasCode = (i == 0x8 || i == 0x11 || (i >= 0xa && i <= 0xe));
    if (hasCode) {
      globalIdt.SetHandler(i, (void *)RawCodedHandler, 0x8e);
    } else {
      globalIdt.SetHandler(i, (void *)RawNonCodedHandler, 0x8e);
    }
  }
}

}

}
