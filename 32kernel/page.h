#ifndef SINOS_32KERNEL_PAGE_H
#define SINOS_32KERNEL_PAGE_H

typedef unsigned long long PageTableEntry;

const PageTableEntry kPTE_Flag_P   = 1;	  // Present
const PageTableEntry kPTE_Flag_RW  = 2;	  // Read/Write
const PageTableEntry kPTE_Flag_US  = 4;	  // User/Supervisor
const PageTableEntry kPTE_Flag_PWT = 8;	  // Page-Level Write-Through
const PageTableEntry kPTE_Flag_PCD = 16;  // Page-Level Cache Disable
const PageTableEntry kPTE_Flag_A   = 32;  // Accessed

const PageTableEntry kPTE_Flag_D   = 64;  // Dirty
const PageTableEntry kPTE_Flag_PS  = 128; // Page Size
const PageTableEntry kPTE_Flag_G   = 256; // Global
const PageTableEntry kPTE_Flag_PAT = 4096;// Page Attribute Table Index
const PageTableEntry kPTE_Flag_EXB = 0x8000000000000000; // Execute-Disable

// 각 Page map level에서의 Page table entry flag들을 담아놓은 구조체
struct PMLs_PTE_Flags
{
  PageTableEntry pml1_flags;
  PageTableEntry pml2_flags;
  PageTableEntry pml3_flags;
  PageTableEntry pml4_flags;
};

int init64bitPageTables(PageTableEntry* const kPML4BaseAddress,
			const char* physical_memory_size,
			PMLs_PTE_Flags PMLs_PTE_flags
			= { kPTE_Flag_P | kPTE_Flag_RW,
			    kPTE_Flag_P | kPTE_Flag_RW,
			    kPTE_Flag_P | kPTE_Flag_RW,
			    kPTE_Flag_P | kPTE_Flag_RW, });
/* 첫 테이블의 기준주소와 관리대상의 총 물리메모리량, 그리고
   각 테이블 레벨의 속성을 지정한 구조체를 인수로 넘겨주면
   64bit페이지의 테이블들을 만들어주는 함수.(단, 4KB page로 생성)
   성공시, 0를 반환하고 아래조건을 만족하지 않아 실패시 -1을 반환.
   
   첫 번째 인수는 CR3에도 입력되는 PML4의 Base address,
   두 번째 인수는 관리하려고 하는 물리메모리 크기를 문자열로
   나타낸 값으로 단위를 정확하게 입력해야 한다.
   단위로는 대문자 영문[K, M, G, T]만 간주한다. ex) 4K, 32G, 16M, 1T
   1. KB미만, TB를 초과하는 단위는 간주하지 않는다.
   2. KB로 환산했을 때 64bit정수로 표현가능하여야 한다.
   3. 하나의 페이지가 4KB이니, 적어도 4KB이상 설정할 필요가 있다.
   세 번째 인수는 각 테이블 레벨의 속성을 담은 구조체로
   비트 연산으로 set하려는 flag들을 비트or연산으로 묶으면 된다.
   ## default값으로 모든 PML의 P와 R/W비트가 on으로 설정된다.*/

#endif // SINOS_32KERNEL_PAGE_H
