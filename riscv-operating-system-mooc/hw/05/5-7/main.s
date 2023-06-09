.data
msg:
    .string "hello,world!\0"
len_msg:
    .string "length = %d\n"
.text
.globl main
main:
    addi sp, sp, -8
    sd ra, (sp)
    la a0, msg
    call get_len
    #ld ra, (sp)  
    mv a1, a0
    la a0, len_msg
    call printf
    #li a2, 4
    #li a7, 64
    #ecall
    ld ra, (sp)
    addi sp, sp, 8
    li a0, 0
    ret
                  
get_len:
    li a1, 0
loop:
    lbu a2, (a0)
    beqz a2, back 
    addi a1, a1, 1
    addi a0, a0, 1
    j loop
back:
    mv a0, a1
    ret
