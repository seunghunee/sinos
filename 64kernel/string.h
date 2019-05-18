#ifndef SINOS_64KERNEL_MEM_H
#define SINOS_64KERNEL_MEM_H

#include "types.h"

void* memset(void* s, int c, u32_t n);
void* memcpy(void* dest, const void* src, u32_t num);

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, u32_t len);
int strcmp(const char* s1, const char* s2);
u32_t strlen(const char* str);
char* strlwr(char* str);
char* strupr(char* str);

/* str부터 시작하는 문자열을 뒤집어 저장하는 함수 */
char* strrev(char* str);
char* strnrev(char* str, int len);

#endif // SINOS_64KERNEL_MEM_H
