#include "mutex.h"
#include "scheduler.h"

bool Mutex::isLocker()
{
  if (lock_count_ == 0)
    return false;
  return (task_id_ == sched.currentTask()->getID()) ? true : false;
}

bool Mutex::isLockedIfNotLock()
{
  // 이상하게도 14line에 rax설정하는 부분을 지우면 리턴값은 rbx로되어 rax설정
  u64_t rax;
  asm volatile ("movq $0, %%rax\n\t"
		"lock; cmpxchgb %%dl, %0\n\t"
		: "+b" (lock_count_), "=a" (rax)
		: "d" (1));
  return (rax == 0) ? false : true;
}

void Mutex::lock()
{
  if (isLockedIfNotLock()) {
    if (isLocker()) {
      ++lock_count_;
      return;
    }

    while (isLockedIfNotLock()) {}
  }

  task_id_ = sched.currentTask()->getID();
}

void Mutex::unLock()
{
  if (!isLocker())
    return;

  --lock_count_;
}
