; Name: Badr Almadhi
; Class: CS6013
; File: print_int.s
; Purpose: print an integer using write syscall
; =============================================

global _start ; start tells it's the begging of the program, start exist in this file of code. 

section .data
digit: db '7', 10 ;; this is also a label that db ( data byte ) 

section .text
_start:   ; it's a label that declare a function. 
    mov rax, 1 ;; this is asking to write to screen. 
    mov rdi, 1 ;; first pram write to standard out 
    mov rsi, digit ;; got to this digit and print it out. 
    mov rdx, 2 ;; third pram, dx => count of bytes we want to display. 
    syscall ;; pipeline between OS to program


    mov rax, 60 ;; 60 
    xor rdi, rdi ;;   same as  mov rdi, 0
    syscall


