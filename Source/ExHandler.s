.global ctrl_handleException

.section .text

// R0 points to the start of exception data.
// R1 points to the start of context data.
.type ctrl_handleException, %function
ctrl_handleException:
    // Save context pointer.
    push {r1}

    // Call handler.
    mrc p15, 0, r1, c13, c0, 3
    ldr r1, [r1, #0x4C]
    blx r1

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