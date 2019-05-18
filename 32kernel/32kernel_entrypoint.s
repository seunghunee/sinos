;;; 32kernel이 로딩되는 위치 : 0x10000
;;; Code Section
	;; 부트로더에서 CS(0x1000):IP(0)으로 점프
	;; Intialize DS
	mov ax, cs
	mov ds, ax
	
	;; Load GDT
	cli			; 인터럽트 설정완료 전까지 정지상태가 좋다.
	lgdt [GDTR]

	;; Switch to Protected mode
	mov eax, 0x4000003B
	mov cr0, eax

	;; Jump to Protected Mode
	jmp dword 8:0x10000 + (ProtectedMode - $$)

	[BITS 32]
ProtectedMode:
	;; Intialize segment selector
	mov ax, 16		; Data descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	;; Intialize stack register
	mov ss, ax
	mov esp, 0xFFFE
	mov ebp, 0xFFFE

	push 0x10000 + (mode_chage_success_message - $$)
	push 2
	push 0
	call printMeassge
	add esp, 12

	;; Jump to 32kernel main
	jmp dword 8:0x10200


	
;;; Function	
printMeassge:
	push ebp
	mov ebp, esp
	push eax
	push ecx
	push esi
	push edi
	
	;; x
	mov edi, dword [ebp + 8]
	;; y
	mov esi, 160
	mov eax, dword [ebp + 12]
	mul esi
	;; 좌표
	add edi, eax
	mov esi, dword [ebp + 16]
messageLoop:
	mov cl, byte [esi]
	cmp cl, 0
	je messageLoopEnd
	
	mov byte [0xB8000 + edi], cl
	add esi, 1
	add edi, 2
	jmp messageLoop

messageLoopEnd:
	pop edi
	pop esi
	pop ecx
	pop eax
	pop ebp
	ret
	


;;; Data Section
mode_chage_success_message:	db "Switching to Protected mode", 0
	
	;; Global Descriptor Table
GDTbegin:
null_descriptor:
	dq 0
code_descriptor:
	dw 0xFFFF
	dw 0
	db 0
	db 0x9A			; Execute / Read
	db 0xCF			; D = 1, L = 0
	db 0
data_descriptor:
	dw 0xFFFF
	dw 0
	db 0
	db 0x92			; Read / Write
	db 0xCF			; D = 1, L = 0
	db 0
	
	;; 64bit mode의 segment descriptor
code_descriptor64:
	dw 0xFFFF
	dw 0
	db 0
	db 0x9A			; Execute / Read
	db 0xAF			; D = 0, L = 1
	db 0
data_descriptor64:
	dw 0xFFFF
	dw 0
	db 0
	db 0x92			; Read / Write
	db 0xAF			; D = 0, L = 1
	db 0
GDTend:	

	;; GDT Register
GDTR:
	;; Limit of GDT (크기 - 1)
	dw GDTend - GDTbegin - 1
	;; GDT의 Base Address (DS와 상관없는 절대주소)
	dd 0x10000 + (GDTbegin - $$)

	times 512 - ($ - $$) db 0
