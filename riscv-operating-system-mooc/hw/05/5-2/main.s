    .section .text 
    .global main
main:
    li t1, 0x1
    li t2, 0x2
    li t3, 0x3
    add t0, t1, t2
    sub t4, t0, t3

    .end

