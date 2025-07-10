.global crashForExHandler

.section .text

.type crashForExHandler, %function
crashForExHandler:
    mov r0, #0
    ldr r0, [r0]
    bx lr
