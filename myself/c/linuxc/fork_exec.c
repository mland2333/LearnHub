
#include "helper.h".h"

const int N = 10;
const int MAXARGS = 128;
const int MAXLINE = 8192;
void evel(char*cmdline);
int parseline(char*buf, char*argv);
int builtin_command(char**argv);

int main(int argc, char*argv[], char*envp[])
{
    print_env(argc,argv,envp);
    
    char cmdline[MAXLINE];

    while(1)
    {
        printf("> ");
        fgets(cmdline,MAXLINE,stdin);
    }

    exit(0);
}
