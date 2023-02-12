#include "cachelab.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int hits = 0, misses = 0, eviction = 0;
char result[40];
const char help[] = "• -h: Optional help flag that prints usage info\n• -v: Optional verbose flag that displays trace info\n• -s <s>: Number of set index bits (S = 2 s is the number of sets)\n• -E <E>: Associativity (number of lines per set)\n• -b <b>: Number of block bits (B = 2b is the block size)\n• -t <tracefile>: Name of the valgrind trace to replay\0" ;
typedef struct {
    long long  flag;
    int enable;
} cacheline;

struct cache_group {
    int length, size;
    cacheline *cache;
    int *time;
};

void cache_group_init(struct cache_group *cg, int size, int groupsize) {
    for (int i = 0; i < groupsize; ++i) {
        cg[i].length = size;
        cg[i].cache = (cacheline *)malloc(sizeof(cacheline) * size);
        memset(cg[i].cache, 0, sizeof(cacheline) * size);
        cg[i].size = 0;
        cg[i].time = (int *)malloc(sizeof(int) * size);
        memset(cg[i].time, 0, sizeof(int) * size);
    }
}
void time_add(int *time, int size) {
    for (int i = 0; i < size; ++i) {
        ++time[i];
    }
}

void cache_access(struct cache_group *cg, long long flag) {
    for (int i = 0; i < cg->size; ++i) {
        if (cg->cache[i].enable == 1 && cg->cache[i].flag == flag) {
            cg->time[i] = 0;
            ++hits;
            strcpy(result, " hit");
            time_add(cg->time, cg->size);
            return;
        }
    }
    ++misses;
    strcpy(result, " miss");
    if (cg->size < cg->length) {
        cg->cache[cg->size].flag = flag;
        cg->cache[cg->size].enable = 1;
        ++cg->size;

    } else {
        int index;
        int time = 0;
        for (int i = 0; i < cg->size; ++i) {

            if (cg->time[i] > time) {
                time = cg->time[i];
                index = i;
            }
        }
        cg->cache[index].flag = flag;
        cg->time[index] = 0;
        ++eviction;
        strcat(result, " eviction");
    }
    time_add(cg->time, cg->size);
}
int main(int argc, char *argv[]) {
    // int hits, misses, eviction;
    int index = 1, block = 1, associate = 1;
    FILE *tracefile = NULL;
    int  vflag = 0;
    char str[40];
    memset(str, 0, sizeof(str));
    for (int i = 1; i < argc; i++) {
        switch (argv[i][0]) {
        case '-': {
            if (argv[i][1] == 'h')
            {
                printf("%s\n",help);
                return 0;
            }
                
            if (argv[i][1] == 'v') {
                vflag = 1;
                break;
            }
            if (argv[i][1] == 's') {
                i++;
                index = atoi(argv[i]);
                break;
            }
            if (argv[i][1] == 'E') {
                i++;
                associate = atoi(argv[i]);
                break;
            }
            if (argv[i][1] == 'b') {
                i++;
                block = atoi(argv[i]);
                break;
            }
            if (argv[i][1] == 't') {
                i++;
                tracefile = fopen(argv[i], "r");
                // printf("%s",argv[i]);
                break;
            }
        }
        }
    }

    int group = 1 << index;
    int group_index = 0;
    for (int group_i = group; group_i > 1; ++group_index)
        group_i = group_i >> 1;
    long long addr = 0;
    char addr_string[20];
    addr_string[0] = '0';
    addr_string[1] = 'x';

    struct cache_group *my_cg =
        (struct cache_group *)malloc(sizeof(struct cache_group) * group);
    cache_group_init(my_cg, associate, group);

    if (vflag == 0) {
        while (fgets(str, sizeof(str), tracefile) != NULL) {

            //printf("%s\n", str);
            if (str[0] != ' ')
                continue;
            int i = 3;
            while (str[i] != ',')
                i++;
            strncpy(addr_string + 2, str + 3, i - 3);
            addr_string[i - 1] = '\0';
            sscanf(addr_string, "%llx", &addr);
            long long cache_flag = addr >> (block + group_index);
            // printf("hi\n");
            int cache_group_index = (addr >> block) % group;
            int str_index;
            for (str_index = 0;
                 str[str_index] != '\n' && str[str_index] != '\0'; ++str_index)
                ;
            str[str_index] = '\0';
            //char str1[20];
            //strcpy(str1,str+1);
            //strcpy(str,str1);
            memmove(str,str+1,sizeof(str)-1);
            if (str[0] == 'L' || str[0] == 'S') {
                cache_access(my_cg + cache_group_index, cache_flag);
            } else {
                cache_access(my_cg + cache_group_index, cache_flag);
                cache_access(my_cg + cache_group_index, cache_flag);
            }
        }
    } else {
        while (fgets(str, sizeof(str), tracefile) != NULL) {

            if (str[0] != ' ')
                continue;
            int i = 3;
            while (str[i] != ',')
                i++;
            strncpy(addr_string + 2, str + 3, i - 3);
            addr_string[i - 1] = '\0';
            sscanf(addr_string, "%llx", &addr);
            int cache_flag = addr >> (block + group_index);
            int cache_group_index = (addr >> block) % group;
            int str_index;
            for (str_index = 0;
                 str[str_index] != '\n' && str[str_index] != '\0'; ++str_index)
                ;
            str[str_index] = '\0';
            memmove(str,str+1,sizeof(str)-1);
            if (str[0] == 'L' || str[0] == 'S') {
                cache_access(my_cg + cache_group_index, cache_flag);
                printf("%s%s\n", str, result);
            } else {
                cache_access(my_cg + cache_group_index, cache_flag);
                printf("%s%s", str, result);
                cache_access(my_cg + cache_group_index, cache_flag);
                printf("%s\n", result);
            }
        }
    }

    printSummary(hits, misses, eviction);
    return 0;
}
