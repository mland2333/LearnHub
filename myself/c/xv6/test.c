#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "stdlib.h"
void primes(int *p)
{
   int a;
   while(read(p[0],&a,  sizeof(int))!=0)
        printf("a = %x\n", a);
}
int main()
{
     
     int p[2];
     pipe(p);
     
     
     pid_t pid;
     if((pid = fork())==0)
     {
       
        primes(p);
        exit(0);
     }
     else
     {   
        for(int i = 2;i < 35; i++)
        {
           write(p[1], &i, sizeof(int));
        } 
     }
     sleep(1);
     return 0;
}
