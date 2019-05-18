#ifndef SINOS_64KERNEL_SCHEDULER_H
#define SINOS_64KERNEL_SCHEDULER_H

#include "types.h"
#include "list.h"
#include "task.h"
#include "flags.h"

const int kMaxNumTasks = 1024;
const int kTaskStackSize = 1024 * 8; // 8KB
const int kNumOfLevels = 5;

class Scheduler
{
protected:
  DISALLOW_COPY_AND_ASSIGN(Scheduler);

  // Task/Stack pool
  TaskPool task_pool_;
  u64_t stack_pool_addr_;

  // Multi level queue
  List<Task*, kMaxNumTasks> task_list_[kNumOfLevels];
  u32_t executed_count_[kNumOfLevels];	// by level
  int cur_lev_;
  int total_tasks;

  u64_t exFxUserId;

  void popTask(int lev) {task_list_[lev].pop_front();}
  void pushTask(Task* task, int lev) {task_list_[lev].push_back(task);}
  Task* removeTaskFromQueue(u16_t no);
  void killTask(Task* task);

  // executed_count_를 이용해 다음 level을 리턴
  int nextLev();

public:
  Scheduler() {}
  void init(Task* task_pool_addr, int max_num_tasks = kMaxNumTasks);

  // Schedule
  /* void switchTask(); */
  void switchTaskIntrpt();
  bool newTask(u64_t entry_point, u8_t flags,
	       u64_t* mem_addr, u64_t mem_size,
	       int priority = 3); // default: Normal
  int deleteTask(u16_t target_task_no);
  Task* currentTask() const {return task_list_[cur_lev_].front();}
  Task* searchTask(u64_t id) {return task_pool_.searchTCB(id);}

  u64_t getExFxUserId() {return exFxUserId;}
  void setExFxUserId(u64_t id) {exFxUserId = id;}

  Task* getTaskInfo() const;
  int size() const {return total_tasks;}
};

extern Scheduler sched;
int exitTask();

#endif // SINOS_64KERNEL_SCHEDULER_H
