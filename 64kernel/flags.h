#ifndef SINOS_64KERNEL_FLAGS_H
#define SINOS_64KERNEL_FLAGS_H

#include "types.h"

inline u64_t saveRFLAGS()
{
  asm volatile ("pushfq\n\t"
		"popq %rax\n\t");
}

// Interrupt Flag : bit 9
inline bool getIF()
{
  return saveRFLAGS() & 0x200;
}

inline void setIF(bool flag_on)
{
  if (flag_on)
    asm volatile ("sti");	// set IF
  else
    asm volatile ("cli");	// clear IF
}

// Clear task-switched flag in CR0
inline void clearTS()
{
  asm volatile ("clts");
}

// Set task-switched flag in CR0
inline void setTS()
{
  asm volatile ("movq %cr0, %rax\n\t"
		"or $8, %rax\n\t"
		"movq %rax, %cr0\n\t");
}

#endif // SINOS_64KERNEL_FLAGS_H
