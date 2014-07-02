#ifndef __SCHEDULER_USER_CODE_HPP__
#define __SCHEDULER_USER_CODE_HPP__

#include <arch/general/address-space.hpp>
#include <cstdint>
#include <atomic>

namespace OS {

class UserCode {
public:
  /**
   * @noncritical
   */
  UserCode(void * virtCode, size_t codeSize);
  ~UserCode(); // @noncritical
  
  size_t GetPageAlignment();
  PhysAddr GetPhysStart(); // @noncritical
  const AddressSpace::Size & GetPhysSize(); // @noncritical
  
protected:
  size_t alignment = 0;
  PhysAddr physStart = 0;
  AddressSpace::Size physSize;
};

}

#endif