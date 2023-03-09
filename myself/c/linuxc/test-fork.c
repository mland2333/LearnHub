#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

const int N = 10;

void unix_error(char *msg)
{
    fprintf(stderr,"%s:%s\n",msg,strerror(errno));
    exit(0);
}

int main()
{
    /*pid_t pid = fork();
    if(pid == -1)
    {
        perror("fork error");
        exit(1);
    }
    else if(pid == 0)
        printf("child process,pid = %d\n",getpid());
    else
    {
        printf("parent process,pid = %d\n",getpid());
    }*/
    int i,status;
    pid_t pid[N], retpid;
    for(i = 0;i<N;++i)
    {
        pid[i] = fork();
        if(pid[i] == -1)
        {
            perror("fork error");
            exit(-1);
        }
        else if(pid[i] == 0)
        {
            printf("child process,pid = %d,parent process pid = %d\n",getpid(),getppid());
            //sleep(1);
            exit(i);
        }
        else
        {   
            if(i == 0)
                printf("parent process,pid = %d,parent process pid = %d\n",getpid(),getppid());
        }
    }
    i = 0;
    sleep(1);
    while((retpid = waitpid(pid[i++],&status, WNOHANG)) > 0 )
    {
        if(WIFEXITED(status))
            printf("child %d terminated normally with exit=%d\n",retpid,WEXITSTATUS(status));
        else 
            printf("child %d terminated abnormally\n",retpid);
    }
    if(errno != ECHILD&&errno!=EINTR&&errno!=EXIT_SUCCESS)
        unix_error("waitpid error");
    exit(0);
}
