;;; 부트로더가 로딩되는 위치 : 0x7C00
;;; Code Section
	;; CS = 0x7C0로 초기화
	jmp 0x7C0:Start

Start:
	;; Initialize segment register
	mov ax, cs
	mov ds, ax
	mov ax, 0xB800		; video memory address
	mov es, ax

	;; Initialize stack register
	mov ax, 0
  	mov ss, ax
	mov sp, 0xFFFE
	mov bp, 0xFFFE
	
	;; Clear screen
	mov di, 0
	mov cx, 80 * 25		; 글자 개수 = 가로*세로
ClearSreenLoop:
	mov byte [es:di], 0
	add di, 2		; 글자하나당 2byte
	loop ClearSreenLoop

	push start_message
	push 0
	push 0
	call printMeassge
	add sp, 6

	;; Reset Disk Drive (읽기 전 드라이브 리셋)
	;; BIOS가 부팅 된 Disk번호를 DL에 자동으로 저장해 준다.
	mov ah, 0
	int 0x13		; Disk I/O service (BIOS)
	jc DiskErrorHandler	; if error then CF is set
	
	;; Extended Read Drive Parameters (LBA)
	mov ah, 0x48
	mov si, result_buffer
	int 0x13
	jc DiskErrorHandler
	
	;; Extended Read Sectors from Drive (LBA)
	mov ah, 0x42
	mov si, DAP
	int 0x13
	jc DiskErrorHandler

	push complete_message
	push 1
	push 0
	call printMeassge
	add sp, 6

	;; Jump to 32kernel entrypoint
	jmp 0x1000:0



;;; Function
printMeassge:
	push bp
	mov bp, sp
	push es
	push ax
	push bx
	push cx
	push dx
	push si
	push di

	mov ax, 0xB800
	mov es, ax
	;; x
	mov di, word [bp + 4]
	;; y
	mov si, 160
	mov ax, word [bp + 6]
	mul si
	;; 좌표
	add di, ax
	mov si, word [bp + 8]
messageLoop:
	mov cl, byte [si]
	cmp cl, 0
	je messageLoopEnd
	
	mov byte [es:di], cl
	add si, 1
	add di, 2
	jmp messageLoop

messageLoopEnd:
	pop di
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
	pop es
	pop bp
	ret
	
DiskErrorHandler:
	mov si, 0
	mov di, 0
ErrorLoop:	
	mov cl, byte [disk_error_message + si]
	cmp cl, 0
	je ErrorEnd
	
	mov byte [es:di], cl
	add si, 1
	add di, 2
	jmp ErrorLoop

ErrorEnd:	
	jmp $


	
;;; Data Section
disk_error_message:	db "Disk Error", 0
start_message:		db "Sinos is comming...", 0
complete_message:	db "Load complete", 0

sectors_32kernel:	dw 0
sectors_64kernel:	dw 0
	;; Disk Address Packet
DAP:
	db 16			; size of DAP
	db 0			; unused
	dw 1			; num of sectors to be read
	;; where to be loaded (segment:offset)
	dw 0			; offset
	dw 0x1000		; segment
	dq 1			; Start sector

	;; Readed LBA Parameters
result_buffer:
	dw 30			; size of Result Buffer
	dw 0			; information flags
	dd 0			; num of cylinders
	dd 0			; num of heads
	dd 0			; num of sectors per track
num_of_sectors:	dq 0
	dw 0			; bytes per sector
	dd 0			; optional pointer to EDD
		
	times 510 - ($ - $$) db 0
	db 0x55
	db 0xAA
