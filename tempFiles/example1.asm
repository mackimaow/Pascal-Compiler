

section	.data

	msg db '[ERROR] ARRAY OUT OF BOUNDS!', 0xa  

	len equ $ - msg     

section .text
    global _start
_start:

	call __example

	mov eax, 1
	mov ebx, 0
	int 0x80


_write: 
	push ebp 
	mov ebp, esp 
 
	mov eax, [ebp+8]		; grab the first argument 
	push eax 				; stores the entire value to use  (-4) 
 
	cmp eax, -1 
	jg  _write_store_digits 
	mov edx, 0 
	mov ecx, -1				;  
	mul ecx					; negate value 
	mov [ebp-4], eax		; place back into register			 
 
	push '-' 
 
	mov ecx, esp 
	mov eax, 4              ; SYS_WRITE 
    mov edx, 1              ; Amount of chars to print 
    mov ebx, 1              ; STDOUT 
    int 0x80 
 
    pop eax 
 
_write_store_digits: 
 
	mov edx, 0 
	mov eax, [ebp-4]		; set up number to divide 
	mov ebx, 10				; set up divider 
	div ebx					; eax = eax / ebx    edx = eax mod ebx  
	add edx, '0' 
	mov [ebp-4], eax		; 
	push edx				; 
 
    cmp eax, 0  
jne _write_store_digits 
 
_write_digits: 
    mov ecx, esp 
	mov eax, 4              ; SYS_WRITE 
    mov edx, 1              ; Amount of chars to print 
    mov ebx, 1              ; STDOUT 
    int 0x80 
 
    pop eax 
    mov eax, ebp 
    cmp eax, esp 
jne _write_digits 
 
    mov eax, 0xa 
    push eax 
    mov ecx, esp 
	mov eax, 4              ; SYS_WRITE 
    mov edx, 1              ; Amount of chars to print 
    mov ebx, 1              ; STDOUT 
    int 0x80 
    pop eax 
 
    mov eax, 0xd 
    push eax 
    mov ecx, esp 
	mov eax, 4              ; SYS_WRITE 
    mov edx, 1              ; Amount of chars to print 
    mov ebx, 1              ; STDOUT 
    int 0x80 
    pop eax 
 
	mov esp, ebp 
	pop ebp 
	ret 


_less_than:
	mov eax, -1
	cmp ebx, ecx  
	jl _less_than_jump
	mov eax, 0
_less_than_jump:
	ret

_greater_than:
	mov eax, -1
	cmp ebx, ecx  
	jg _greater_than_jump
	mov eax, 0
_greater_than_jump:
	ret


_greater_than_equal:
	mov eax, -1
	cmp ebx, ecx  
	jge _greater_than_equal_jump
	mov eax, 0
_greater_than_equal_jump:
	ret

_less_than_equal:
	mov eax, -1
	cmp ebx, ecx  
	jle _less_than_equal_jump
	mov eax, 0
_less_than_equal_jump:
	ret

_not_equal:
	mov eax, -1
	cmp ebx, ecx  
	jle _not_equal_jump
	mov eax, 0
_not_equal_jump:
	ret

_equal:
	mov eax, -1
	cmp ebx, ecx  
	je _equal_jump
	mov eax, 0
_equal_jump:
	ret



__example_gcd:


	push ebp
	mov ebp, esp

	push eax	; base pointer of parent 
	push 0		; temporary register 

__example_gcd_end:

	mov esp, ebp
	pop ebp
	ret


__example_other:


	push ebp
	mov ebp, esp

	push eax	; base pointer of parent 
	push 0		; temporary register 

	mov eax,1

	mov [ebp-8],eax
	mov eax,10

 mov ecx,eax
	mov ebx,[ebp-8]

	mov eax, ebx
	mov edx, 0
	div ecx
	mov eax,edx 
	mov [ebp-8],eax

	mov eax,4

 mov ecx,eax
	mov ebx,[ebp-8]

	mov eax, ebx
	mov edx, 0
	div ecx
	mov [ebp-8],eax

	push eax

	mov eax,0

	mov edx,eax

	mov ebx,ebp

	add ebx,12

	mov eax,[ebx]
	cmp edx,eax
 jl _error

	mov eax,[ebx-4]
	cmp edx,eax
 jg _error

	sub ebx,8
	mov eax,edx 
	mov edx,4 
	mul edx
	 sub ebx,eax

	pop eax
	mov [ebx],eax 

	mov esp, ebp
	pop ebp
	ret


__example:


	push ebp
	mov ebp, esp

	push eax	; base pointer of parent 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; variable register 
	push 0		; temporary register 

__example_while_0:

	mov eax,0

	mov [ebp-56],eax
	mov eax,0

 mov ecx,eax
	mov ebx,[ebp-56]

	call _equal
	mov [ebp-56],eax

	cmp eax, 0

	je __example_while_end_0

	push 0

	pop eax

	jmp __example_while_0
__example_while_end_0:

	mov eax,1

	push eax

	push eax

	mov ebx,ebp

	add ebx,-12

	pop eax
	mov [ebx],eax 

	mov eax,1

	push eax

	mov eax,1
	mov ebx,[esp+4]
	mov ecx,[esp]
	cmp ebx,ecx
	jle __example_for_incr_0

	mov eax, 1

__example_for_incr_0:

	mov [esp+4],eax
__example_for_0:

	push 0

	pop eax

	mov ebx,ebp

	add ebx,-12
	mov eax,[ebx]

	mov ebx,[esp]
 	cmp eax,ebx
	je __example_for_end_0

	mov ebx,[esp+4]
 	add eax,ebx

	push eax

	mov ebx,ebp

	add ebx,-12

	pop eax
	mov [ebx],eax 

	jmp __example_for_0
__example_for_end_0:

	pop eax

	pop eax

	mov eax,0

	mov [ebp-56],eax
	mov eax,0

 mov ecx,eax
	mov ebx,[ebp-56]

	call _equal
	mov [ebp-56],eax

	cmp eax, 0

	je __example_else_0

	push 0

	pop eax

__example_else_0:

	mov ebx,ebp

	add ebx,-8
	mov eax,[ebx]

	push eax

	mov ebx,ebp

	add ebx,-12

	pop eax
	mov [ebx],eax 

	push 0

	mov ebx,-1
mul ebx

	mov [esp],eax
call _write
	pop eax

	mov esp, ebp
	pop ebp
	ret


_error:	            ;tells linker entry point
	push edx 		
	call _write 	
	pop edx 		
	push eax 		
	call _write 	
	 				
	mov	edx,len     ;message length
	mov	ecx,msg     ;message to write
	mov	ebx,1       ;file descriptor (stdout)
	mov	eax,4       ;system call number (sys_write)
	int	0x80        ;call kernel
	 				
	mov	eax,1       ;system call number (sys_exit)
	mov	ebx,1       ; error 				
	int	0x80        ;call kernel

