#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "stdlib.h"
#include "sys/wait.h"
void primes(int *p1, int *p2)
{
   int a;
   pid_t pid;
   int p3[2], p4[2];
   pipe(p3);
   pipe(p4);
   if((pid = fork())==0)
   {
       close(p1[0]);
       close(p1[1]);
       close(p2[0]);
       close(p2[1]);
       while(read(p3[0],&a,  sizeof(int))!=0)
        {
            sleep(1);
            printf("3:a = %x\n", a);
        }
       close(p3[0]);
       close(p4[1]);
       //sleep(1);
       exit(0);
   }
   else
   {
       close(p1[1]);
       close(p2[0]);
        while(read(p1[0],&a,  sizeof(int))!=0)
        {
            sleep(1);
            printf("2:a = %x\n", a);
            write(p3[1], &a, sizeof(int));
        }
        close(p1[0]);
        close(p2[1]);
        //printf("2\n");
        wait(NULL);
        exit(0);
   }
}
int main()
{
     
     int p1[2], p2[2];
     pipe(p1);
     pipe(p2);
     
     pid_t pid;
     if((pid = fork())==0)
     {
       
        primes(p1, p2);
        exit(0);
     }
     else
     {  
        close(p1[0]);
       close(p2[1]); 
        for(int i = 2;i < 35; i++)
        {
           write(p1[1], &i, sizeof(int));
        }
        close(p1[1]);
        char buf[1];
        printf("here\n");
        if(read(p1[0], buf, 1)==0)
        {
            printf("finish\n");
            
        }
        wait(NULL);
     } 
     return 0;
}
