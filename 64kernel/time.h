#ifndef SINOS_64KERNEL_TIME_H
#define SINOS_64KERNEL_TIME_H

#include "types.h"

struct time
{
  u8_t hour;
  u8_t min;
  u8_t sec;
};

struct date
{
  u16_t year;
  u8_t month;
  u8_t day_of_month;
  u8_t day_of_week;		// Sun ~ Sat = 1 ~ 7
};

time gettime();
date getdate();
const char* dayOfWeekToStr(u8_t num_day);

// Read Time Stamp Counter
u64_t rdtsc();

#endif // SINOS_64KERNEL_TIME_H
