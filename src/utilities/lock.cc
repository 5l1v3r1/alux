#include "lock.hpp"
#include "critical.hpp"

namespace OS {

ScopeLock::ScopeLock(anlock_t lock) : theLock(lock) {
  assert(!((uint64_t)lock & 7));
  AssertNoncritical();
  anlock_lock(theLock);
}

ScopeLock::~ScopeLock() {
  anlock_unlock(theLock);
}

ScopeCriticalLock::ScopeCriticalLock(anlock_t lock) : theLock(lock) {
  assert(!((uint64_t)lock & 7));
  AssertCritical();
  anlock_lock(theLock);
}

ScopeCriticalLock::~ScopeCriticalLock() {
  anlock_unlock(theLock);
}

}
