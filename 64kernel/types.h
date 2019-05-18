#ifndef SINOS_64KERNEL_TYPES_H
#define SINOS_64KERNEL_TYPES_H

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long u64_t;

typedef void (*intr_handler_t)(u8_t vector_no, u64_t error_code);
typedef void (*ISR_t)(void);

#define NULL 0

#define DISALLOW_COPY_AND_ASSIGN(ClassName)	\
  ClassName(const ClassName&);			\
  void operator=(const ClassName&)

#endif // SINOS_64KERNEL_TYPES_H
