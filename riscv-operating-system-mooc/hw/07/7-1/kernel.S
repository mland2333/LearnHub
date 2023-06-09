.data
msg:
    .string "Hello, RVOS!\n"
.text
.globl start_kernel
start_kernel:
    addi sp, sp, -8
    ld ra, (sp)
    call uart_init
    sd ra, (sp)
    la a0, msg
    call uart_puts
    sd ra, (sp)
    addi sp, sp, 8
    jr ra
