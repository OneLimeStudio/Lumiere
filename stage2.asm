[BITS 32]
global _start
extern kmain


_start:
    mov edi, 0xB8000

    mov ax, 0x0F46     ; 'H'
    mov [edi], ax

    mov ax, 0x0F49     ; 'I'
    mov [edi+2], ax
    call delayL
    call kmain
    jmp $

delayL:
    mov ecx, 10000000   
.loop:
    dec ecx
    jnz .loop
    ret