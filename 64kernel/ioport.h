#ifndef SINOS_64KERNEL_INOUT_H
#define SINOS_64KERNEL_INOUT_H

#include "types.h"

// [PIC] Programable interrupt controller (Master)
const u16_t	kPIC1Command = 0x20;
const u16_t	kPIC1Data    = 0x21;
// [PIC] Programable interrupt controller (Slave)
const u16_t	kPIC2Command = 0xA0;
const u16_t	kPIC2Data    = 0xA1;

// [PIT] Programable interval Timer
const u16_t	kPITChannel0 = 0x40;
const u16_t	kPITChannel1 = 0x41;
const u16_t	kPITChannel2 = 0x42;
const u16_t	kPITCtrl     = 0x43;

// PS2 keryboard controller
const u16_t	kPS2Data = 0x60;
const u16_t	kPS2Ctrl = 0x64;

// CMOS
const u16_t	kCMOSAddr = 0x70;
const u16_t	kCMOSData = 0x71;

// Video display controller
const u16_t	kCRTCAddr = 0x3D4;
const u16_t	kCRTCData = 0x3D5;

// do port output byte
void outb(u8_t data, u16_t port);
// do port input byte
u8_t inb(u16_t port);

#endif // SINOS_64KERNEL_INOUT_H
