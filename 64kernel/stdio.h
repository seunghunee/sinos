#ifndef SINOS_64KERNEL_STDIO_H
#define SINOS_64KERNEL_STDIO_H

#include <stdarg.h>

int vsprintf(char* buffer, const char* format, va_list ap);
int sprintf(char* buffer, const char* format, ...);

#endif // SINOS_64KERNEL_STDIO_H
