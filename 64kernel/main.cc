#include "types.h"
#include "keyboard.h"
#include "descriptor.h"
#include "isr.h"
#include "pic.h"
#include "pit.h"
#include "flags.h"
#include "console.h"
#include "shell.h"
#include "time.h"
#include "scheduler.h"
#include "memmap.h"

int main()
{
  con.init();
  con.gotoxy(0, 9);
  con.puts("64bit C++ kernel is operational.\n");
  
  con.puts("Initializing GDT...\n");
  map.GDT = 0x700000;
  GDTManager gdt((u64_t*)map.GDT);
  // 32bit mode의 code & data segment descriptor 자리채움
  gdt.setSegDesMem();
  gdt.setSegDesMem();
  // 64bit mode의 kernel code & data segment descrpitor
  gdt.setSegDesMem(kGDTEntryFlags_SinosCode);
  gdt.setSegDesMem(kGDTEntryFlags_SinosData);

  con.puts("Initializing TSS and TSS descriptor...\n");
  TSS tss = {0};
  map.IST = map.GDT;
  tss.IST[0] = map.IST;
  tss.IOmap_base_addr = 0xFFFF;		     // No use IOmap
  gdt.setTSSDesMemAndTSSMem(kGDTEntryFlags_SinosTSS, tss);

  con.puts("Loading GDTR...\n");
  gdt.loadGDTR();

  con.puts("Loading TR...\n");
  gdt.loadTR();

  con.puts("Initializing IDT...\n");
  map.IDT = gdt.getTSSBaseAddr() + sizeof(TSS);
  IDTManager idt((u64_t*)map.IDT);
  ISRContainer isr_container;
  for (int i = 0; i < kNumOfISR; i++)
    idt.setGateDesMem(kIDTEntryFlags_SinosGate, 8*3, isr_container[i]);

  con.puts("Loading IDTR...\n");
  idt.loadIDTR();

  con.puts("Initializing PIC...\n");
  initPIC();
  maskIRQ();

  con.puts("Enabling Interrupt...\n");
  setIF(true);
  
  con.puts("Initializing scheduler...\n");
  map.task_pool = 0x800000;
  sched.init((Task*)map.task_pool);

  kbd.init();
  Shell ssh;
  ssh.start();

  return 0;
}
