#ifndef __X64_CLOCK_HPP__
#define __X64_CLOCK_HPP__

#include <arch/general/clock.hpp>

namespace OS {

namespace x64 {

class ClockModule : public OS::ClockModule {
public:
  static void InitGlobal();
  static ClockModule & GetGlobal();
  
  virtual OS::Clock & GetClock();
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  
private:
  Clock * realClock;
};

}

}

#endif