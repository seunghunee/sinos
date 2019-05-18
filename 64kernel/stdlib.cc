#include "stdlib.h"
#include "ctype.h"
#include "string.h"

int atoi(const char* str)
{
  while (isblank(*str))
    str++;

  bool is_negative = false;
  if (*str == '-') {
    is_negative = true;
    str++;
  }

  int converted_int = 0;
  while (isdigit(*str)) {
    converted_int *= 10;
    converted_int += *str++ - '0';
  }

  return (is_negative) ? -converted_int : converted_int;
}

/* 한 자릿수를 해당하는 ASCII코드값으로 변환한다.
   특징은 10진수를 넘는 값일 경우 알파벳 'A'이상에 할당한다. */
inline char ntoa(int num)
{
  return (num < 10) ? num + '0' : num - 10 + 'A';
}

int itoa(int value, char* str, int base)
{
  if (value == 0) {
    str[0] = '0';
    str[1] = '\0';
    return 1;
  }

  int i = 0;
  if (value < 0) {
    str[i++] = '-';
    value = -value;
  }

  return i + utoa(value, str + i, base);
}

int utoa(u64_t value, char* str, int base)
{
  if (value == 0) {
    str[0] = '0';
    str[1] = '\0';
    return 1;
  }

  int i = 0;
  while (value != 0) {
    str[i++] = ntoa(value % base);
    value /= base;
  }
  str[i] = '\0';

  strrev(str);

  return i;
}

int dtoa(double value, char* str)
{
  int i = 0;
  for (int j = 1000000; j > 1; j /= 10)
    str[i++] = ntoa((u64_t)(value * j) % 10);
  str[i++] = '.';
  do {
    str[i++] = ntoa((u64_t)value % 10);
    value /= 10;
  } while ((u64_t)value != 0);
  str[i] = '\0';

  strrev(str);

  return i;
}
