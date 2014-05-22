#include "int-init-x64.hpp"

namespace OS {
namespace x64 {

void InitializeInterrupts() {
  cout << "OS::x64::InitializeInterrupts()" << endl;
  IDT::Initialize();
  IRT::Initialize();
  IRT::GetGlobal().ConfigureDummy();
  IDT::GetGlobal().Load();
  RemapPIC(0xf0, 0xf8, 0xff, 0xff);
  SetInterruptsEnabled(true);
  // if there are a few interrupts masked, we can trigger our handlers for all
  // of them and send appropriate EOIs to the PIC.
  __asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop");
  SetInterruptsEnabled(false);
  IRT::GetGlobal().Configure();
}

void ConfigureIOAPIC() {
  cout << "OS::x64::ConfigureIOAPIC()" << endl;
  IOAPIC::Initialize();
  IOAPIC & apic = IOAPIC::GetBase();
  for (uint32_t i = 0; i < apic.GetPinCount(); i++) {
    apic.MaskPin(i);
  }
  IOAPIC::StartUsing();
}

void InitializeLAPIC() {
  LAPIC & lapic = LAPIC::GetCurrent();
  lapic.SetDefaults();
  lapic.Enable();
  cout << "OS::x64::InitializeLAPIC() - enabled LAPIC (id=" << lapic.GetId()
    << ")" << endl;
}

}
}
