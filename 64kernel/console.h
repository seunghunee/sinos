#ifndef SINOS_64KERNEL_CONSOLE_H
#define SINOS_64KERNEL_CONSOLE_H

#include "types.h"

void printMessage(int x, int y, const char* msg);

class Console
{
protected:
  u8_t* // const
  kVideoMemAddr_;

  // Screen
  u32_t width_;
  u32_t height_;
  u32_t max_position_;

  void scrollUpLine();
  
  // The point to print out.
  u32_t point_;			// y * width + x

  void setCursor(u32_t point) const;

public:
  Console(u8_t* addr = (u8_t*)0xB8000,
	  u32_t width = 80, u32_t height = 25)
    : kVideoMemAddr_(addr),
      width_(width), height_(height), max_position_(width * height),
      point_(0)
  {}

  void init()
  {
    kVideoMemAddr_ = (u8_t*)0xB8000;
    width_ = 80;
    height_ = 25;
    max_position_ = width_ * height_;
    point_ = 0;
  }
  
  // Output
  void puts(const char* str);
  void printf(const char* format, ...);
  
  // Input
  u8_t getch() const;

  // Cursor
  void gotoxy(u32_t x, u32_t y);
  u32_t wherex() const {return point_ % width_;}
  u32_t wherey() const {return point_ / width_;}

  // Screen
  u32_t getHeight() const {return height_;}
  u32_t getWidth() const {return width_;}
  void clearScreen();
};

inline void Console::gotoxy(u32_t x, u32_t y)
{
  point_ = (y % height_) * width_ + (x % width_);
  setCursor(point_);
}

extern Console con;

#endif // SINOS_64KERNEL_CONSOLE_H
