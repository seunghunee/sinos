#include "console.h"
#include "string.h"
#include "stdio.h"
#include "ioport.h"
#include "keyboard.h"

Console con;

void printMessage(int x, int y, const char* msg)
{
  char* video_memory_address = (char*)0xB8000;
  video_memory_address += 2 * x + 80 * 2 * y;

  for (int i = 0; msg[i] != '\0'; ++i) {
    *video_memory_address = msg[i];
    video_memory_address += 2;
  }
}

u8_t Console::getch() const
{
  while (u8_t ret_char = kbd.getASKEY()) {
    if (kbd.isKeyDown()) {
      kbd.update();
      return ret_char;
    }
    kbd.update();
  }
}

void Console::setCursor(u32_t point) const
{
  // Cursor location low register
  outb(0xF, kCRTCAddr);
  outb(point, kCRTCData);

  // Cursor location high register
  outb(0xE, kCRTCAddr);
  outb(point >> 8, kCRTCData);
}

void Console::scrollUpLine()
{
  // Copy text to one line up
  memcpy(kVideoMemAddr_,
	 kVideoMemAddr_ + width_ * 2,
	 width_ * (height_ - 1) * 2);

  // Set last line to blank
  for (int i = width_ * (height_ - 1); i < max_position_; i++)
    kVideoMemAddr_[i * 2] = 0;

  // Set point to last line
  point_ = width_ * (height_ - 1);
}

void Console::puts(const char* str)
{
  for (int i = 0; str[i]; i++) {
    switch (str[i]) {
    case '\n':
      point_ += width_ - (point_ % width_);
      break;
    case '\t':
      point_ += 8 - (point_ % 8);
      break;
    default:
      kVideoMemAddr_[point_ * 2] = str[i]; // one char => 2bytes
      point_++;
      break;
    }

    if (point_ >= max_position_)
      scrollUpLine();
  }

  setCursor(point_);
}

void Console::printf(const char* format, ...)
{
  char buffer[256];
  
  va_list ap;
  va_start(ap, format);
  vsprintf(buffer, format, ap);
  va_end(ap);

  puts(buffer);
}

void Console::clearScreen()
{
  for (point_ = 0; point_ < max_position_; point_++)
    kVideoMemAddr_[point_ * 2] = ' ';
  gotoxy(0, 0);
}
