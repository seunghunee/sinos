#ifndef SINOS_64KERNEL_ISR_H
#define SINOS_64KERNEL_ISR_H

#include "types.h"

const u8_t kNumOfISR = 48;

class ISRContainer // Interrupt Service Routines
{
protected:
  DISALLOW_COPY_AND_ASSIGN(ISRContainer);

  ISR_t ISRs[kNumOfISR];

public:
  ISRContainer();

  void setHandler(u8_t vector_no, intr_handler_t handler);
  const ISR_t& operator[](u8_t vector_no) {return ISRs[vector_no];}
  const ISR_t& operator[](u8_t vector_no) const {return ISRs[vector_no];}
};

#endif // SINOS_64KERNEL_ISR_H
