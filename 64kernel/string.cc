#include "string.h"
#include "ctype.h"

void* memset(void* ptr, int value, u32_t num)
{
  for (u32_t i = 0; i < num; i++)
    ((u8_t*)ptr)[i] = value;

  return ptr;
}

void* memcpy(void* dest, const void* src, u32_t num)
{
  for (u32_t i = 0; i < num; i++)
    ((u8_t*)dest)[i] = ((u8_t*)src)[i];

  return dest;
}

char* strcpy(char* dest, const char* src)
{
  char* d = dest;
  while (*dest++ = *src++) ;
  return d;
}

char* strncpy(char* dest, const char* src, u32_t len)
{
  for (int i = 0; i < len; i++)
    dest[i] = src[i];
  return dest;
}

int strcmp(const char* s1, const char* s2)
{
  for (; *s1 == *s2; s1++, s2++) {
    if (*s1 == '\0')
      break;
  }

  return *s1 - *s2;
}

u32_t strlen(const char* str)
{
  u32_t len = 0;
  while (*str++)
    len++;

  return len;
}

char* strlwr(char* str)
{
  while (*str) {
    *str = tolower(*str);
    str++;
  }

  return str;
}

char* strupr(char* str)
{
  while (*str) {
    *str = toupper(*str);
    str++;
  }

  return str;
}

char* strrev(char* str)
{
  return strnrev(str, strlen(str));
}

char* strnrev(char* str, int len)
{
  for (int i = 0; i < len/2; i++) {
    char tmp = str[i];
    str[i] = str[len - 1 - i];
    str[len - 1 - i] = tmp;
  }

  return str;
}
