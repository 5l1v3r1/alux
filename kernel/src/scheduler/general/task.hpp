#ifndef __SCHEDULER_TASK_HPP__
#define __SCHEDULER_TASK_HPP__

#include <scheduler-specific/task.hpp>
#include <scheduler/internal/garbage-thread.hpp>
#include <utilities/index-set.hpp>
#include <cstdint>
#include <macros>

namespace OS {

class Thread;
class PIDPool;
class UserMap;
class AddressSpace;

class Task : public SchedTask, public GarbageThread::Garbage {
public:
  static Task * New(bool forKernel); // @noncritical
  
  static void InitializeCounter();
  static uint64_t GetCounter();
  
  static void Exit(uint64_t status) OS_NORETURN; // @critical
  
  virtual ~Task(); // @noncritical
  virtual void Delete(); // @noncritical
  virtual bool IsKernel();
  
  void AddThread(Thread * th); // @noncritical
  void RemoveThread(Thread * th); // @noncritical
  
  // TODO: get rid of this in favor of returning a referenced Thread
  void UnsleepThreadById(uint64_t ident); // @noncritical
  
  bool Retain(); // @ambicritical
  void Release(); // @ambicritical
  bool Hold(); // @ambicritical
  void Unhold(); // @ambicritical
  void Kill(uint64_t status); // @ambicritical

  uint64_t GetPID();
  
  uint64_t GetUID();
  void SetUID(uint64_t);
  
  AddressSpace & GetAddressSpace();
  UserMap * GetUserAddressSpace();

protected:
  Task * pidPoolNext, * pidPoolLast;
  friend class PIDPool;
  
  Task(bool forKernel);
  virtual void CleanupGarbage();

private:
  uint64_t pid;
  uint64_t uid = 0;
  bool isKernel;
  
  uint64_t threadsLock OS_ALIGNED(8) = 0; // @noncritical
  Thread * firstThread = NULL;
  
  IndexSet<0x10> threadIds;
  IndexSet<0x20> descriptors;
  
  uint64_t stateLock OS_ALIGNED(8) = 0; // @critical
  uint64_t retainCount = 0;
  uint64_t holdCount = 1;
  uint64_t killStatus;
  bool isKilled = false;
  
  UserMap * userSpace;
  
  void Terminate(); // @critical
};

}

#endif
