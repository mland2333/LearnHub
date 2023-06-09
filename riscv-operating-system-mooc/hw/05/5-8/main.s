 
.data

msg:
    .string "result = %d\n"
.text
.global main

square:
    addi sp, sp, -8
    sd s0, (sp)
    mv s0, a0
    mul a0, s0, s0
    ld s0, (sp)
    addi sp, sp, 8
    ret
sum_squares:
    addi sp, sp, -32
    sd s0, (sp)
    sd s1, 8(sp)
    sd s2, 16(sp)
    mv s0, a0
    li s1, 0
    li s2, 1
loop:    
    bgt s2, s0, end
    mv a0, s2
    sd ra, 24(sp)
    call square
    ld ra, 24(sp)
    add s1, a0, s1
    addi s2, s2, 1
    j loop
end:
    mv a0, s1
    ld s2, 16(sp)
    ld s1, 8(sp)
    ld s0, (sp)
    addi sp, sp, 32
    
    ret
main:
    addi sp, sp, -16
    sd ra, (sp)
    sd a0, 8(sp)
    li a0, 3
    call sum_squares
    mv a1, a0
    la a0, msg
    call printf
    ld a0, 8(sp)
    ld ra, (sp)
    addi sp, sp, 16
    li a0, 0
    jr ra
    


