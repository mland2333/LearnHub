 #include"platform.h"

.macro UART_REG, reg, addr_reg
    li \addr_reg, UART0
    addi \addr_reg, \addr_reg, \reg
   
.endm

.macro uart_read_reg, reg, result_reg
    UART_REG \reg, \result_reg
    lbu \result_reg, 0(\result_reg)
.endm

.macro uart_write_reg, reg, v
    addi sp, sp, -8
    sd t0, (sp)
    UART_REG \reg, t0
    sb \v, (t0)
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


.equ LSR_RX_READY, 0b1
.equ LSR_TX_IDLE, 0b100000

.globl uart_init, uart_putc, uart_puts
.text

uart_init:
    addi sp, sp, -8
    sd s1, (sp)
    uart_write_reg IER, x0
    uart_read_reg LCR, s1
    ori s1, s1, 0b10000000
    uart_write_reg LCR, s1
    li s1, 0x03
    uart_write_reg DLL, s1
    uart_write_reg DLM, x0
    li s1, 0b011
    uart_write_reg LCR, s1
    ld s1, (sp)
    ret

uart_putc:
    addi sp, sp, -8
    sd s1, (sp)
    uart_read_reg LSR, s1
    andi s1, s1, LSR_TX_IDLE
cloop:
    beqz s1, cloop
    uart_write_reg THR, a0
    ld s1, (sp)
    addi sp, sp, 8
    ret

uart_puts:
    addi sp, sp, -16
    sd ra, 8(sp)
    sd s1, (sp)
    mv s1, a0
sloop:    
    lb a0, (s1)
    beqz a0, end
    call uart_putc
    addi s1, s1, 1
    j sloop
end:
    ld s1, (sp)
    ld ra, 8(sp)
    addi sp, sp, 16
    ret
uart_in:
    addi sp, sp, -16
    sd ra, (sp)
    sd s1, (sp)

   

