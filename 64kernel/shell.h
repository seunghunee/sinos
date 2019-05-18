#ifndef SINOS_64KERNEL_SHELL_H
#define SINOS_64KERNEL_SHELL_H

#include "types.h"
#include "console.h"
#include "list.h"

#define DIR_PATH_LEN 256
#define PROMPT_LEN 256
#define MAX_OPTION_LEN 128
#define MAX_OTHER_STRING_LEN 128

const u16_t kInputBufLen = 256;
const u16_t kKeywordLen = 256;
const u8_t kInfoLen = 60;
const u8_t kMaxInputs = 3;

class Shell
{
protected:
  DISALLOW_COPY_AND_ASSIGN(Shell);
  
  // Console console_;
  char dir_path_[DIR_PATH_LEN];
  char prompt_[PROMPT_LEN];

  char input_buf_[kInputBufLen];
  int input_end_;	     // index where will be set null character
  int input_at_;	     // current index
  char input_list_[kMaxInputs][kInputBufLen];
  int list_at;

  // Operation
  void initInput();
  void deleteBackwardChar();
  void deleteForwardChar();
  void insertChar(u8_t key);
  void execute();
  void BackwardChar();
  void forwardChar();
  void recordInput();
  void previousInput();
  void nextInput();

  int searchCmd(const char* cmd);
  void extractCmd(char* cmd);
  void extractOthers(char* option, char* object_str);

public:
  Shell();

  void start();
};

#endif // SINOS_64KERNEL_SHELL_H
