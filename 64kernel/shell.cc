#include "shell.h"
#include "keyboard.h"
#include "string.h"
#include "time.h"
#include "console.h"
#include "task.h"
#include "scheduler.h"
#include "stdlib.h"

// Command
typedef void (*cmd_func)(const char* object_str,
			 const char* option);
struct Command
{
  char keyword[kKeywordLen];
  cmd_func command;
  char info[kInfoLen];
};

// Command function list
void clearCmd(const char*, const char*);
void shutdownCmd(const char*, const char*);
void dateCmd(const char*, const char*);
void mtCmd(const char*, const char*);
void helpCmd(const char*, const char*);
void psCmd(const char*, const char*);
void killCmd(const char*, const char*);

Command cmd_list[] = {
  {"help", helpCmd, "print Sinos shell help"},
  {"clear", clearCmd, "clear the terminal screen"},
  {"shutdown", shutdownCmd, "bring the system down"},
  {"date", dateCmd, "print the system date and time"},
  {"mt", mtCmd, "multitasking test"},
  {"ps", psCmd, "displays information about current tasks"},
  {"kill", killCmd, "kill tasks"},
  {"EndOfCmd", 0, 0},
};

Shell::Shell()
  : input_end_(0), input_at_(0)
{
  strcpy(dir_path_, "/");
  strcpy(prompt_, "Sinos[/]: ");
  memset(input_buf_, 0, sizeof(input_buf_));
}

void Shell::start()
{
  con.puts(" >> Welcome to Sinos <<\n");
  con.printf(prompt_);

  bool isCtrlPressed = false;
  for (u8_t key; key = kbd.getASKEY(); kbd.update()) {
    // shortcut을 위한 ctrl키 눌러짐 여부, 표시하지 않을 키들
    if (key == kKey_Ctrl)
      kbd.isKeyDown() ? (isCtrlPressed = true) : (isCtrlPressed = false);
    if (!kbd.isKeyDown() ||	// released
	(key >= 0x80 &&		// not ASCII
	 key != kKey_Up && key != kKey_Down &&
	 key != kKey_Left && key != kKey_Right) ||
	key == kKey_ESC || key == kKey_Tab)
      continue;

    /* Action based on different key */
    // case: Return
    if (key == kKey_Enter ||
	isCtrlPressed && (key == 'j' || key == 'J' ||
			  key == 'm' || key == 'M')) {
      con.puts("\n");
      input_buf_[input_end_] = '\0';
      for (input_at_ = 0; input_buf_[input_at_]; input_at_++) {
	if (input_buf_[input_at_] != ' ') {
	  kbd.update();		// 에 남아있는 '\n'를 제거 (실행한는 함수에서 곧바로 key를 입력받는 경우를 대비)
	  execute();
	  recordInput();
	  break;
	}
      }

      initInput();
      con.printf(prompt_);
    }
    // case: Delete Backward Character
    else if (key == kKey_BackSpace ||
	     isCtrlPressed && (key == 'h' || key == 'H')) {
      deleteBackwardChar();
    }
    // case: Delete Forward Character
    else if (key == kKey_Delete ||
	     isCtrlPressed && (key == 'd' || key == 'D')) {
      deleteForwardChar();
    }
    // case: Go to Backward Character
    else if (key == kKey_Left ||
	     isCtrlPressed && (key == 'b' || key == 'B')) {
      BackwardChar();
    }
    // case: Go to Forward Character
    else if (key == kKey_Right ||
	     isCtrlPressed && (key == 'f' || key == 'F')) {
      forwardChar();
    }
    // case: Previous input_buf_
    else if (key == kKey_Up ||
	     isCtrlPressed && (key == 'p' || key == 'P')) {
      previousInput();
    }
    // case: Next input_buf_
    else if (key == kKey_Down ||
	     isCtrlPressed && (key == 'n' || key == 'N')) {
      nextInput();
    }
    // case: shortcut -> clear
    else if (isCtrlPressed && (key == 'l' || key == 'L')) {
      initInput();      
      clearCmd(0, 0);
      con.printf(prompt_);
    }
    // case: Just insert Character
    else {
      insertChar(key);
    }
  }
}

void Shell::recordInput()
{
  
}

void Shell::previousInput()
{
  
}

void Shell::nextInput()
{
  
}

void Shell::BackwardChar()
{
  if (input_end_ <= 0)
    return;

  if (con.wherex() == 0)
    con.gotoxy(con.getWidth() - 1, con.wherey() - 1);
  else
    con.gotoxy(con.wherex() - 1, con.wherey());    

  --input_at_;
}

