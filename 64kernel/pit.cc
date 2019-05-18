#include "pit.h"
#include "ioport.h"

const u8_t	kMode0 = 0x00;
const u8_t	kMode2 = 0x04;

const u8_t	kLatch	   = 0x00;
const u8_t	kLowerByte = 0x10;
const u8_t	kUpperByte = 0x20;
const u8_t	k2Bytes	   = 0x30;

const u8_t	kChannel0 = 0x00;
const u8_t	kChannel1 = 0x40;
const u8_t	kChannel2 = 0x80;

void setPIT(u16_t interval, bool isPeriodic)
{
  if (isPeriodic)
    outb(kMode2 | k2Bytes | kChannel0, kPITCtrl);
  else
    outb(kMode0 | k2Bytes | kChannel0, kPITCtrl);

  outb(interval, kPITChannel0);
  outb(interval >> 8, kPITChannel0);
}

u16_t getPITCount()
{
  outb(kLatch, kPITCtrl);

  u16_t lower_count = inb(kPITChannel0);
  u16_t upper_count = inb(kPITChannel0);

  return lower_count | upper_count << 8;
}

void delay50ms(u16_t count)
{
  setPIT(0);			// set interval 0x10000
  u16_t last_count = getPITCount();
  while (1) {
    u16_t diff_count = last_count - getPITCount();
    if (diff_count >= count)
      break;
  }
}

void delay(u32_t milliseconds)
{
  int repeat = milliseconds / 50;
  for (int i = 0; i < repeat; i++)
    delay50ms(mstoc(50));
  delay50ms(mstoc(milliseconds % 50));
}
