#include "page.h"

void printMessage(int x, int y, const char* msg);
bool set64kernelMemory(int* to, const int value, int space = 1);
int copy64kernelArea();
extern "C"
{
  void getCPUID(unsigned eax, unsigned* p_eax, unsigned* p_ebx, unsigned* p_ecx, unsigned* p_edx);
  /* eax값에 따라 cpuid의 출력값을 각각의 포인터를 참조해 저장해주는 함수 */
  int switch64kernel(void);
}

int main()
{
  printMessage(0, 3, "32bit C++ Kernel is operational.");

  //왜 OS구동 하는데 필요한 최소메모리로 64MB로 검사하는가
  printMessage(0, 4, "Checking memory size...");
  if (set64kernelMemory((int*)0x4000000, 0x20310065, 0x100000 / 4) == false) {
    printMessage(0, 5, ">> error : Sinos needs 64MB at least.");
    while (1);
  }

  printMessage(0, 5, "Initializing 64Kernel Area...");
  if (set64kernelMemory((int*)0x800000, 0) == false) {
    printMessage(0, 6, ">> error : Initialization failed. please check A20 gate.");
    while (1);
  }

  printMessage(0, 6, "Initializing 64bit Page Tables...");
  if (init64bitPageTables((PageTableEntry*)0x1200000, "1G") == -1) {
    printMessage(0, 7, ">> error : Page Tables Initialization failed");
    while (1);
  }

  printMessage(0, 7, "Checking 64bit mode support...");
  unsigned edx;
  getCPUID(0x80000001, 0, 0, 0, &edx);
  //edx의 bit 29가 64bit mode를 지원하는 여부를 나타낸다.
  if ((edx & (1 << 29)) == 0) {
    printMessage(0, 8, ">> error : We can not switch to 64bit mode");
    while (1);
  }

  printMessage(0, 8, "Switching to 64bit mode...");
  copy64kernelArea();
  switch64kernel();

  return 0;
}

void printMessage(int x, int y, const char* msg)
{
  char* video_memory_address = (char*)0xB8000;
  video_memory_address += 2 * x + 80 * 2 * y;

  for (int i = 0; msg[i] != '\0'; ++i) {
    *video_memory_address = msg[i];
    video_memory_address += 2;
  }
}

bool set64kernelMemory(int* to, const int value, int space)
{
  //64커널은 1MB(0x100000)에서 부터 로드된다.
  int* current_address = (int*)0x100000;

  while (current_address <= (int*)to) {
    *current_address = value;
    if (*current_address != value) {
      return false;
    }
    current_address += space;
  }

  return true;  
}

int copy64kernelArea()
{
  int num_sectors_32kernel = *((short*)0x7CE2);
  int num_sectors_64kernel = *((short*)0x7CE4);

  int* srcAddress = (int*)(0x10000 + num_sectors_32kernel * 512);
  int* destAddress = (int*)(0x100000);

  for (int i = 0; i < num_sectors_64kernel; ++i) {
    for (int j = 0 ; j < 512; j += 4) {
      *destAddress++ = *srcAddress++;
    }
  }

  return (int)destAddress;
}
