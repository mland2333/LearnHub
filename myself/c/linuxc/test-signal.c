#include "helper.h"
const int MAXBUF = 8192;
void handler(int sig)
{
    int olderrno = errno;

    while((waitpid(-1,NULL,0)) > 0)
        //printf("waitpid error");
        printf("Handler reaped child\n");
    if(errno!=ECHILD)
        unix_error("waitpid error");
    sleep(1);
    errno = olderrno;
    //printf("Caught Sigint!\n");
    //exit(0);
}

int main()
{
    int i, n;
    char buf[MAXBUF];

    if(signal(SIGCHLD,handler) == SIG_ERR)
        unix_error("signal error");

    for(i = 0;i<3;i++)
    {
        if(fork() == 0)
        {
            printf("Hello from child%d\n", (int)getpid());
            exit(0);
        }
    }

    if((n = read(STDIN_FILENO, buf, sizeof(buf))) , 0)
        unix_error("read error");
    while(1);
    exit(0);
}
