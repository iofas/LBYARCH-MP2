section .data
msg db "Hello from Assembly!", 13, 10, 0
section .text
bits 64
default rel

extern printf

; SAXPY Assembly Kernel
; RCX = n (integer, length of vectors)
; XMM1 = A (float, scalar multiplier)
; R8 = X (pointer to float array)
; R9 = Y (pointer to float array)
; [RSP+48] = Z (pointer to float array, 5th parameter on stack)

global saxpy_asm

saxpy_asm:
    push rbp                    
    mov rbp, rsp                ;

    mov r10, [rbp+48]           ; Load Z pointer into r10 (48 = 16 + 32 shadow space)

    xor rax, rax 
    
    test rcx, rcx               ; Test if n is zero or negative
    jle .done                   ; If n <= 0, exit
    
.loop:
    movss xmm0, dword [r8 + rax*4]  ; xmm0 = X[i] (load single-precision float)
    
    mulss xmm0, xmm1            ; xmm0 = A * X[i] (sc alar multiply)
  
    movss xmm2, dword [r9 + rax*4]  ; xmm2 = Y[i] (load single-precision float)
    
    addss xmm0, xmm2            ; xmm0 = A * X[i] + Y[i] (scalar add)
    
    movss dword [r10 + rax*4], xmm0 ; Z[i] = xmm0 (store single-precision float)
    
    inc rax                     ; i++
    
    ; Check if we've processed all elements
    cmp rax, rcx                ; Compare i with n
    jl .loop                    ; If i < n, continue loop
    
.done:
    pop rbp                     
    ret                         
