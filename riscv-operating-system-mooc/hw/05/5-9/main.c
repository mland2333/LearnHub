#include <stdio.h>
int foo(int a, int b)
{
    int c;
    /*asm [volatile](
        "add %[sum],%[add1],%[add2]"
        :[sum]"=r"(c)
        :[add1]"r"(a),[add2]"r"(b)
        );*/
    __asm__ volatile(
        "mul %[a], %[a], %[a]\n"
        "mul %[b], %[b], %[b]\n"
        "add %[c], %[a], %[b]\n"
        :[c]"=r"(c)
        :[a]"r"(a),[b]"r"(b)
        );
    return c;
}


int main()
{
    printf("result = %d\n", foo(2,3));
    return 0;
}