void Shell::forwardChar()
{
  if (input_at_ >= input_end_)
    return;

  if (con.wherex() == con.getWidth() - 1)
    con.gotoxy(0, con.wherey() + 1);
  else
    con.gotoxy(con.wherex() + 1, con.wherey());

  ++input_at_;
}

void Shell::initInput()
{
  memset(input_buf_, 0, sizeof(input_buf_));
  input_at_ = input_end_ = 0;
}

void Shell::insertChar(u8_t key)
{
  if (input_end_ >= kInputBufLen - 1)
    return;

  // change input buffer
  char tmp[kInputBufLen] = {0,};
  strncpy(tmp, input_buf_ + input_at_, input_end_ - input_at_);
  input_buf_[input_at_++] = key;
  strncpy(input_buf_ + input_at_, tmp, strlen(tmp));
  ++input_end_;
  
  // print change
  con.printf("%c", key);
  printMessage(con.wherex(), con.wherey(), tmp);
}

void Shell::deleteForwardChar()
{
  if (input_at_ >= input_end_)
    return;

  // change input buffer
  strncpy(input_buf_ + input_at_,
	  input_buf_ + (input_at_ + 1),
	  input_end_ - input_at_ - 1);
  input_buf_[--input_end_] = ' ';
  
  // print changes
  int x = con.wherex();
  int y = con.wherey();
  con.printf("%s", input_buf_ + input_at_);
  con.gotoxy(x, y);
}

void Shell::deleteBackwardChar()
{
  if (input_at_ <= 0)
    return;

  // change input buffer
  strncpy(input_buf_ + (input_at_ - 1),
	  input_buf_ + input_at_,
	  input_end_ - input_at_);
  --input_at_;
  input_buf_[--input_end_] = ' ';

  // set coordinates
  u32_t backward_x, backward_y;
  if (con.wherex() == 0) {
    backward_x = con.getWidth() - 1;
    backward_y = con.wherey() - 1;
  }
  else {
    backward_x = con.wherex() - 1;
    backward_y = con.wherey();
  }

  // print changes
  con.gotoxy(backward_x, backward_y);
  con.printf("%s", input_buf_ + input_at_);
  con.gotoxy(backward_x, backward_y);
}

void Shell::execute()
{
  char cmd[kKeywordLen];
  char object_str[MAX_OTHER_STRING_LEN];
  char option[MAX_OPTION_LEN];

  // extract token
  extractCmd(cmd);
  extractOthers(object_str, option);

  // search command
  int cmd_idx = searchCmd(cmd);
  if (strcmp(cmd_list[cmd_idx].keyword, "EndOfCmd") == 0) {
    con.printf("%s: command not found\n", cmd);
    return;
  }

  // execute command
  cmd_list[cmd_idx].command(object_str, option);
}

void Shell::extractCmd(char* cmd)
{
  int cmd_end = input_at_ + 1;
  while (input_buf_[cmd_end] != '\0' && input_buf_[cmd_end] != ' ')
    cmd_end++;

  int cmd_len = cmd_end - input_at_;
  strncpy(cmd, input_buf_ + input_at_, cmd_len);
  cmd[cmd_len] = '\0';

  input_at_ = cmd_end;
}

void Shell::extractOthers(char* object_str, char* option)
{
  int option_end = 0;
  int string_end = 0;
  
  for (; input_buf_[input_at_] != '\0'; input_at_++) {
    if (input_buf_[input_at_] == ' ')
      continue;

    if (input_buf_[input_at_] == '-') {
      while (input_buf_[input_at_ + 1] != ' ' && input_buf_[input_at_ + 1] != '\0')
	option[option_end++] = input_buf_[++input_at_];
    }
    else
      object_str[string_end++] = input_buf_[input_at_];
  }

  option[option_end] = '\0';
  object_str[string_end] = '\0';
}

int Shell::searchCmd(const char* cmd)
{
  int i;
  for (i = 0; strcmp("EndOfCmd", cmd_list[i].keyword) != 0; ++i) {
    if (strcmp(cmd_list[i].keyword, cmd) == 0)
      break;
  }
  return i;
}

// Command List
void clearCmd(const char* string, const char* option)
{
  con.clearScreen();
}

