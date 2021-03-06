#ifndef __ALUX_HOLD_SCOPE_HPP__
#define __ALUX_HOLD_SCOPE_HPP__

#include "user-task.hpp"

namespace Alux {

/**
 * This is the only means by which you should leave a critical section.
 */
class HoldScope {
public:
  /**
   * Attempts to hold the current task if the current thread is not already
   * holding it. If the task cannot be held, the thread is exited and this will
   * never return.
   * @ambicritical (x) -> @noncritical
   */
  HoldScope();
  
  /**
   * Leave the noncritical section if this hold scope put us in it.
   * @noncritical -> @ambicritical (x)
   */
  ~HoldScope();
  
  inline Task & GetTask() {
    return *task;
  }
  
  inline Thread & GetThread() {
    return *thread;
  }
  
  inline UserTask & GetUserTask() {
    assert(GetTask().IsUserTask());
    return static_cast<UserTask &>(GetTask());
  }
  
  /**
   * Exit the current thread and unhold the current task.
   * @ambicritical
   */
  void ExitThread() ANSA_NORETURN;
  
  /**
   * Exit the current task and unhold it.
   * @ambicritical
   */
  void ExitTask(int status) ANSA_NORETURN;
  
private:
  bool wasCritical;
  bool didHold;
  Task * task;
  Thread * thread;
};

}

#endif
