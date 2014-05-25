#include <utilities/lock.hpp>
#include <utilities/critical.hpp>

extern "C" {

#include <anlock.h>

}

namespace OS {

ScopeLock::ScopeLock(uint64_t * _lock) : lock(_lock) {
  AssertCritical();
  anlock_lock(lock);
}

ScopeLock::~ScopeLock() {
  anlock_unlock(lock);
}

}
