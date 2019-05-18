#include "pic.h"
#include "ioport.h"

void initPIC(u16_t vtr_no_offset)
{
  // Initialize Master PIC
  outb(0x11, kPIC1Command);  	// ICW1
  outb(vtr_no_offset, kPIC1Data);	// ICW2
  outb(0x04, kPIC1Data);    	// ICW3
  outb(0x01, kPIC1Data);    	// ICW4

  // Initialize Slave PIC
  outb(0x11, kPIC2Command);	          // ICW1
  outb(vtr_no_offset + 8, kPIC2Data); // ICW2
  outb(0x02, kPIC2Data);       	  // ICW3
  outb(0x01, kPIC2Data);    	  // ICW4
}

void maskIRQ(u16_t IRQ_bitmask)
{
  outb((u8_t)IRQ_bitmask, kPIC1Data);
  outb((u8_t)(IRQ_bitmask >> 8), kPIC2Data);
}

bool sendEOItoPIC(int IRQ_no)
{
  if (IRQ_no < 0 || IRQ_no > 15)
    return false;

  outb(0x20, kPIC1Command);  // EOI to master PIC

  if (IRQ_no > 7) {
    outb(0x20, kPIC2Command);  // EOI to slave PIC
  }

  return true;
}
