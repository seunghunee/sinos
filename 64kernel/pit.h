#ifndef SINOS_64KERNEL_PIT_H
#define SINOS_64KERNEL_PIT_H
// Programmable Interval Timer
#include "types.h"

const u32_t PITClockPerSec = 1193182;

// millisecond -> count
inline u16_t mstoc(u32_t mili) {return mili * PITClockPerSec / 1000;}
// microsecond -> count
inline u16_t ustoc(u32_t mili) {return mili * PITClockPerSec / 1000000;}

void setPIT(u16_t interval, bool isPeriodic = true);
u16_t getPITCount();
void delay50ms(u16_t count);	// max delay 50ms
void delay(u32_t milliseconds);

#endif // SINOS_64KERNEL_PIT_H
