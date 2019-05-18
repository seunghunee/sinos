#include "task.h"
#include "scheduler.h"

void Task::init(u64_t* stack_addr, u64_t stack_size, u64_t entry_point,
		u64_t* mem_addr, u64_t mem_size)
{
  stack_addr_ = stack_addr;
  stack_size_ = stack_size;
  mem_addr_ = mem_addr;
  mem_size_ = mem_size;
  isUsingFx_ = false;

  // Set segment selector
  context_[kOffsetSS] = 32;
  context_[kOffsetCS] = 24;
  context_[kOffsetDS] = 32;
  context_[kOffsetES] = 32;
  context_[kOffsetFS] = 32;
  context_[kOffsetGS] = 32;

  // Set RBP, RSP, RIP
  u64_t top_of_stack = (u64_t)stack_addr + stack_size - 8;
  context_[kOffsetRBP] = context_[kOffsetRSP] = top_of_stack;
  *(u64_t*)top_of_stack = (u64_t)exitTask;
  context_[kOffsetRIP] = entry_point;

  // Set RFLAGS, interrupt ON
  context_[kOffsetRFLAGS] = 0x200;
}

void Task::switchContext(Task& to, Task& from)
{
  from.saveContext();
  to.loadContext();
}

void Task::saveContext()
{
  /* 함수를 호출함으로 인해 RIP, RBP, RSP의 값이 변한다.
     따라서, context를 저장하기 전에 switchContext와 saveContext함수
     호출 전의 값을 context_에 저장해야 한다.
     offset은 g++ -S으로 확인, rax는 base addr로 사용되어 rbx사용 */
  asm volatile ("movq $0, %%rbx\n\t"
		"movw %%ss, %%bx\n\t"
  		"movq %%rbx, %0\n\t"
		
  		"movq %%rbp, %%rbx\n\t"
		"addq $48, %%rbx\n\t"
  		"movq %%rbx, %1\n\t"
		
  		"pushfq\n\t"
  		"pop %%rbx\n\t"
  		"movq %%rbx, %2\n\t"
		
		"movq $0, %%rbx\n\t"
		"movw %%cs, %%bx\n\t"
  		"movq %%rbx, %3\n\t"
		
  		"movq 40(%%rbp), %%rbx\n\t"
  		"movq %%rbx, %4\n\t"

  		"movq 32(%%rbp), %%rbx\n\t"
  		"movq %%rbx, %5\n\t"
  		: "=m" (context_[kOffsetSS]),
  		  "=m" (context_[kOffsetRSP]),
  		  "=m" (context_[kOffsetRFLAGS]),
  		  "=m" (context_[kOffsetCS]),
  		  "=m" (context_[kOffsetRIP]),
		  "=m" (context_[kOffsetRBP]));
}

void Task::loadContext()
{
  asm volatile ("movq %0, %%rbp\n\t"
		"movq %1, %%rsp\n\t"
  		"iretq\n\t"
  		:
  		: "m" (context_[kOffsetRBP]),
		  "r" (context_ + 19));
}

void Task::rmThread(Task* thread)
{
  thread_list_.begin();
  for (int i = 0; i < thread_list_.size(); i++) {
    if ((*thread_list_)->getID() == thread->getID()) {
      thread_list_.remove();
      return;
    }
    ++thread_list_;
  }
}

void TaskPool::init(Task* base_addr, int max_num_tasks)
{
  base_addr_ = base_addr;
  max_num_tasks_ = max_num_tasks;
  num_tasks_ = 0;
  count_for_encryption_ = 1;

  for (int i = 0; i < max_num_tasks; i++)
    base_addr_[i].setNo(i);
}
#include "console.h"
Task* TaskPool::allocateTCB(int priority)
{
  if (num_tasks_ >= max_num_tasks_)
    return NULL;
  con.puts("1");
  Task* empty_tcb_entry;
  for (int i = 0; i < max_num_tasks_; i++) {
    if (base_addr_[i].isAllocated() == false) {
      empty_tcb_entry = base_addr_ + i;
      break;
    }
  }
  con.puts("2");
  empty_tcb_entry->setKey(count_for_encryption_);
    con.puts("3");
  empty_tcb_entry->setPriority((u8_t)priority);
    con.puts("4");

  num_tasks_++;
  if (++count_for_encryption_ == 0)
    count_for_encryption_++;
  con.puts("5");
  return empty_tcb_entry;
}

void TaskPool::freeTCB(Task* target_tcb)
{
  if (target_tcb->getFlags() & kFlagThread){
    searchTCB(target_tcb->getPPID())->rmThread(target_tcb);
  }
  target_tcb->setKey(0);
  num_tasks_--;
}

Task* TaskPool::searchTCB(u64_t id)
{
  for (int i = 0; i < max_num_tasks_; i++)
    if (base_addr_[i].isAllocated() && base_addr_[i].getID() == id)
      return base_addr_ + i;
  return NULL;
}
