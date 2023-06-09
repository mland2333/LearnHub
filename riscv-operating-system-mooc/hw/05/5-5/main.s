#int array[2] = {0x11111111, 0xffffffff};
#int i = array[0];
#int j = array[1];

.section .data
array:
    .word 0x11111111, 0xffffffff

.section .text

lw t0, array
lw t1, 4(array)
