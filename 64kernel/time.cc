#include "time.h"
#include "ioport.h"

const u8_t	kRTCSec	 = 0x00;
const u8_t	kRTCMin	 = 0x02;
const u8_t	kRTCHour = 0x04;

const u8_t	kRTCDayOfWeek  = 0x06;
const u8_t	kRTCDayOfMonth = 0x07;
const u8_t	kRTCMonth      = 0x08;
const u8_t	kRTCYear       = 0x09;

const char day[][4] = {"Non", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char* dayOfWeekToStr(u8_t num_day)
{
  return day[num_day % 8];
}

inline u8_t bcdToBin(u8_t bcd)
{
  return (bcd & 0xF) + (bcd >> 4) * 10;
}

time gettime()
{
  time current_time;
  
  outb(kRTCHour, kCMOSAddr);
  current_time.hour = bcdToBin(inb(kCMOSData));
  outb(kRTCMin, kCMOSAddr);
  current_time.min = bcdToBin(inb(kCMOSData));
  outb(kRTCSec, kCMOSAddr);
  current_time.sec = bcdToBin(inb(kCMOSData));

  return current_time;
}

date getdate()
{
  date current_date;

  outb(kRTCYear, kCMOSAddr);
  current_date.year = bcdToBin(inb(kCMOSData)) + 2000;
  outb(kRTCMonth, kCMOSAddr);
  current_date.month = bcdToBin(inb(kCMOSData));
  outb(kRTCDayOfMonth, kCMOSAddr);
  current_date.day_of_month = bcdToBin(inb(kCMOSData));
  outb(kRTCDayOfWeek, kCMOSAddr);
  current_date.day_of_week = bcdToBin(inb(kCMOSData));

  return current_date;
}

u64_t rdtsc()
{
  asm volatile ("rdtsc\n\t"
		"shlq $32, %rdx\n\t"
		"orq %rdx, %rax\n\t");
}
