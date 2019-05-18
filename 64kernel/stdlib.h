#ifndef SINOS_64KERNEL_STDLIB_H
#define SINOS_64KERNEL_STDLIB_H

#include "types.h"

int atoi(const char* str);
int itoa(int value, char* str, int base);
int utoa(u64_t value, char* str, int base);
int dtoa(double value, char* str);

#endif // SINOS_64KERNEL_STDLIB_H
