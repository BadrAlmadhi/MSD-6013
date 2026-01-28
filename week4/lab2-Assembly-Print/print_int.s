
; =========================================
; Name: Badr Almadhi
; Class: CS6013
; File: print_int.s
; Purpose: print an integer using write syscall
; =============================================

global _start

section .data
digit: db '7', 10

section .text
_start:
    mov rax, 1
    mov rdi, 1
    mov rsi, digit
    mov rdx, 2
    syscall

    mov rax, 60
    xor rdi, rdi
    syscall



