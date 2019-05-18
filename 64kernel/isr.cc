#include "isr.h"
#include "pic.h"
#include "keyboard.h"
#include "console.h"
#include "scheduler.h"

// Push all 64bit general purpose registers
#define PUSHAQ "pushq %rax\n\t"			\
  "pushq %rbx\n\t"				\
  "pushq %rcx\n\t"				\
  "pushq %rdx\n\t"				\
  "pushq %rsi\n\t"				\
  "pushq %rdi\n\t"				\
  "pushq %r8\n\t"				\
  "pushq %r9\n\t"				\
  "pushq %r10\n\t"				\
  "pushq %r11\n\t"				\
  "pushq %r12\n\t"				\
  "pushq %r13\n\t"				\
  "pushq %r14\n\t"				\
  "pushq %r15\n\t"				\
  "movw %ds, %ax\n\t"				\
  "pushq %rax\n\t"				\
  "movw %es, %ax\n\t"				\
  "pushq %rax\n\t"				\
  "movw %fs, %ax\n\t"				\
  "pushq %rax\n\t"				\
  "movw %gs, %ax\n\t"				\
  "pushq %rax"
// Pop all 64bit general purpose registers
#define POPAQ "popq %rax\n\t"			\
  "movw %ax, %gs\n\t"				\
  "popq %rax\n\t"				\
  "movw %ax, %fs\n\t"				\
  "popq %rax\n\t"				\
  "movw %ax, %es\n\t"				\
  "popq %rax\n\t"				\
  "movw %ax, %ds\n\t"				\
  "popq %r15\n\t"				\
  "popq %r14\n\t"				\
  "popq %r13\n\t"				\
  "popq %r12\n\t"				\
  "popq %r11\n\t"				\
  "popq %r10\n\t"				\
  "popq %r9\n\t"				\
  "popq %r8\n\t"				\
  "popq %rdi\n\t"				\
  "popq %rsi\n\t"				\
  "popq %rdx\n\t"				\
  "popq %rcx\n\t"				\
  "popq %rbx\n\t"				\
  "popq %rax\n\t"				\
  "popq %rbp"
// Change to ring level 0 data selectors
#define EXCHANGE_DATA_SELECTORS "movw $32, %ax\n\t"	\
  "movw %ax, %ds\n\t"					\
  "movw %ax, %es\n\t"					\
  "movw %ax, %fs\n\t"					\
  "movw %ax, %gs"

void commonExceptionHandler(u8_t vector_no, u64_t error_code = 0)
{
  char vector_number[4] = {0,};
  vector_number[0] = vector_no / 100 + '0';
  vector_number[1] = (vector_no / 10) % 10 + '0';
  vector_number[2] = vector_no % 10 + '0';

  printMessage(0, 0, ">>>>>>>>>>>>>>> Exception Occured <<<<<<<<<<<<<<<");
  printMessage(0, 1, "Interrupt vector number: ");
  printMessage(25, 1, vector_number);

  // while (1) ;
}

void commonInterruptHandler(u8_t vector_no, u64_t error_code = 0)
{
  char vector_number[4] = {0,};
  vector_number[0] = vector_no / 100 + '0';
  vector_number[1] = (vector_no / 10) % 10 + '0';
  vector_number[2] = vector_no % 10 + '0';
  printMessage(74, 0, vector_number);
  
  static int count = 0;
  char count_number[2] = {0,};
  count_number[0] = '0' + count++;
  printMessage(78, 0, count_number);
  count %= 10;

  sendEOItoPIC(vector_no - 32);
}

void PITHandler(u8_t vector_no, u64_t error_code = 0)
{
  sched.switchTaskIntrpt();
  sendEOItoPIC(vector_no - 32);
}

void keyboardHandler(u8_t vector_no, u64_t error_code = 0)
{
  kbd.loadData();
  sendEOItoPIC(vector_no - 32);
}

void deviceNotAvailableHandler(u8_t vector_no, u64_t error_code = 0)
{
  clearTS();

  Task* ex_fx_user = sched.searchTask(sched.getExFxUserId());
  if (ex_fx_user != NULL)
    ex_fx_user->fxSave();

  Task* cur_fx_user = sched.currentTask();
  if (cur_fx_user->isUsingFx()) {
    cur_fx_user->fxRstor();
  }
  else {
    cur_fx_user->fxInit();
    cur_fx_user->useFx();
  }

  sched.setExFxUserId(cur_fx_user->getID());
}

intr_handler_t handlers[kNumOfISR] = {commonExceptionHandler, // 0
				      commonExceptionHandler, // 1
				      commonExceptionHandler, // 2
				      commonExceptionHandler, // 3
				      commonExceptionHandler, // 4
				      commonExceptionHandler, // 5
				      commonExceptionHandler, // 6
				      deviceNotAvailableHandler, // 7
				      commonExceptionHandler, // 8
				      commonExceptionHandler, // 9
				      commonExceptionHandler, // 10
				      commonExceptionHandler, // 11
				      commonExceptionHandler, // 12
				      commonExceptionHandler, // 13
				      commonExceptionHandler, // 14
				      0,		      // 15
				      commonExceptionHandler, // 16
				      commonExceptionHandler, // 17
				      commonExceptionHandler, // 18
				      commonExceptionHandler, // 19
				      0,		      // 20
				      0,		      // 21
				      0,		      // 22
				      0,		      // 23
				      0,		      // 24
				      0,		      // 25
				      0,		      // 26
				      0,		      // 27
				      0,		      // 28
				      0,		      // 29
				      0,		      // 30
				      0,		      // 31
				      PITHandler,	      // 32
				      keyboardHandler,	      // 33
				      commonInterruptHandler, // 34
				      commonInterruptHandler, // 35
				      commonInterruptHandler, // 36
				      commonInterruptHandler, // 37
				      commonInterruptHandler, // 38
				      commonInterruptHandler, // 39
				      commonInterruptHandler, // 40
				      commonInterruptHandler, // 41
				      commonInterruptHandler, // 42
				      commonInterruptHandler, // 43
				      commonInterruptHandler, // 44
				      commonInterruptHandler, // 45
				      commonInterruptHandler, // 46
				      commonInterruptHandler, // 47
};

void divideErrorISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[0](0, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void debugExceptionISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[1](1, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");	// 
}

void NMIISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[2](2, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void breakpointISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[3](3, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void overflowISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[4](4, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void boundRangeEsceededISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[5](5, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void invalidOpcodeISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[6](6, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void deviceNotAvailableISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[7](7, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void doubleFaultISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  u64_t error_code;
  asm volatile ("movq -8(%%rbp) ,%0"
		: "=r" (error_code));
  handlers[8](8, error_code);
  asm volatile (POPAQ);
  asm volatile ("add 8, %rsp");
  asm volatile ("iretq");
}

void coprocessorSegmentOverrunISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[9](9, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}
void invalidTSSISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  u64_t error_code;
  asm volatile ("movq -8(%%rbp) ,%0"
		: "=r" (error_code));
  handlers[10](10, error_code);
  asm volatile (POPAQ);
  asm volatile ("add 8, %rsp");
  asm volatile ("iretq");
}

void segmentNotPresentISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  u64_t error_code;
  asm volatile ("movq -8(%%rbp) ,%0"
		: "=r" (error_code));
  handlers[11](11, error_code);
  asm volatile (POPAQ);
  asm volatile ("add 8, %rsp");
  asm volatile ("iretq");
}

void stackSegmentFaultISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  u64_t error_code;
  asm volatile ("movq -8(%%rbp) ,%0"
		: "=r" (error_code));
  handlers[12](12, error_code);
  asm volatile (POPAQ);
  asm volatile ("add 8, %rsp");
  asm volatile ("iretq");
}

void generalProtectionISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  u64_t error_code;
  asm volatile ("movq -8(%%rbp) ,%0"
		: "=r" (error_code));
  handlers[13](13, error_code);
  asm volatile (POPAQ);
  asm volatile ("add 8, %rsp");
  asm volatile ("iretq");
}

void pageFaultISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  u64_t error_code;
  asm volatile ("movq -8(%%rbp) ,%0"
		: "=r" (error_code));
  handlers[14](14, error_code);
  asm volatile (POPAQ);
  asm volatile ("add 8, %rsp");
  asm volatile ("iretq");
}

void FPUFloatingErrorISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[16](16, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void alignmentCheckISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  u64_t error_code;
  asm volatile ("movq -8(%%rbp) ,%0"
		: "=r" (error_code));
  handlers[17](17, error_code);
  asm volatile (POPAQ);
  asm volatile ("add 8, %rsp");
  asm volatile ("iretq");
}

void machineCheckISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[18](18, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void SIMDFloatingErrorISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[19](19, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void timerISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[32](32, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}
  
void keyboardISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[33](33, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void slavePICISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[34](34, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void serial2ISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[35](35, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}


void serial1ISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[36](36, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void parallel2ISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[37](37, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void floppyISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[38](38, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void parallel1ISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[39](39, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void RTCISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[40](40, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void mouseISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[44](44, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void coprocessorISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[45](45, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void hdd1ISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[46](46, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}

void hdd2ISR()
{
  asm volatile (PUSHAQ);
  asm volatile (EXCHANGE_DATA_SELECTORS);
  handlers[47](47, 0);
  asm volatile (POPAQ);
  asm volatile ("iretq");
}
  
ISRContainer::ISRContainer()
{
  ISRs[0] = divideErrorISR;
  ISRs[1] = debugExceptionISR;
  ISRs[2] = NMIISR;		// Nonmaskable Interrupt
  ISRs[3] = breakpointISR;
  ISRs[4] = overflowISR;
  ISRs[5] = boundRangeEsceededISR;
  ISRs[6] = invalidOpcodeISR;
  ISRs[7] = deviceNotAvailableISR;
  ISRs[8] = doubleFaultISR;
  ISRs[9] = coprocessorSegmentOverrunISR;
  ISRs[10] = invalidTSSISR;
  ISRs[11] = segmentNotPresentISR;
  ISRs[12] = stackSegmentFaultISR;
  ISRs[13] = generalProtectionISR;
  ISRs[14] = pageFaultISR;

  ISRs[16] = FPUFloatingErrorISR;
  ISRs[17] = alignmentCheckISR;
  ISRs[18] = machineCheckISR;
  ISRs[19] = SIMDFloatingErrorISR;

  ISRs[32] = timerISR;
  ISRs[33] = keyboardISR;
  ISRs[34] = slavePICISR;
  ISRs[35] = serial2ISR;
  ISRs[36] = serial1ISR;
  ISRs[37] = parallel2ISR;
  ISRs[38] = floppyISR;
  ISRs[39] = parallel1ISR;
  ISRs[40] = RTCISR;
  ISRs[44] = mouseISR;
  ISRs[45] = coprocessorISR;
  ISRs[46] = hdd1ISR;
  ISRs[47] = hdd2ISR;
}

void ISRContainer::setHandler(u8_t vector_no, intr_handler_t handler)
{
  if (vector_no < 32)
    return;
  
  handlers[vector_no] = handler;
}
