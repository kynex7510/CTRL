.global ctrl_handleException

.section .text

// R0 points to the start of exception data.
// R1 points to the start of context data.
.type ctrl_handleException, %function
ctrl_handleException:
    // Save pointers.
    push {r1}
    mov r6, r0

    // Call handlers.
    mrc p15, 0, r4, c13, c0, 3
    mov r5, #0

    _ctrl_handleException_callHandler:
    add r1, r4, r5
    ldr r1, [r1, #0x4C]
    cmp r1, #0
    beq _ctrl_handleException_incHandlerCount
    mov r0, r6
    blx r1

    _ctrl_handleException_incHandlerCount:
    add r5, #0x04
    cmp r5, #0x34
    bne _ctrl_handleException_callHandler

    // Restore CPSR.
    ldr sp, [sp]
    ldr r0, [sp, #0x40]
    msr cpsr, r0

    // Restore context.
    pop {r0-r12}

    ldr lr, [sp, #0x08]
    str lr, [sp, #0x0C]

    ldr lr, [sp, #0x04]
    str lr, [sp, #0x08]

    ldr lr, [sp]
    str lr, [sp, #0x04]
    
    add sp, #0x04
    ldm sp, {sp, lr, pc}