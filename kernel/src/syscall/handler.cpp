#include "handler.hpp"
#include "console.hpp"
#include <anarch/stream>
#include <anarch/critical>

namespace Alux {

anarch::SyscallRet SyscallHandler(uint16_t number,
                                  anarch::SyscallArgs & args) {
  AssertCritical();
  switch (number) {
    case 0:
      PrintSyscall(args);
      break;
    default:
      anarch::cerr << "unknown StandardSyscallHandler(" << number << ", ...)"
        << anarch::endl;
      break;
  }
  return anarch::SyscallRet();
}

}