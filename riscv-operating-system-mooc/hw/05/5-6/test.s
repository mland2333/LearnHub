	.file	"test.c"
	.option nopic
	.attribute arch, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	s
	.section	.sbss,"aw",@nobits
	.align	3
	.type	s, @object
	.size	s, 8
s:
	.zero	8
	.text
	.align	1
	.globl	foo
	.type	foo, @function
foo:
	addi	sp,sp,-32
	sd	s0,24(sp)
	sd	s1,16(sp)
	sd	s2,8(sp)
	addi	s0,sp,32
	li	a5,305418240
	addi	s2,a5,1656
	li	a5,-2023407616
	addi	s1,a5,801
	lui	a5,%hi(s)
	sw	s2,%lo(s)(a5)
	lui	a5,%hi(s)
	addi	a5,a5,%lo(s)
	sw	s1,4(a5)
	nop
	ld	s0,24(sp)
	ld	s1,16(sp)
	ld	s2,8(sp)
	addi	sp,sp,32
	jr	ra
	.size	foo, .-foo
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	sd	ra,8(sp)
	sd	s0,0(sp)
	addi	s0,sp,16
	call	foo
	li	a5,0
	mv	a0,a5
	ld	ra,8(sp)
	ld	s0,0(sp)
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.ident	"GCC: (SiFive GCC 10.1.0-2020.08.2) 10.1.0"
