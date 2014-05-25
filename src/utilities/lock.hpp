#ifndef __UTILITIES_LOCK_HPP__
#define __UTILITIES_LOCK_HPP__

#include <cstdint>

namespace OS {

class ScopeLock {
private:
  uint64_t * lock;

public:
  ScopeLock(uint64_t * lock); // @noncritical
  ~ScopeLock(); // @ambicritical
};

class ScopeCriticalLock {
private:
  uint64_t * lock;

public:
  ScopeCriticalLock(uint64_t * lock); // @noncritical
  ~ScopeCriticalLock(); // @ambicritical
};

}

#endif
