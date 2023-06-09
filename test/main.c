#include <stdio.h>

int main()
{
    float a = 4;
    int b  = *( (int *)(&a));
    printf("b = %d", b);
    return 0;
}
