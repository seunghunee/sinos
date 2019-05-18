#ifndef SINOS_64KERNEL_PIC_H
#define SINOS_64KERNEL_PIC_H
// Programmable Interrupt Controller
#include "types.h"

void initPIC(u16_t vtr_no_offset = 0x20);
void maskIRQ(u16_t IRQ_bitmask = 0);
bool sendEOItoPIC(int IRQ_no);

#endif // SINOS_64KERNEL_PIC_H
