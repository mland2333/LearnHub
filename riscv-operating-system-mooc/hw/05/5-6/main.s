# struct S {
#unsigned int a;
#unsigned int b;
#};
#struct S s = {0};
#define set_struct(s) \
#s.a = a; \
#s.b = b;
#define get_struct(s) \
#a = s.a; \
#b = s.b;
#void foo()
#{
#register unsigned int a = 0x12345678;
#register unsigned int b = 0x87654321;
#set_struct(s);
#a = 0;
#b = 0;
#get_struct(s);
#}

.section .data
.globl S
S:
.struct
    .word a
    .word b
.globl d
.type d, @object
.size d, 8
d:
    .zero 8

.macro set_struct s,a,b
    sw a, (s)
    sw b, 4(s)
.endm

.macro get_struct s
    lw a, (s)
    lw b, 4(s)
.endm
 
 .text
    li t0, 0x12345678
    li t1, 0x87654321
    la t2, d
    set_struct t2,t0,t1
    li t0, 0
    li t1, 0
    get_struct t2,t0,t1


