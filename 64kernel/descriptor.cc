#include "descriptor.h"
#include "string.h"

DTManager::DTManager(u64_t* base_addr)
  : limit_(0xFFFF), base_addr_(base_addr)
{      // overflow
}

void DTManager::setEntryMem8B(u64_t data)
{
  limit_ += 8;
  base_addr_[limit_ / 8] = data;
}

GDTManager::GDTManager(u64_t* base_addr)
  : DTManager(base_addr)
{
  // Set null descriptor
  setSegDesMem();
}

u64_t GDTManager::getLowerEntry(u16_t flags,
				u32_t limit,
				u64_t base_addr) const
{
  u64_t dte = (u64_t)(limit & 0xFFFF);
  dte = dte | (u64_t)(limit & 0xF0000) << 32;
  dte = dte | (base_addr & 0xFFFFFF) << 16;
  dte = dte | (base_addr & 0xFF000000) << 32;
  dte = dte | (u64_t)flags << 40;

  return dte;
}

u64_t GDTManager::getUpperEntry(u64_t base_addr) const
{
  return base_addr >> 32;
}

void GDTManager::setSegDesMem(u16_t flags,
			      u32_t limit,
			      u64_t base_addr)
{
  setEntryMem8B(getLowerEntry(flags, limit, base_addr));
}

void GDTManager::setTSSDesMemAndTSSMem(u16_t flags,
				       TSS& tss,
				       u64_t base_addr)
{
  setTSSDesMem(flags, base_addr);
  setTSSMem(tss);
}

void GDTManager::setTSSDesMem(u16_t flags,
			      u64_t base_addr)
{
  TSS_base_addr_ = (TSS*)(base_addr_ + (limit_ + 1) / 8 + 2);
  //                                   (total bytes)/ 8 + TSS
  base_addr = (u64_t)TSS_base_addr_;

  setEntryMem8B(getLowerEntry(flags, sizeof(TSS) - 1, base_addr));
  setEntryMem8B(getUpperEntry(base_addr));
}

void GDTManager::setTSSMem(TSS& tss) const
{
  memset(TSS_base_addr_, 0, sizeof(TSS));
  for (int i = 0; i < 3; ++i) {
    TSS_base_addr_->RSP[i] = tss.RSP[i];
  }
  for (int i = 0; i < 7; ++i) {
    TSS_base_addr_->IST[i] = tss.IST[i];
  }
  TSS_base_addr_->IOmap_base_addr = tss.IOmap_base_addr;
}

void GDTManager::loadGDTR() const
{
  asm volatile ("lgdt (%0)"
		:
		: "r" (&limit_));
}

void GDTManager::loadTR() const
{
  u16_t offset_TSSDes = (limit_ + 1) - 16;
  asm volatile ("ltr %0"
		:
		: "r" (offset_TSSDes));
}

u64_t GDTManager::getTSSBaseAddr() const
{
  return (u64_t)TSS_base_addr_;
}

IDTManager::IDTManager(u64_t* base_addr)
  : DTManager(base_addr)
{
}

void IDTManager::setGateDesMem(u16_t flags,
			       u16_t segment_selector,
			       ISR_t isr_addr)
{
  setEntryMem8B(getLowerEntry(flags, segment_selector, isr_addr));
  setEntryMem8B(getUpperEntry(isr_addr));
}

u64_t IDTManager::getLowerEntry(u16_t flags,
				u16_t segment_selector,
				ISR_t isr_addr) const
{
  u64_t dte = (u64_t)isr_addr & 0xFFFF;
  dte = dte | ((u64_t)isr_addr & 0xFFFF0000) << 32;
  dte = dte | (u64_t)segment_selector << 16;
  dte = dte | (u64_t)flags << 32;

  return dte;
}

u64_t IDTManager::getUpperEntry(ISR_t isr_addr) const
{
  return (u64_t)isr_addr >> 32;
}

void IDTManager::loadIDTR() const
{
  asm volatile ("lidt (%0)"
		:
		: "r" (&limit_));
}
