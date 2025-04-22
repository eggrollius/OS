; ========================
; Define Multi-Boot Header 
; ========================
%assign ALIGN     1<<0                ; Aligns loaded modules on page boundaries
%assign MEMINFO   1<<1                ; Provides memory map to us
%assign FLAGS     ALIGN | MEMINFO     ; Multi-Boot flag field
%assign MAGIC     0x1BADB002          ; 'Magic' number that bootloader looks for
%assign CHECKSUM  -(MAGIC + FLAGS)    ; Checksum of the above fields

; ======================
; Place Multiboot Header
; ======================
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; ==================
; Allocate for stack
; ==================
section .bss
align 16        ; We must align with 16 bytes, or else undefined behaviour (System V ABI)
stack_bottom:   ; Lower addr of the stack
resb 16384      ; 16 KiB
stack_top:      ; Higher addr of the stack

section .text
global _start

; ==============================
; Define start routine,
; Bootloader will place us here.
; ==============================
extern kernel_main
_start:
    mov esp, stack_top   ; setup the stack pointer
    call kernel_main        ; Initiate the kernels main subroutine

    ; if nothing to do, then infinite loop 
    cli       ; clear the 'interrupt enable' flag in eflags
hang:
    hlt       ; halts until the next interrupt, there will be none
    jmp hang  ; just in case

