
section .text
    global _start

_start:
	mov ebp,esp

	mov eax, 0xa
    push eax
    mov ecx, esp
	mov eax, 4              ; SYS_WRITE
    mov edx, 1              ; Amount of chars to print
    mov ebx, 1              ; STDOUT
    int 0x80
    pop eax


	;push 123

	;mov edx,ebp
	;call __scope1

	mov eax, 1
	mov ebx, 0
	int 0x80


__scope1:
	push ebp
	mov ebp, esp
	push edx

	push 111

	mov edx,ebp
	call __scope2

	mov esp, ebp
	pop ebp
	ret
__scope2:
	push ebp
	mov ebp, esp
	push edx

	push 222

	mov edx,ebp
	call __scope3

	mov esp, ebp
	pop ebp
	ret
__scope3:
	push ebp
	mov ebp, esp
	push edx

	push 333
	mov eax,[ebp-4]  ; get the address stored in edx
	mov ecx,[eax-8]  ; get 222	
	push ecx
	call _write


	mov esp, ebp
	pop ebp
	ret



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




