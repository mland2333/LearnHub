#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define BUFFER_SIZE 512

void _find(int fd, char *dirpath, char*name)
{
    //printf("_find\n");
    int fd2;
    struct dirent de;
    struct stat st;
    char buf[BUFFER_SIZE], *p;
    strcpy(buf, dirpath);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if(de.inum == 0)
            continue;
        if(strcmp(de.name, ".")==0||strcmp(de.name, "..")==0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        //printf("p:%s\n",p);
        if(stat(buf, &st) < 0)
        {
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        switch(st.type)
        {
            case T_DEVICE:
            case T_FILE:
                if(strcmp(name, de.name)==0)
                    printf("%s\n", buf);
                break;
            case T_DIR:
                
                if((fd2 = open(buf, 0)) < 0)
                {
                    fprintf(2, "find: cannot open %s\n", buf);
                    return;
                }
                _find(fd2, buf, name);
                close(fd2);
        }
    }
}
void find(char* path, char*name)
{
    
    int fd;
    
    struct stat st;
    if((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type)
    {
        case T_DEVICE:
        case T_FILE:
            printf("Your path should be a dirname\n");
            return;
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > BUFFER_SIZE)
            {
                printf("find: path too long\n");
                return;
            }
            _find(fd, path, name);
            close(fd);
            return;
    }
}



int main(int argc, char*argv[])
{
    if(argc != 3)
    {
        printf("Usage: find path name\n");
        exit(0);
    }
    char *path = argv[1];
    char *name = argv[2];
    find(path, name);
    exit(0);
}
