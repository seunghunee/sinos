#include "page.h"

typedef unsigned long long uint_64;

const int kPTEsPerTable = 512;

// low level의 page수를 인수로 받아 high level의 필요한 table수를 반환.
inline uint_64 getNumNextPMLTables(uint_64 num_low_level_pages)
{
  uint_64 num_high_level_tables = num_low_level_pages / kPTEsPerTable;
  return (num_high_level_tables == 0) ? 1 : num_high_level_tables;
}
// high level의 기준주소와 table수를 인수로 받아 low level의 기준주소를 반환.
inline PageTableEntry* getPrevPMLBaseAddress(PageTableEntry* high_level_base_address,
					     uint_64 num_high_level_tables)
{
  // (필요한 table안의 총 PTE수) = (필요한 table수) x (테이블당 PTE수)
  uint_64 num_PTEs = num_high_level_tables * kPTEsPerTable;
  return high_level_base_address + num_PTEs;
}

void setPMLMem(PageTableEntry* this_base_address,
	       PageTableEntry* prev_level_base_address,
	       uint_64 num_prev_level_pages,
	       PageTableEntry pte_flags);
/* 한 page map level의 테이블 정보를 직접 메모리에 써서
   그 PML의 테이블들을 모두 준비한다.
   첫 번째 인수로 해당 PML의 기준주소를, (즉 그 PML의 첫 번째 PTE 주소)
   두 번째 인수로 전 단계 PML의 기준주소를,
   세 번째 인수로 전 단계 PML의 page(혹은 table)수, 즉 관리할 page 수를,
   네 번째 인수로 그 PML의 전체PTE에 적용할 flags를 보내면 된다.
   ## 단 주소를 받는 PageTableEntry*이 32bit자료형이기에 
   ## 이 범위를 넘는 페이지 어드레스는 다루기 힘들듯 하다. 
   ## 그래도 4GB까지 이기에 넉넉하다고 본다.(이론상 TB까진 충분)*/
uint_64 memSize_KB(const char* mem_size);
/* 문자열로 표현된 memory size를 KB단위로 환산해 부호없는 64비트 정수로 반환.
   단, 실패나 에러발생시 0 반환.
   ex) "2K"입력시, 2^11 == 2048반환
   # 단위를 입력할 땐 정확하게 입력해야 한다. ex) 32G, 16M, 4K, 1T
   # TB를 초과하는 단위 입력시, 에러발생.
   # 계산 값이 64bit정수로 표현가능하여야 한다.*/
bool isInteger(const char c);



int init64bitPageTables(PageTableEntry* const kPML4BaseAddress,
			const char* physical_memory_size,
			PMLs_PTE_Flags PMLs_PTE_flags)
{
  // 총 physical memory를 하나의 page 단위(4KB)로 나누면 총 page 수
  const uint_64 kNumPhysicalPages = memSize_KB(physical_memory_size) / 4;
  if (kNumPhysicalPages == 0)
    return -1;

  /* Page Table			  = > PML1
     Page Directory		  = > PML2
     Page Directory Pointer Table = > PML3
     Page Map Level 4 Table	  = > PML4 */
  const uint_64 kNumPML1Tables = getNumNextPMLTables(kNumPhysicalPages);
  const uint_64 kNumPML2Tables = getNumNextPMLTables(kNumPML1Tables);
  const uint_64 kNumPML3Tables = getNumNextPMLTables(kNumPML2Tables);
  const uint_64 kNumPML4Tables = getNumNextPMLTables(kNumPML3Tables);

  PageTableEntry* const kPML3BaseAddress = getPrevPMLBaseAddress(kPML4BaseAddress, kNumPML4Tables);
  PageTableEntry* const kPML2BaseAddress = getPrevPMLBaseAddress(kPML3BaseAddress, kNumPML3Tables);
  PageTableEntry* const kPML1BaseAddress = getPrevPMLBaseAddress(kPML2BaseAddress, kNumPML2Tables);

  setPMLMem(kPML4BaseAddress, kPML3BaseAddress, kNumPML3Tables, PMLs_PTE_flags.pml4_flags);
  setPMLMem(kPML3BaseAddress, kPML2BaseAddress, kNumPML2Tables, PMLs_PTE_flags.pml3_flags);
  setPMLMem(kPML2BaseAddress, kPML1BaseAddress, kNumPML1Tables, PMLs_PTE_flags.pml2_flags);
  setPMLMem(kPML1BaseAddress, (PageTableEntry*)0, kNumPhysicalPages, PMLs_PTE_flags.pml1_flags);

  return 0;
}

void setPMLMem(PageTableEntry* this_base_address,
	       PageTableEntry* prev_level_base_address,
	       uint_64 num_prev_level_pages,
	       PageTableEntry pte_flags)
{
  int space = num_prev_level_pages % kPTEsPerTable;
  for (; num_prev_level_pages > 0; --num_prev_level_pages) {
    *this_base_address++ = (PageTableEntry)prev_level_base_address | pte_flags;
    prev_level_base_address += kPTEsPerTable;
  }
  for (; space > 0; --space) {
    *this_base_address++ = 0;
  }
}

uint_64 memSize_KB(const char* mem_size)
{
  if (!isInteger(*mem_size))
    return 0;

  uint_64 mem_bytes = 0;
  while (isInteger(*mem_size)) {
    mem_bytes *= 10;
    mem_bytes += *mem_size++ - '0';
  }

  switch (*mem_size++) {
  case 'K':
    break;
  case 'M':
    mem_bytes = mem_bytes << 10;
    break;
  case 'G':
    mem_bytes = mem_bytes << 20;
    break;
  case 'T':
    mem_bytes = mem_bytes << 30;
    break;
  default:
    return 0;
  }

  if (*mem_size == '\0')
    return mem_bytes;
  else
    return 0;
}

bool isInteger(const char c)
{
  if ('0' <= c && c <= '9')
    return true;
  else
    return false;
}
