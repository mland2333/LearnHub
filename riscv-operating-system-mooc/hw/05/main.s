#char array[] = {'h', 'e', 'l', 'l', 'o', ',', 'w', 'o', 'r', 'l', 'd', '!', '\0'};
#int len = 0;
#while (array[len] != '\0') {
#len++;
#}

.data
msg:
    .string "hello,world!\0"

.text
.globl main
main:
    la a0, msg
    call get_len    
    mv a1, a0
    #li a0, 1
    #li a2, 4
    #li a7, 64
    #ecall
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
