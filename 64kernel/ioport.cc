#include "ioport.h"

void outb(u8_t data, u16_t port)
{
  asm volatile ("outb %%al, %%dx\n\t"
		:
		: "a" (data), "d" (port));
}

u8_t inb(u16_t port)
{
  asm volatile ("inb %%dx, %%al\n\t"
		:
		: "d" (port));
}
