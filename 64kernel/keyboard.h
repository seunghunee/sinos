#ifndef SINOS_64KERNEL_KEYBOARD_H
#define SINOS_64KERNEL_KEYBOARD_H

#include "types.h"
#include "queue.h"

const u8_t	kKey_None	 = 0;
const u8_t	kKey_ESC	 = 0x1B;
const u8_t	kKey_BackSpace	 = '\b';
const u8_t	kKey_Tab	 = '\t';
const u8_t	kKey_Enter	 = '\n';
const u8_t	kKey_Ctrl	 = 0x80;
const u8_t	kKey_LShift	 = 0x81;
const u8_t	kKey_RShift	 = 0x82;
const u8_t	kKey_Alt	 = 0x83;
const u8_t	kKey_Center	 = 0x84;
const u8_t	kKey_PrintScreen = 0x85;
const u8_t	kKey_Home	 = 0x86;
const u8_t	kKey_End	 = 0x87;
const u8_t	kKey_PageUp	 = 0x88;
const u8_t	kKey_PageDown	 = 0x89;
const u8_t	kKey_Up		 = 0x8A;
const u8_t	kKey_Down	 = 0x8B;
const u8_t	kKey_Left	 = 0x8C;
const u8_t	kKey_Right	 = 0x8D;
const u8_t	kKey_Insert	 = 0x8E;
const u8_t	kKey_Delete	 = 0x8F;
const u8_t	kKey_Pause	 = 0x90;
const u8_t	kKey_F1		 = 0x91;
const u8_t	kKey_F2		 = 0x92;
const u8_t	kKey_F3		 = 0x93;
const u8_t	kKey_F4		 = 0x94;
const u8_t	kKey_F5		 = 0x95;
const u8_t	kKey_F6		 = 0x96;
const u8_t	kKey_F7		 = 0x97;
const u8_t	kKey_F8		 = 0x98;
const u8_t	kKey_F9		 = 0x99;
const u8_t	kKey_F10	 = 0x9A;
const u8_t	kKey_F11	 = 0x9B;
const u8_t	kKey_F12	 = 0x9C;
const u8_t	kKey_CapsLock	 = 0x9D;
const u8_t	kKey_NumLock	 = 0x9E;
const u8_t	kKey_ScrollLock	 = 0x9F;

/* Polling과 Interrupt 모두 대응 가능하도록 설계되었다.
   Controller의 output buffer의 내용을 읽어 큐에 저장하는 loadData함수를
   polling loop안에 두면 polling방식, ISR내에 두게 되면 interrupt방식
   1. loadData로 scan code를 queue에 push하고
   2. getASKEY나 getScanCode로 queue에 저장된 값을 얻어내고
   3. update로 Toggle key와 LED를 업데이트하고 얻어낸 값을 pop한다.*/

class Keyboard
{
protected:
  DISALLOW_COPY_AND_ASSIGN(Keyboard);
  
  bool is_shift_down_;
  bool is_num_lock_on_;
  bool is_caps_lock_on_;
  bool is_scroll_lock_on_;

  bool is_extended_code_;
  u8_t num_remaining_pause_key_code_;

  Queue<u8_t, 100> scan_codes_;
  static const u8_t scanCodeToChar[][2];

  u8_t readStatusRegister() const;
  void writeCtrlRegister(u8_t data) const;
  inline bool isBufReadable() const;
  inline bool isBufWritable() const;

  void writeInBuf(u8_t data) const;
  u8_t readOutBuf() const;
  bool updateToggleKeyStatus();	// LED변경이 필요하면 true를 리턴
  void updateKeyboardLED();
  void waitACK();

  inline u8_t toDownCode(u8_t scan_code) const;
  bool isCombinedKey() const;
  inline bool isAlpha() const;
  inline bool isNumOrSymbol() const;
  inline bool isNumpad() const;

public:
  Keyboard() {}
  void init();

  inline bool isKeyDown() const;

  void loadData();  
  /* 큐에서 값을 얻어내는 밑의 두 함수는 기본적으로 큐가 찰때까지
     기다리도록 되어있다. 큐의 빈 버퍼의 쓰레기값을 읽지 않기 위해 */
  u8_t getScanCode() const;
  u8_t getASKEY();
  /* Scan code를 Toggle key status를 참조하여 ASCII code 및
     위의 할당된 상수로 치환해 주는 함수, 단, Extended key와
     Pause key는 2번이상(scan code개수 만큼) 호출해야 한다.*/
  void update();
  void reboot() const;
};

inline bool Keyboard::isBufReadable() const
{    // bit 0 : ouput buffer state is full : empty
  return (readStatusRegister() & 1) ? true : false;
}

inline bool Keyboard::isBufWritable() const
{     // bit 1 : input buffer state is full : empty
  return (readStatusRegister() & 2) ? false : true;
}

inline bool Keyboard::isKeyDown() const
{
  return (getScanCode() & 0x80) ? false : true;
}

inline u8_t Keyboard::toDownCode(u8_t scan_code) const
{        // Is this Up code ? yes              : no
  return (scan_code & 0x80) ? scan_code - 0x80 : scan_code;
}

inline bool Keyboard::isAlpha() const
{
  u8_t c = scanCodeToChar[getScanCode()][0];
  return ('a' <= c && c <= 'z') ? true : false;
}

inline bool Keyboard::isNumOrSymbol() const
{
  return (0x02 <= getScanCode() && getScanCode() <= 0x35 && !isAlpha())
    ? true : false;
}

inline bool Keyboard::isNumpad() const
{
  return (0x47 <= getScanCode() && getScanCode() <= 0x53) ? true : false;
}

extern Keyboard kbd;

#endif // SINOS_64KERNEL_KEYBOARD_H
