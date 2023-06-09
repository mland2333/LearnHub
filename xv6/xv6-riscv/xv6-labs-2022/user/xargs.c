#include "../kernel/param.h"
#include "../kernel/types.h"

#include "user.h"

#define MAX_CHAR 128
void xargs(int argc, char *argv[])
{
    
    char p[MAX_CHAR];
    char *argv2[MAXARG];
    for(int j = 0;j<argc-1;j++)
    {
        argv2[j] = argv[j+1];
    }
    
    argv2[argc] = 0;
    char ch;
    int i = 0;
    int pid;
    while(read(0, &ch, 1)!=0)
    {
        if(ch!='\n')
        {
            if(i < MAX_CHAR)
                p[i++] = ch;
            else
            { 
                printf("argu is too long\n");
                return;
            }
        }
        else
        {
            
            p[i] = '\0';
            argv2[argc-1] = p;
            //for(int j = 0;j<argc;j++)
            //    printf("%s ", argv2[j]);
            if((pid = fork())==0)
            {
                exec(argv2[0], argv2);
            }
            else
            {
                wait(0);
                i = 0;
            }
        }
    }
}

int main(int argc, char*argv[])
{
    if(argc < 2)
    {
        printf("Usage: xargs execname\n");
        exit(0);
    }
    xargs(argc, argv);
    exit(0);
}
