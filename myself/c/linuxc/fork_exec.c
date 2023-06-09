
#include "helper.h"

const int N = 10;
const int MAXARGS = 128;
const int MAXLINE = 8192;

void eval(char*cmdline);
int parseline(char*buf, char**argv);
int builtin_command(char**argv);

int main(int argc, char*argv[], char*envp[])
{
    print_env(argc,argv,envp);
    
    char cmdline[MAXLINE];

    while(1)
    {
        printf("> ");
        fgets(cmdline,MAXLINE,stdin);
        if(feof(stdin))
            exit(0);
        eval(cmdline);
    }

    exit(0);
}

int parseline(char*buf, char**argv)
{
    char *delim;
    int argc, bg;
    buf[strlen(buf)-1] = ' ';
    while(*buf&& *buf==' ')
        buf++;
    argc = 0;
    while((delim=strchr(buf,' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        while(*buf&&*buf==' ')
            buf++;
    }
    argv[argc] = NULL;

    if(argc == 0)
        return 1;
    if((bg = (*argv[argc-1]=='&'))!=0)
        argv[argc-1] = NULL;
    return bg;
}
void eval(char*cmdline)
{
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    strcpy(buf,cmdline);
    bg = parseline(buf,argv);
    if(argv[0]==NULL)
        return;
    if(builtin_command(argv))
    {
        if((pid = fork())==0)
        {
            printf("hello\n");
            if(execve(argv[0],argv,environ)<0)
            {
                printf("%s:Comand not found.\n",argv[0]);
                exit(0);
            }
        }
        if(!bg)
        {
            int status;
            if(waitpid(pid,&status,0)<0)
                unix_error("waitfg:waitpid error");
            else
                printf("%d,%s",pid,cmdline);
        }
    }
    
    return;
}
int builtin_command( char **argv)
{
    if (!strcmp(argv[0], "quit"))
        exit (0) ;
    if (!strcmp(argv[0], "&"))
        return 1;
    if (!strcmp(argv[0], "/usr/bin/ls"))
        return 1;
    return 0;
}
