#include "keyboard.h"
#include "ioport.h"
#include "flags.h"

Keyboard kbd;

void Keyboard::init()
{
  is_shift_down_ = false;
  is_num_lock_on_ = true;
  is_caps_lock_on_ = false;
  is_scroll_lock_on_ = false;
  is_extended_code_ = false;
  num_remaining_pause_key_code_ = 0;
}

u8_t Keyboard::readStatusRegister() const
{
  return inb(kPS2Ctrl);
}

void Keyboard::writeCtrlRegister(u8_t data) const
{
  outb(data, kPS2Ctrl);
}

u8_t Keyboard::readOutBuf() const
{
  while (!isBufReadable()) ;
  return inb(kPS2Data);
}

void Keyboard::writeInBuf(u8_t data) const
{
  while (!isBufWritable()) ;
  outb(data, kPS2Data);	// 여기서 인터럽트 발생!!!
}

void Keyboard::loadData()
{
  // entry CS
  bool previous_IF = getIF();
  setIF(false);
  
  scan_codes_.push(readOutBuf());

  // exit CS
  setIF(previous_IF);
};

u8_t Keyboard::getScanCode() const
{
  while (scan_codes_.isEmpty()) ;
  return scan_codes_.front();
}

void Keyboard::update()
{
  if (updateToggleKeyStatus())
    updateKeyboardLED();

  // entry CS
  bool previous_IF = getIF();
  setIF(false);

  scan_codes_.pop();

  // exit CS
  setIF(previous_IF);
}

bool Keyboard::updateToggleKeyStatus()
{
  switch (getScanCode()) {
  case 0x36:
  case 0x2A:
    is_shift_down_ = true;
    return false;
  case 0xB6:
  case 0xAA:
    is_shift_down_ = false;
    return false;
  case 0x45:
    is_num_lock_on_ ^= true;
    return true;
  case 0x3A:
    is_caps_lock_on_ ^= true;
    return true;
  case 0x46:
    is_scroll_lock_on_ ^= true;
    return true;
  default:
    return false;
  }
}

void Keyboard::updateKeyboardLED()
{
  // 인터럽트가 ACK를 처리할 수 있기 때문에
  bool previous_IF = getIF();
  setIF(false);

  writeInBuf(0xED);		// LED변경을 요청
  waitACK();

  writeInBuf(is_caps_lock_on_ << 2 |
  	     is_num_lock_on_ << 1 |
  	     is_scroll_lock_on_);
  waitACK();
  
  setIF(previous_IF);
}

void Keyboard::waitACK()
{
  while (1) {
    u8_t data = readOutBuf();
    if (data == 0xFA) // Got ACK !
      return;
    scan_codes_.push(data);
  }
}

