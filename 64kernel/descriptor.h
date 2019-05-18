#ifndef SINOS_64KERNEL_DESCRIPTOR_H
#define SINOS_64KERNEL_DESCRIPTOR_H

#include "types.h"

const u16_t	kGDTEntryFlag_Type_bit8  = 0x0001;
const u16_t	kGDTEntryFlag_Type_bit9  = 0x0002;
const u16_t	kGDTEntryFlag_Type_bit10 = 0x0004;
const u16_t	kGDTEntryFlag_Type_bit11 = 0x0008;
const u16_t	kGDTEntryFlag_S		 = 0x0010;	// Segment
const u16_t	kGDTEntryFlag_DPL0	 = 0x0000;	// Descriptor Privilege Level
const u16_t	kGDTEntryFlag_DPL1	 = 0x0020;
const u16_t	kGDTEntryFlag_DPL2	 = 0x0040;
const u16_t	kGDTEntryFlag_DPL3	 = 0x0060;
const u16_t	kGDTEntryFlag_P		 = 0x0080;	// Present
const u16_t	kGDTEntryFlag_AVL	 = 0x1000;	// Available
const u16_t	kGDTEntryFlag_L		 = 0x2000;	// Long mode (IA-32e mode)
const u16_t	kGDTEntryFlag_DB	 = 0x4000;	// Default Operation Size
const u16_t	kGDTEntryFlag_G		 = 0x8000;	// Granularity

const u16_t	kIDTEntryFlag_IST1	     = 0x0001;
const u16_t	kIDTEntryFlag_IST2	     = 0x0002;
const u16_t	kIDTEntryFlag_IST3	     = 0x0003;
const u16_t	kIDTEntryFlag_IST4	     = 0x0004;
const u16_t	kIDTEntryFlag_IST5	     = 0x0005;
const u16_t	kIDTEntryFlag_IST6	     = 0x0006;
const u16_t	kIDTEntryFlag_IST7	     = 0x0007;
const u16_t	kIDTEntryFlag_Type_Interrupt = 0x0E00;
const u16_t	kIDTEntryFlag_Type_Trap	     = 0x0F00;
const u16_t	kIDTEntryFlag_DPL0	     = 0x0000;
const u16_t	kIDTEntryFlag_DPL1	     = 0x2000;
const u16_t	kIDTEntryFlag_DPL2	     = 0x4000;
const u16_t	kIDTEntryFlag_DPL3	     = 0x6000;
const u16_t	kIDTEntryFlag_P		     = 0x8000;

const u16_t kGDTEntryFlags_SinosCode
= kGDTEntryFlag_DPL0 | kGDTEntryFlag_Type_bit11 | kGDTEntryFlag_Type_bit9 |
  kGDTEntryFlag_S | kGDTEntryFlag_P | kGDTEntryFlag_L | kGDTEntryFlag_G;
const u16_t kGDTEntryFlags_SinosData
= kGDTEntryFlag_DPL0 | kGDTEntryFlag_Type_bit9 |
  kGDTEntryFlag_S | kGDTEntryFlag_P | kGDTEntryFlag_L | kGDTEntryFlag_G;
const u16_t kGDTEntryFlags_SinosTSS
= kGDTEntryFlag_DPL0 | kGDTEntryFlag_Type_bit11 | kGDTEntryFlag_Type_bit8 |
  kGDTEntryFlag_P | kGDTEntryFlag_G;

const u16_t kIDTEntryFlags_SinosGate
= kIDTEntryFlag_DPL0 | kIDTEntryFlag_Type_Interrupt |
  kIDTEntryFlag_IST1 | kIDTEntryFlag_P;

#pragma pack(push, 1)

struct TSS
{
  u32_t reserved0;
  u64_t RSP[3];
  u32_t reserved1;
  u32_t reserved2;
  u64_t IST[7];
  u32_t reserved3;
  u32_t reserved4;
  u16_t reserved5;
  u16_t IOmap_base_addr;
};

class DTManager // Descriptor Table Manager
{
protected:
  DISALLOW_COPY_AND_ASSIGN(DTManager);
  
  u16_t limit_;
  u64_t* base_addr_;

  void setEntryMem8B(u64_t data);
  
public:
  DTManager(u64_t* base_address);

  u16_t getLimit() const {return limit_;}
  u64_t getAddr() const {return (u64_t)base_addr_;}
};

class GDTManager : public DTManager
{
protected:
  DISALLOW_COPY_AND_ASSIGN(GDTManager);
  
  TSS* TSS_base_addr_;
  
  u64_t getLowerEntry(u16_t flags,
		      u32_t limit,
		      u64_t base_addr) const;
  u64_t getUpperEntry(u64_t base_addr) const;
  void setTSSDesMem(u16_t flags,
		    u64_t base_addr);
  void setTSSMem(TSS& tss) const;
  
public:
  GDTManager(u64_t* base_addr);

  void setSegDesMem(u16_t flags = 0,
		    u32_t limit = 0,
		    u64_t base_addr = 0);
  void setTSSDesMemAndTSSMem(u16_t flags,
			     TSS& tss,
			     u64_t base_addr = 0);
  u64_t getTSSBaseAddr() const;
  
  void loadGDTR() const;
  void loadTR() const;
};

class IDTManager : public DTManager
{
protected:
  DISALLOW_COPY_AND_ASSIGN(IDTManager);
  
  u64_t getLowerEntry(u16_t flags,
		      u16_t segment_selector,
		      ISR_t isr_addr) const;
  u64_t getUpperEntry(ISR_t isr_addr) const;

public:
  IDTManager(u64_t* base_address);

  void setGateDesMem(u16_t flags,
		     u16_t segment_selector,
		     ISR_t isr_addr);
  
  void loadIDTR() const;
};

#pragma pack(pop)

#endif // SINOS_64KERNEL_DESCRIPTOR_H
