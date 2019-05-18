#ifndef SINOS_64KERNEL_TASK_H
#define SINOS_64KERNEL_TASK_H

#include "types.h"
#include "list.h"

const int	kMaxNumThreads = 32;

const u8_t	kOffsetSS     = 23;
const u8_t	kOffsetRSP    = 22;
const u8_t	kOffsetRFLAGS = 21;
const u8_t	kOffsetCS     = 20;
const u8_t	kOffsetRIP    = 19;
const u8_t	kOffsetRBP    = 18;
const u8_t	kOffsetRAX    = 17;
const u8_t	kOffsetRBX    = 16;
const u8_t	kOffsetRCX    = 15;
const u8_t	kOffsetRDX    = 14;
const u8_t	kOffsetRDI    = 13;
const u8_t	kOffsetRSI    = 12;
const u8_t	kOffsetR8     = 11;
const u8_t	kOffsetR9     = 10;
const u8_t	kOffsetR10    = 9;
const u8_t	kOffsetR11    = 8;
const u8_t	kOffsetR12    = 7;
const u8_t	kOffsetR13    = 6;
const u8_t	kOffsetR14    = 5;
const u8_t	kOffsetR15    = 4;
const u8_t	kOffsetDS     = 3;
const u8_t	kOffsetES     = 2;
const u8_t	kOffsetFS     = 1;
const u8_t	kOffsetGS     = 0;

const u8_t	kFlagSystem = 0x80;
const u8_t	kFlagThread = 0x40;
const u8_t	kFlagDelete = 0x01;

#pragma pack(push, 16)		// for FPU

class Task // This is TCB (Task Control Block)
{
protected:
  DISALLOW_COPY_AND_ASSIGN(Task);

  u64_t context_[24];
  u8_t FxContext[512];
  bool isUsingFx_;
  u64_t* stack_addr_;
  u64_t stack_size_;
  u64_t* mem_addr_;
  u64_t mem_size_;
  u64_t id_; /* bit 32 - bit 63 (32bit): Encryption key
		-> if this is 0, this task is not allocated.
		bit 24 - bit 31 (8bit): Flags
		                    31: System process
				    30: Thread
				    (31 is 0, 30 is 0 -> general process)
				    24: Delete
  		bit 16 - bit 23 (8bit): Priority
  		bit 0  - bit 15 (16bit): Task number
  	      */
  // Priority level : 1(Highest), 2(High), 3(Nomal), 4(Low), 5(Lowest)

  u64_t PPID_;			// Parent Process ID
  List<Task*, kMaxNumThreads> thread_list_;

  // Thread data
  void saveContext();
  void loadContext();

  void commitPPIDOfThreads()
  {
    thread_list_.begin();
    for (int i = 0; i < thread_list_.size(); i++) {
      (*thread_list_)->setPPID(id_);
      ++thread_list_;
    }
  }

public:
  Task() {}
  void init(u64_t* stack_addr, u64_t stack_size, u64_t entry_point,
	    u64_t* mem_addr, u64_t mem_size);

  // Access
  u64_t getID() const {return id_;}
  u16_t getNo() const {return id_;}
  u32_t getKey() const {return id_ >> 32;}
  u8_t getPriority() const {return id_ >> 16;}
  u8_t getFlags() const {return id_ >> 24;}
  u64_t getPPID() {return (getFlags() & kFlagThread) ? PPID_ : id_;}

  u64_t setID(u64_t id) {id_ = id;}
  void setNo(u16_t no) {id_ = no;}
  void setKey(u32_t key) {id_ = (id_ & 0x00000000FFFFFFFF) | (u64_t)key << 32;}
  void setPriority(u8_t pri) {
    if (pri <= 5)
      id_ = (u32_t)pri << 16 | (id_ & 0xFFFFFFFFFF00FFFF);
    commitPPIDOfThreads();
  }
  void setFlags(u8_t f) {id_ = (id_ & 0xFFFFFFFF00FFFFFF) | (u64_t)f << 24;}
  void setPPID(u64_t PPID) {PPID_ = PPID;}
  
  void addThread(Task* thread) {thread_list_.push_back(thread);}
  void rmThread(Task* thread);
  u32_t numOfThread() {return thread_list_.size();}
  void cleanThread()		// Set delete flags of all threads
  {
    thread_list_.begin();
    for (int i = 0; i < thread_list_.size(); i++) {
      (*thread_list_)->setDeleteFlag();
      ++thread_list_;
    }
  }

  bool isSystem() {return getFlags() & kFlagSystem;}
  bool isAllocated() {return (getKey() == 0) ? false : true;}
  
  void setDeleteFlag() {setFlags(getFlags() | kFlagDelete);commitPPIDOfThreads();}
  bool getDeleteFlag() {return (getFlags() & kFlagDelete) ? true : false;}

  static void switchContext(Task& to, Task& from);

  bool isUsingFx() {return isUsingFx_;}
  void useFx() {isUsingFx_ = true;}
  void fxInit() {asm volatile ("finit");}
  void fxSave() {asm volatile ("fxsave %0" :: "m" (FxContext));}
  void fxRstor() {asm volatile ("fxrstor %0" :: "m" (FxContext));}
};

#pragma pack(pop)

class TaskPool
{
protected:
  DISALLOW_COPY_AND_ASSIGN(TaskPool);

  Task* base_addr_;
  int max_num_tasks_;
  
  int num_tasks_;
  u32_t count_for_encryption_;
  
public:
  TaskPool() {}
  void init(Task* base_addr, int max_num_tasks);

  Task* allocateTCB(int priority);
  Task* searchTCB(u64_t id);
  void freeTCB(Task* target_tcb);
};

#endif // SINOS_64KERNEL_TASK_H
