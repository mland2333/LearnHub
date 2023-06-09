#include <algorithm>
#include <iostream>

void f1(void)
{
    int a1 = 1;
    int a2 = 2;
    std::cout<<static_cast<int>(a1<=>a2)<<std::endl;
}

int main()
{
    f1();
    return 0;
}
