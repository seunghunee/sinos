	[BITS 32]
	global getCPUID

;;; void getCPUID(unsigned eax, unsigned* p_eax, unsigned* p_ebx, unsigned* p_ecx, unsigned* p_edx)
getCPUID:
	push ebp
	mov ebp, esp
	push eax
	push ebx
	push ecx
	push edx
	push edi

	mov eax, dword [ebp + 8]
	cpuid

	mov edi, dword [ebp + 12]
	cmp edi, 0
	je setEBX
	mov dword [edi], eax
setEBX:	
	mov edi, dword [ebp + 16]
	cmp edi, 0
	je setECX
	mov dword [edi], ebx
setECX:	
	mov edi, dword [ebp + 20]
	cmp edi, 0
	je setEDX
	mov dword [edi], ecx
setEDX:	
	mov edi, dword [ebp + 24]
	cmp edi, 0
	je end
	mov dword [edi], edx
	
end:	
	pop edi
	pop edx
	pop ecx
	pop ebx
	pop eax
	pop ebp
	ret
