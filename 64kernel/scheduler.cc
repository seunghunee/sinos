#include "scheduler.h"
#include "string.h"
#include "memmap.h"
#include "pit.h"

Scheduler sched;
int exitTask() {return sched.deleteTask(sched.currentTask()->getNo());}
#include "console.h"
void Scheduler::init(Task* task_pool_addr, int max_num_tasks)
{
  // Initialize task/stack pool
  task_pool_.init(task_pool_addr, max_num_tasks);
  stack_pool_addr_ = (u64_t)task_pool_addr + sizeof(Task) * max_num_tasks;
  
  // Initialize members
  memset(executed_count_, 0, sizeof(executed_count_));
  total_tasks = 1;
  
  // Allocate TCB for shell task
  // shell task는 booting task이기 때문에, 테스크 전환시 스텍영역이 TCB에 저장된다.
  // 따라서, task.init()을 따로 호출하지 않는다.
  cur_lev_ = 0; // shell is the higshest level
  con.puts("start\n");
  Task* shell_task = task_pool_.allocateTCB(cur_lev_ + 1);
  con.puts("end\n");
  shell_task->setFlags(kFlagSystem);
  pushTask(shell_task, cur_lev_); //cur_lev_ + 1 == priority
  // set PIT speed(왜 이 코드를 썼는지 이유를 쓰는 게 타당)
  setPIT(500);
}

void Scheduler::switchTaskIntrpt()
{
  if (total_tasks <= 1)
    return;
  
  Task* current_task = currentTask();
  popTask(cur_lev_);
  if (current_task->getDeleteFlag() == true && current_task->numOfThread() == 0) {
    task_pool_.freeTCB(current_task);
    --total_tasks;
  }
  else {
    pushTask(current_task, cur_lev_);
  }

  cur_lev_ = nextLev();
  Task* next_task = currentTask();

  if (exFxUserId == next_task->getID())
    clearTS();
  else
    setTS();

  memcpy(current_task, (u64_t*)map.IST - 24, 24 * 8);
  memcpy((u64_t*)map.IST - 24, next_task, 24 * 8);
}

int Scheduler::nextLev()
{
  ++executed_count_[cur_lev_];
  
  for (int i = 0; i < kNumOfLevels; ++i) {
    if (executed_count_[i] < task_list_[i].size())
      return i;
    executed_count_[i] = 0;
  }

  return 0; // Higest level
}

bool Scheduler::newTask(u64_t entry_point, u8_t flags,
			u64_t* mem_addr, u64_t mem_size, int priority)
{
  // entry CS
  bool previous_IF = getIF();
  setIF(false);
  
  Task* task = task_pool_.allocateTCB(priority);
  task->setFlags(flags);
  task->setPPID(task->getID());
  if (task == NULL) {
    setIF(previous_IF);
    return false;
  }

  //                 base address     + (stack size    * offset)
  u64_t stack_addr = stack_pool_addr_ + kTaskStackSize * task->getNo();
  task->init((u64_t*)stack_addr, kTaskStackSize, entry_point,
	     mem_addr, mem_size);

  if (flags & kFlagThread) {
    task->setPPID(currentTask()->getPPID());
    task_pool_.searchTCB(currentTask()->getPPID())->addThread(task);
  }

  ++total_tasks;
  pushTask(task, priority - 1);	// priority - 1 == queue level

  // exit CS
  setIF(previous_IF);
  return true;
}

Task* Scheduler::getTaskInfo() const
{
  static int i = 0;
  while (1) {
    if (i >= kMaxNumTasks)
      i = 0;
    if ( ((Task*)map.task_pool)[i++].isAllocated() )
      return (Task*)map.task_pool + i - 1;
  }
}

int Scheduler::deleteTask(u16_t target_task_no)
{
  // entry CS
  bool previous_IF = getIF();
  setIF(false);
  // case: running task == target task
  if (currentTask()->getNo() == target_task_no) {
    currentTask()->setDeleteFlag();
    currentTask()->cleanThread();
    // exit CS
    setIF(previous_IF);
    while (1) {} // waiting interrupt
  }

  // find out target task
  Task* target_task = removeTaskFromQueue(target_task_no);
  if (target_task == NULL) {
    // exit CS
    setIF(previous_IF);
    return -1;
  }

  if (target_task->numOfThread() != 0) {
    target_task->setDeleteFlag();
    target_task->cleanThread();
    pushTask(target_task, target_task->getPriority() - 1);
  }
  else {
    task_pool_.freeTCB(target_task);
    --total_tasks;
  }
  
  // exit CS
  setIF(previous_IF);

  return 0;
}

Task* Scheduler::removeTaskFromQueue(u16_t no)
{
  for (int i = 0; i < kNumOfLevels; ++i) {
    task_list_[i].begin();
    for (int j = 0; j < task_list_[i].size(); ++j) {
      if ((*task_list_[i])->getNo() == no) {
      	Task* removed_task = *task_list_[i];
      	task_list_[i].remove();
      	return removed_task; 
      }
      ++task_list_[i];
    }
  }

  return NULL;
}

// void Scheduler::switchTask()
// {
//   if (task_list_[cur_lev_].size() <= 1)
//     return;

//   /* 만약 인터럽트가 context switching전에 발생하면
//      엉뚱한 TCB에 context를 덮어 쓸 수 있기 때문에 */
//   bool previous_IF = getIF();
//   setIF(false);

//   Task* current_task = currentTask();
//   popTask(cur_lev_);
//   pushTask(current_task, cur_lev_);

//   cur_lev_ = nextLev();
//   Task* next_task = currentTask();

//   Task::switchContext(*next_task, *current_task);
  
//   setIF(previous_IF);
// }
