#ifndef SINOS_64KERNEL_MUTEX_H
#define SINOS_64KERNEL_MUTEX_H

#include "types.h"

class Mutex
{
protected:
  DISALLOW_COPY_AND_ASSIGN(Mutex);
public:
  volatile u8_t lock_count_;
  volatile u64_t task_id_;

  bool isLockedIfNotLock();
  bool isLocker();

public:
  Mutex()
    : lock_count_(0), task_id_(0)
  {}

  void lock();
  void unLock();
};

#endif // SINOS_64KERNEL_MUTEX_H
