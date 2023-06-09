



.section .data
msg:
	.asciz "a = %d\n"

.section .text
.globl main
.type main, @function
main:
	# Set the integer value to print
	li a0, 42

	# Load the address of the format string into a1
	la a1, msg

	# Load the length of the format string into a2
	li a2, 7

	# Call write using ecall instruction
	li a7, 64
	ecall

	# Exit program
	li a0, 0
	li a7, 93
	ecall


        