void shutdownCmd(const char* string, const char* option)
{
  if (strlen(option) == 0)
    con.puts("shutdown: no option\n");
  
  for (; *option != '\0'; option++) {
    switch (*option) {
    case 'r':
      kbd.reboot();
      con.puts("Shutdown Sinos\n");
      break;
    default:
      con.printf("shutdown: invalid option: %c\n", *option);
      return;
    }
  }
}

void dateCmd(const char* string, const char* option)
{
  time cur_time = gettime();
  date cur_date = getdate();
  
  con.printf("%d.%d.%d (%s) %d:%d:%d\n",
		  cur_date.year,
		  cur_date.month,
		  cur_date.day_of_month,
		  dayOfWeekToStr(cur_date.day_of_week),
		  cur_time.hour,
		  cur_time.min,
		  cur_time.sec);
}

void testTask()
{
  u16_t offset = sched.currentTask()->getNo();
  int y = offset / con.getWidth();
  int x = offset % con.getWidth() - 1;

  int i = 0;
  char output[2] = {0,};
  
  while (1) {
    output[0] = '0' + (i++ % 10);
    printMessage(x, y, output);
    // u64_t id = sched.currentTask()->getID();
    // double a, b, c;
    // a = (double)id / 31;
    // for (int i = 0; i < 4000; i++);
    // b = (double)id / 31;
    // for (int i = 0; i < 3000; i++);
    // c = (double)id / 31;
    // for (int i = 0; i < 5000; i++);

    // if (a == b)
    //   if (b == c)
    // 	continue;
    // break;
  }
}

void testThread()
{
  for (int i = 0; i < 4; i++)
    if (sched.newTask((u64_t)testTask, kFlagThread, 0, 0) == NULL)
      return;

  while (1) {}
}

void mtCmd(const char* string, const char* option)
{
  int num_tasks = atoi(string);
  if (num_tasks == 0) {
    con.printf("mt: [/]: mt [-t or -d ] <number of process>:\n"
	       "  -t for testing thread\n"
	       "  -d for testing FPU & SMID\n");
    return;
  }
  if (num_tasks > 1024) {
    con.printf("mt: max number of tasks: 1024\n");
    return;
  }

  for (; *option != '\0'; option++) {
    switch (*option) {
    case 't':
      if (num_tasks > 204) {
	con.printf("mt: max number of process that has 4 threads: 204\n");
	return;
      }
      for (int i = 0; i < num_tasks; ++i) {
	if (sched.newTask((u64_t)testThread, 0, 0, 0) == NULL)
	  break;
      }
      return;
    default:
      con.printf("kill: invalid option: %c\n", *option);
      return;
    }
  }  

  for (int i = 0; i < num_tasks; ++i) {
    if (sched.newTask((u64_t)testTask, 0, 0, 0) == NULL)
      break;
  }
}

void helpCmd(const char* string, const char* option)
{
  con.puts(" Sinos shell, command list\n");
  for (int i = 0; strcmp("EndOfCmd", cmd_list[i].keyword) != 0; ++i) {
    con.printf("  * %s: %s\n", cmd_list[i].keyword, cmd_list[i].info);
  }
}

void psCmd(const char*, const char*)
{
  int num_of_tasks = sched.size();
  con.printf("total %d\n", num_of_tasks);
  for (int i = 1; i <= num_of_tasks; ++i) {
    if (i % 10 == 0) {
      con.puts(" - press ANY KEY to continue to display ('q' to exit)\n");
      if (con.getch() == 'q')
    	return;
    }
    Task* cur_task = sched.getTaskInfo();
    con.printf("%c: NO.:%d, PRIORITY:%d, THREAD:%d, ID: 0x%X, PPID:0x%X\n",
	       (cur_task->getFlags() & kFlagThread) ? 'T' : 'P',
	       cur_task->getNo(),
	       cur_task->getPriority(),
	       cur_task->numOfThread(),
	       cur_task->getID(),
	       cur_task->getPPID());
  }
}

void killCmd(const char* string, const char* option)
{
  for (; *option != '\0'; option++) {
    switch (*option) {
    case 'a':
      for (int i = 0; i <= kMaxNumTasks; ++i) {
	Task* cur_task = sched.getTaskInfo();
	if (!cur_task->isSystem())
	  sched.deleteTask(cur_task->getNo());
      }
      return;
    default:
      con.printf("kill: invalid option: %c\n", *option);
      return;
    }
  }  

  int task_no = atoi(string);
  if (task_no == 0) {
    con.puts("kill: invalid task number: kill <task number>\n");
    return;
  }

  if (sched.deleteTask(task_no) == -1)
    con.puts("kill: task not found\n");
}
