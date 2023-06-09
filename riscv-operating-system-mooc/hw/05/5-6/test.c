struct S {
    unsigned int a;
    unsigned int b;
};
struct S s = {0};
#define set_struct(s) \
    s.a = a; \
    s.b = b;
#define get_struct(s) \
    a = s.a; \
    b = s.b;
void foo()
{
    register unsigned int a = 0x12345678;
    register unsigned int b = 0x87654321;
    set_struct(s);
    a = 0;
    b = 0;
    get_struct(s);
}

int main()
{
    foo();
    return 0;
}
