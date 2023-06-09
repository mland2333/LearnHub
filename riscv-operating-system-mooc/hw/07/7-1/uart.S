

.macro UART_REG reg, addr_reg
    li addr_reg, UART0
    addi addr_reg, reg
.endm

.macro uart_read_reg reg, result_reg
    UART_REG reg, result_reg
    lbu result_reg, (result_reg)
.endm

.macro uart_write_reg reg, v
    addi sp, sp, -8
    sd t0, sp
    UART_REG REG, t0
    sb v, (t0)
    ld t0, (sp)
    addi sp, sp, 8
.endm
    
    
.equ RHR, 0
.equ THR, 0
.equ DLL, 0
.equ IER, 1
.equ DLM, 1
.equ FCR, 2
.equ ISR, 2
.equ LCR, 3
.equ MCR, 4
.equ LSR, 5
.equ MSR, 6
.equ SPR, 7


.equ LSR_RX_READY  0b1
.equ LSR_TX_IDLE   0b100000

uart_init:
    addi sp, sp, -8
    sw, s0, (sp)
    uart_write_reg IER, x0
    uart_read_reg LCR, s0
    ori s0, 0b10000000
    uart_write_reg LCR, s0
    li s0, 0x03
    uart_write_reg DLL, s0
    uart_write_reg DLM, x0
    li, s0, 0b011
    uart_write_reg LCR, s0
    ret

uart_putc:
    addi sp, sp, -8
    lw s0, (sp)
    uart_read_reg LCR, s0
    andi s0, LSR_TX_IDLE
loop:
    beqz s0, loop
    uart_write_reg THR, a0
    sw, s0, (sp)
    addi sp, sp, 8
    ret

uart_puts:
    addi sp, sp, -8
    sw, s0, (sp)
    mv s0, a0
loop:    
    lb a0, (s0)
    beqz a0, end
    call uart_putc
    addi s0, s0, 1
    j loop
end:
    lw, s0, (sp)
    addi sp, sp, 8
    ret
    
   

