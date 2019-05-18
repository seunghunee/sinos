	[BITS 32]
	global switch64kernel

;;; void switch64kernel(void)
switch64kernel:
	;; Enable IA-32e mode
	mov ecx, 0xC0000080	; IA32_EFER address
	rdmsr
	or eax, 0x100		; Long mode enable
	wrmsr

	;; Set PAE bit
	mov eax, cr4
	or eax, 0x20		; PAE, OSFXSR, OSXMMEXCPT
	mov cr4, eax

	;; Set PML4 base address
	mov eax, 0x1200000
	mov cr3, eax
	
	;; Cache & Paging Enable (Switch to IA-32e mode)
	mov eax, cr0
	or eax, 0x80000000	; PG = 1
	and eax, 0xBFFFFFFF	; CD, EM = 0
	mov cr0, eax

	;; Jump to 64kernel entrypoint
	jmp dword 24:0x100000
