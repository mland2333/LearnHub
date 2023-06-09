#include "kernel/types.h"

#include "user/user.h"

int main(int argc, char*argv[])
{
    int p1[2];
    
    int pid;
    char buf[2];
    if(pipe(p1)==-1)
    {
        printf("pipe error\n");
        exit(1);
    }
    if((pid = fork())==0)
    {
       
       if(read(p1[0], buf, 1)==1)
            printf("%d: received ping\n",getpid());
       write(p1[1],"h",1);
       
       exit(0);
    }
    else
    {   
        
        write(p1[1],"h",1);
        if(read(p1[0], buf, 1)==1)
            printf("%d: received pong\n",getpid());
        
        exit(0);
    }
    
}


