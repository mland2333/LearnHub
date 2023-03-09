#include "helper.h"

void unix_error(char*msg)
{
    fprintf(stderr,"%s:%s\n",msg,strerror(errno));
    exit(0);
}
void print_env(int argc, char*argv[], char*envp[])
{
    printf("Command-ine arguments:\n");
    for(int i = 0;i<argc;++i)
    {
        printf("\targv[%d]: %s\n",i,argv[i]);
    }
    printf("Environment variables:\n");
    for(int i = 0;envp[i]!=NULL;++i)
    {
        printf("envp[%d]: %s\n",i,envp[i]);
    }
}