const u8_t Keyboard::scanCodeToChar[][2] = {
//{Nomal code, Combined code},          // Down scan code
  {kKey_None, kKey_None},		// None
  {kKey_ESC, kKey_ESC},		        // 0x01
  {'1', '!'},			        // 0x02
  {'2', '@'},			        // 0x03
  {'3', '#'},			        // 0x04
  {'4', '$'},			        // 0x05
  {'5', '%'},			        // 0x06
  {'6', '^'},			        // 0x07
  {'7', '&'},			        // 0x08
  {'8', '*'},			        // 0x09
  {'9', '('},			        // 0x0A
  {'0', ')'},			        // 0x0B
  {'-', '_'},			        // 0x0C
  {'=', '+'},			        // 0x0D
  {kKey_BackSpace, kKey_BackSpace},     // 0x0E
  {kKey_Tab, kKey_Tab},		        // 0x0F
  {'q', 'Q'},			        // 0x10
  {'w', 'W'},			        // 0x11
  {'e', 'E'},			        // 0x12
  {'r', 'R'},			        // 0x13
  {'t', 'T'},			        // 0x14
  {'y', 'Y'},			        // 0x15
  {'u', 'U'},			        // 0x16
  {'i', 'I'},			        // 0x17
  {'o', 'O'},			        // 0x18
  {'p', 'P'},			        // 0x19
  {'[', '{'},			        // 0x1A
  {']', '}'},			        // 0x1B
  {kKey_Enter, kKey_Enter},	        // 0x1C
  {kKey_Ctrl, kKey_Ctrl},	        // 0x1D
  {'a', 'A'},			        // 0x1E
  {'s', 'S'},			        // 0x1F
  {'d', 'D'},			        // 0x20
  {'f', 'F'},			        // 0x21
  {'g', 'G'},			        // 0x22
  {'h', 'H'},			        // 0x23
  {'j', 'J'},			        // 0x24
  {'k', 'K'},			        // 0x25
  {'l', 'J'},			        // 0x26
  {';', ':'},			        // 0x27
  {'\'', '\"'},			        // 0x28
  {'`', '~'},			        // 0x29
  {kKey_LShift, kKey_LShift},	        // 0x2A
  {'\\', '|'},			        // 0x2B
  {'z', 'Z'},			        // 0x2C
  {'x', 'X'},			        // 0x2D
  {'c', 'C'},			        // 0x2E
  {'v', 'V'},			        // 0x2F
  {'b', 'B'},			        // 0x30
  {'n', 'N'},			        // 0x31
  {'m', 'M'},			        // 0x32
  {',', '<'},			        // 0x33
  {'.', '>'},			        // 0x34
  {'/', '?'},			        // 0x35
  {kKey_RShift, kKey_RShift},	        // 0x36
  {'*', '*'},                    	// 0x37
  {kKey_Alt, kKey_Alt}, 		// 0x38
  {' ', ' '},	                 	// 0x39
  {kKey_CapsLock, kKey_CapsLock},	// 0x3A
  {kKey_F1, kKey_F1},			// 0x3B
  {kKey_F2, kKey_F2},			// 0x3C
  {kKey_F3, kKey_F3},			// 0x3D
  {kKey_F4, kKey_F4},			// 0x3E
  {kKey_F5, kKey_F5},			// 0x3F
  {kKey_F6, kKey_F6},			// 0x40
  {kKey_F7, kKey_F7},			// 0x41
  {kKey_F8, kKey_F8},			// 0x42
  {kKey_F9, kKey_F9},			// 0x43
  {kKey_F10, kKey_F10},			// 0x44
  {kKey_NumLock, kKey_NumLock},		// 0x45
  {kKey_ScrollLock, kKey_ScrollLock},	// 0x46
  {kKey_Home, '7'},		        // 0x47
  {kKey_Up, '8'}, 			// 0x48
  {kKey_PageUp, '9'},     		// 0x49
  {'-', '-'},            	        // 0x4A
  {kKey_Left, '4'},			// 0x4B
  {kKey_Center, '5'},			// 0x4C
  {kKey_Right, '6'},			// 0x4D
  {'+', '+'},            		// 0x4E
  {kKey_End, '1'},			// 0x4F
  {kKey_Down, '2'},			// 0x50
  {kKey_PageDown, '3'},     		// 0x51
  {kKey_Insert, '0'},      		// 0x52
  {kKey_Delete, '.'},           	// 0x53
  {kKey_None, kKey_None},		// 0x54
  {kKey_None, kKey_None},		// 0x55
  {kKey_None, kKey_None},		// 0x56
  {kKey_F11, kKey_F11},			// 0x57
  {kKey_F12, kKey_F12},			// 0x58
};

u8_t Keyboard::getASKEY()
{
  while (scan_codes_.isEmpty()) {}
  if (is_extended_code_) {
    is_extended_code_ = false;
    return scanCodeToChar[toDownCode(getScanCode())][0];
  }

  if (num_remaining_pause_key_code_ > 0) {
    --num_remaining_pause_key_code_;
    return kKey_Pause;
  }

  switch (u8_t scan_code = getScanCode()) {
  case 0xE1:
    num_remaining_pause_key_code_ = 2;
    return scan_code;
  case 0xE0:
    is_extended_code_ = true;
    return scan_code;
  default:
    if (isCombinedKey())
      return scanCodeToChar[toDownCode(scan_code)][1];
    else
      return scanCodeToChar[toDownCode(scan_code)][0];
  }
}

bool Keyboard::isCombinedKey() const
{
  if (isAlpha()) {
    if (is_caps_lock_on_ ^ is_shift_down_)
      return true;
  }
  else if (isNumOrSymbol()) {
    if (is_shift_down_)
      return true;
  }
  else if (isNumpad()) {
    if (is_num_lock_on_)
      return true;
  }

  return false;
}

void Keyboard::reboot() const
{
  while (!isBufWritable()) ;
  writeCtrlRegister(0xD1);	// Write output port
  writeInBuf(0);
}
