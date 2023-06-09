#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define R 0
#define W 1

#define MAX_NUM 35

int is_divied(int a, int b)
{
    if(a == b)
    {
        printf("prime %d\n",a);
        return 1;
    }
    int num = a/b;
    if(a==num*b)
        return 1;
    return 0;
} 

void primes(int *fr, int *fw, int num)
{
    //printf("enter primes\n");
    sleep(1);
    int a = 0;
    if(read(fr[R],&a, sizeof(int))!=0)
    {
        int pid;
        int cw[2], cr[2];
        pipe(cw);
        pipe(cr);
        if((pid = fork())==0)
        {
            //printf("pid: %d\n", getpid());
            //sleep(1); 
            close(fr[R]);
            
            close(fw[W]);
            close(cw[W]);
            close(cr[R]);
            primes(cw, cr, num+1);
        }
        else
        {   
            close(cw[R]);
            close(cr[W]);
            do
            {   
                //printf("%d:num = %x\n", num, a);
                //sleep(1);
                if(is_divied(a, num)==0)
                    write(cw[W], &a, sizeof(int));
            }
            while(read(fr[R], &a, sizeof(int))!=0);
            //sleep(1);
            close(fr[R]);
            close(cw[W]);
            if(read(cr[R], &a, sizeof(int))==0)
                close(fw[W]);
            exit(0);
        }
    }
    else
    {   
        close(fr[R]);
        close(fr[W]);
        close(fw[R]);
        close(fw[W]);
        
        exit(0);
    } 
}

int main(int argc, char*argv[])
{
    
    int pw[2], pr[2];
   
    pipe(pw);
    pipe(pr);
    int pid;
    if((pid=fork())==0)
    {   
       
        int num = 2;
        close(pw[W]);
        close(pr[R]);
        primes(pw, pr, num);
        exit(0);
        
    }
    else
    {   
        close(pw[R]);
        close(pr[W]);
    for(int i = 2; i<MAX_NUM; i++)
        {
            write(pw[W], &i, sizeof(int));
            
        }
        close(pw[W]);
        char buf[1];
        if(read(pr[R], buf, 1)==0)
            exit(0);
    }
    return 0;
}
