.section .text
.global main

main:
    li t0, 0x87654321
    li, t1, 0x0000ffff
    and t2, t0, t1
    srli t3, t0, 16
    
