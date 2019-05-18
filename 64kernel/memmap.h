#ifndef SINOS_64KERNEL_MEMMAP_H
#define SINOS_64KERNEL_MEMMAP_H

#include "types.h"

// stack address는 bsp에 대입될 값임에 주의!
struct Map
{
  u64_t GDT;
  u64_t IDT;
  u64_t IST;		// stack
  u64_t task_pool;
};

extern Map map;

#endif // SINOS_64KERNEL_MEMMAP_H
