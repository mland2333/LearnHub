#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "stdlib.h"
#include "sys/wait.h"
int main()
{
    pid_t pid;
    char buf[1024];
    int p[2];
    pipe(p);
    if((pid = fork())==0)
    {
        if((pid = fork())==0)
        {
            sleep(1);
             write(p[1], "孙子进程\n", sizeof("孙子进程\n"));
             close(p[1]);
             //printf("1\n");
             exit(0);
        }
        else
        {
            close(p[1]);
             if(read(p[0], buf, sizeof(buf))!=0)
             //write(p[1], "子进程\n", sizeof("子进程\n"));
                printf("收到，来自%s", buf);
             else
                 printf("1\n");
             //close(p[1]);
             wait(NULL);
             exit(0);
        }
        
        //exit(0);
    }
    else
    {
        //while(read(p[0], buf, sizeof(buf))!=0)
            //printf("收到，来自%s", buf);
        wait(NULL);
        exit(0);
    }
}
