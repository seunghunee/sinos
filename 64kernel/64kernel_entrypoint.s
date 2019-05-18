;;; 64kernel이 로딩되는 위치 : 0x100000
	[BITS 64]
	extern main
;;; Code Section
	;; Intialize segment selector
	mov ax, 32		; Data descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	;; Intialize stack register
	mov ss, ax
	mov esp, 0x5FFFF8
	mov ebp, 0x5FFFF8

	;; Call C++ main function
	call main

	jmp $
