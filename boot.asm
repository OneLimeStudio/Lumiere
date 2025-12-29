[BITS 16]
[ORG 0x7C00]

CODE_OFFSET equ 0x8
DATA_OFFSET equ 0x10

KERNEL_LOAD_SEG equ 0x1000
KERNEL_START_ADDR equ 0x100000

start:
    ; 1. Setup Segments and Stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Save boot drive ID
    mov [boot_drive], dl

    ; --- Visual Check 1: Real Mode ---
    mov si, msg_step1
    call print_string
    call delay

    ; --- Visual Check 2: A20 Gate ---
    in al, 0x92
    or al, 2
    out 0x92, al
    mov si, msg_step2
    call print_string
    call delay

    ; --- Visual Check 3: Loading Kernel ---
    mov si, msg_step3
    call print_string
    
    ; Reset Disk
    ;xor ax, ax
    ;mov dl, [boot_drive]
	call load_stage2
	;jmp 0x1000:0x0000

    ; --- Visual Check 4: Entering PM ---
    mov si, msg_step4
    call print_string
    call delay

    ; Switch to Protected Mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:init_pm

; --- 16-BIT HELPERS ---
print_string:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

delay:
    pusha
    mov cx, 0x07    ; Approx 0.5s delay
    mov dx, 0xA120
    mov ah, 0x86
    int 0x15
    popa
    ret

disk_error:
    mov si, msg_disk_err
    call print_string
    jmp $
load_stage2:
    mov ax, KERNEL_LOAD_SEG   ; 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 8
    mov ch, 0
    mov dh, 0
    mov cl, 2
    mov dl, [boot_drive]
    int 0x13
    jc disk_error
    ret
; --- DATA ---
boot_drive  db 0
msg_step1   db "1. Real Mode Initialized...", 13, 10, 0
msg_step2   db "2. A20 Gate Unlocked...", 13, 10, 0
msg_step3   db "3. Loading Kernel Sector...", 13, 10, 0
msg_step4   db "4. Jumping to Protected Mode...", 13, 10, 0
msg_disk_err db "FATAL: Disk Read Error", 0

; --- GDT ---
align 4
gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF, 0x0000
    db 0x00, 10011010b, 11001111b, 0x00
gdt_data:
    dw 0xFFFF, 0x0000
    db 0x00, 10010010b, 11001111b, 0x00
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; -------------------------------------------------------------------------
[BITS 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Clear screen
    mov edi, 0xB8000
    mov ecx, 80 * 25
    mov ax, 0x0720      ; Space
.clear:
    mov [edi], ax
    add edi, 2
    loop .clear

    ; Print PM Success Message
    mov edi, 0xB8000
    mov esi, msg_pm_success
    mov ah, 0x02        ; Green
.print:
    lodsb
    test al, al
    jz .to_kernel
    mov [edi], al
    mov [edi+1], ah
    add edi, 2
    jmp .print

.to_kernel:
    ; Jump to our loaded kernel
    ;jmp $
    ;mov si, msg_disk_err
    ;call print_string

    jmp CODE_OFFSET:0x10000

msg_pm_success db "SUCCESS: Protected Mode Active. Handing over to Kernel...", 0

times 510-($-$$) db 0
dw 0xAA55