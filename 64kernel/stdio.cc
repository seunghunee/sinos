#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int vsprintf(char* buffer, const char* format, va_list ap)
{
  int i = 0;
  
  for (; *format; format++) {
    if (*format == '%')
      switch (*++format) {
      case 'd':
      case 'i':
	i += itoa(va_arg(ap, int), buffer + i, 10);
	break;
      case 'o':
	i += utoa(va_arg(ap, int), buffer + i, 8);
	break;
      case 'u':
	i += utoa(va_arg(ap, u64_t), buffer + i, 10);
	break;
      case 'x':
	{
	  int start_i = i;
	  i += utoa(va_arg(ap, u64_t), buffer + i, 16);
	  strlwr(buffer + start_i);
	  break;
	}
      case 'X':
	i += utoa(va_arg(ap, u64_t), buffer + i, 16);
	break;
      case 'c':
	buffer[i++] = va_arg(ap, int);
	break;
      case 's':
	{
	  char* src = va_arg(ap, char*);
	  strcpy(buffer + i, src);
	  i += strlen(src);
	  break;
	}
      case 'f':
	i += dtoa(va_arg(ap, double), buffer + i);
	break;
      }
    else
      buffer[i++] = *format;
  }

  buffer[i] = '\0';
  return i;
}

int sprintf(char* buffer, const char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  int len = vsprintf(buffer, format, ap);
  va_end(ap);

  return len;
}
