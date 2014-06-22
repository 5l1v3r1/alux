#include <arch-specific/task.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/user-map.hpp>
#include <scheduler/general/task.hpp>
#include <critical>
#include <panic>

namespace OS {

namespace x64 {

ArchTask::ArchTask(bool kernel) {
  AssertNoncritical();
  if (!kernel) {
    addressSpace = new UserMap();
  } else {
    addressSpace = &GlobalMap::GetGlobal();
  }
}

ArchTask::~ArchTask() {
  AssertNoncritical();
  AddressSpace * space = &GlobalMap::GetGlobal();
  if (addressSpace != space) {
    if (addressSpace) delete addressSpace;
  }
}

AddressSpace & ArchTask::GetAddressSpace() {
  return *addressSpace;
}

}

}
