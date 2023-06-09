#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

 team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE               4
#define DSIZE               8

#define MAX(x, y)           ((x) > (y)? (x) : (y))

#define PACK(size, alloc)   ((size) | (alloc))

#define GET(p)              (* (unsigned int *)(p))
#define PUT(p, val)         (* (unsigned int *) (p) = (val))

#define GET_SIZE(p)         (GET(p) & ~0x7)
#define GET_ALLOC(p)        (GET(p) & 0x1)

#define HEAD(bp)            ((char*)(bp) - WSIZE)
#define FOOT(bp)            ((char*)(bp) + GET_SIZE(HEAD(bp)) - DSIZE)

#define NEXT_PA(bp)       ((char *) (bp) + GET_SIZE(HEAD(bp)))
#define PREV_PA(bp)       ((char *) (bp) - GET_SIZE(((char *) (bp)-DSIZE)))



#define SET_PREV_FREE(bp, val)  (PUT(bp, val))
#define SET_NEXT_FREE(bp, val)  (PUT(bp+WSIZE, val)) 
#define GET_PREV_FREE(bp)       (GET(bp))
#define GET_NEXT_FREE(bp)       (GET(bp+WSIZE))

#define CHUNKSIZE (1<<12)
 void *head_listp = 0; 
void print_free(void)
{
    void* head = (void*)GET_PREV_FREE(head_listp);
    printf("free list: %p ",head_listp);
    while(head!=head_listp)
    {
        if(head != NULL)
        {
            printf("%p ", head);
            head = (void*)GET_PREV_FREE(head);
        }
        else
        {
            printf("\n");
            printf("空闲链表中存在NULL\n");
            exit(0);
        }
    }
    printf("\n");
}

               
//void *free_head = 0;

int i = 0;
/* 
 * mm_init - initialize the malloc package.
 */
void delete_node(void* ptr)
{
    
    SET_PREV_FREE(GET_NEXT_FREE(ptr), GET_PREV_FREE(ptr));
    SET_NEXT_FREE(GET_PREV_FREE(ptr), GET_NEXT_FREE(ptr));
    
}

void join_first(void* bp)
{
    
    SET_NEXT_FREE(bp, (unsigned int)head_listp);
    SET_PREV_FREE(bp, GET_PREV_FREE(head_listp));
    SET_NEXT_FREE(GET_PREV_FREE(head_listp), (unsigned int)bp);  
    SET_PREV_FREE(head_listp, (unsigned int)bp);
    
    
}

void* coalesec(void* bp)
{
    
    size_t prev_alloc = GET_ALLOC(HEAD(PREV_PA(bp)));
    size_t next_alloc = GET_ALLOC(HEAD(NEXT_PA(bp)));
    size_t size = GET_SIZE(HEAD(bp));
     
    if(prev_alloc && next_alloc)
    {   
        join_first(bp);
        
    }
    else if(prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HEAD(NEXT_PA(bp)));
        delete_node(NEXT_PA(bp));
        PUT(HEAD(bp), PACK(size, 0));
        PUT(FOOT(bp), PACK(size, 0));
        join_first(bp);
        
    }
    else if(!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HEAD(PREV_PA(bp)));
        PUT(HEAD(PREV_PA(bp)), PACK(size, 0));
        PUT(FOOT(bp), PACK(size, 0));
        bp = PREV_PA(bp);
    }
    else
    {
        delete_node(NEXT_PA(bp));
        size += GET_SIZE(HEAD(PREV_PA(bp))) + GET_SIZE(HEAD(NEXT_PA(bp)));
        PUT(HEAD(PREV_PA(bp)), PACK(size, 0));
        PUT(FOOT(PREV_PA(bp)), PACK(size, 0));
        bp = PREV_PA(bp);
    }
    return bp;
}

void* divice(void* bp, unsigned int use_size, unsigned int free_size)
{
    
    
    PUT(HEAD(bp), PACK(use_size, 1));
    PUT(FOOT(bp), PACK(use_size, 1));
    bp += use_size;
    PUT(HEAD(bp), PACK(free_size, 0));
    PUT(FOOT(bp), PACK(free_size, 0));
    return coalesec(bp);
}

void *extend_heap(size_t words)
{
    
    char *bp;
    size_t size;
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    PUT(HEAD(bp), PACK(size, 0));/* Free block header */
    PUT(FOOT(bp), PACK(size, 0));/* Free block footer */
    PUT(HEAD(NEXT_PA(bp)), PACK(0, 1));
    return coalesec(bp);
}



int mm_init(void)
{
    if((head_listp = mem_sbrk(6*WSIZE)) == (void*)-1)
        return -1;
    PUT(head_listp, 0);
    PUT(head_listp + WSIZE, PACK(2*DSIZE, 1));
    PUT(head_listp + 2*WSIZE, (unsigned int)(head_listp+2*WSIZE));
    PUT(head_listp + 3*WSIZE, (unsigned int)(head_listp+2*WSIZE));
    PUT(head_listp + 4*WSIZE, PACK(2*DSIZE, 1));
    PUT(head_listp + 5*WSIZE, PACK(0, 1));
    
    head_listp += 2*WSIZE;
    
    
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
       return -1;
    return 0;
}

void mm_free(void *ptr)
{
   
    if(ptr == NULL)
        return;
    size_t size = GET_SIZE(HEAD(ptr));
    PUT(HEAD(ptr), PACK(size, 0));
    PUT(FOOT(ptr), PACK(size, 0));
    coalesec(ptr);
}

static void *find_fit(size_t size)
{
    
    
    void* head = (void*)GET_PREV_FREE(head_listp);
    while(head!=head_listp)
    {   
        
        if(GET_SIZE(HEAD(head)) >=size)
           return head;
        head = (void *)GET_PREV_FREE(head);
    }
     return NULL;
}

static void place(void* bp, size_t size)
{
    
    size_t remain_size, bp_size = GET_SIZE(HEAD(bp));
    delete_node(bp);
    if((remain_size = bp_size - size) >= 2*DSIZE)
    {
        divice(bp, size, remain_size);
    }
    else
    {    
        
        PUT(HEAD(bp), PACK(bp_size, 1));
        PUT(FOOT(bp), PACK(bp_size, 1));
    }
    return ;
}

void *mm_malloc(size_t size)
{
    
    int newsize;
    if(size == 0) return NULL;
    if(size < DSIZE)
        newsize = 2*DSIZE;
    else 
        newsize = (size + DSIZE + DSIZE - 1) & ~0X07;
    char *bp;
    if((bp = find_fit(newsize)) != NULL)
    {
        place(bp, newsize);
        return bp ;
    }
    size_t extendsize = newsize > CHUNKSIZE?newsize:CHUNKSIZE;
    if((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, newsize);
    return bp;
}

void *mm_realloc(void *ptr, size_t size)
{
    if(ptr == NULL)
        return mm_malloc(size);
    if(size == 0)
    {
        mm_free(ptr);
        return NULL;                                            
    }
    size_t ptr_size = GET_SIZE(HEAD(ptr));
    size_t real_size = size>DSIZE?(size + DSIZE + DSIZE - 1) & ~0X07:2*DSIZE;
    size_t ptr2_size;
    void* old_ptr = ptr;
    size_t copy_size = size > ptr_size-2*WSIZE ? ptr_size-2*WSIZE:size;
    if(real_size <= ptr_size)
    {
        if((real_size + 2*DSIZE) > ptr_size)
        {   
            return ptr;
        }
        else
        {   
            divice(ptr, real_size, ptr_size-real_size);
            return ptr;
        }
    }
    else
    {
         if(GET_ALLOC(HEAD(NEXT_PA(ptr)))==0&&(ptr2_size = GET_SIZE(HEAD(NEXT_PA(ptr)))+ptr_size)>=real_size)
         {   

             delete_node(NEXT_PA(ptr));
             if((real_size + 2*DSIZE) > ptr2_size)
             {
                
                PUT(HEAD(ptr), PACK(ptr2_size, 1));
                PUT(FOOT(ptr), PACK(ptr2_size, 1));
             }
             else
             {  
                 divice(ptr, real_size, ptr2_size - real_size);
             }
         }
         else if(GET_ALLOC(HEAD(PREV_PA(ptr)))==0&&GET_ALLOC(HEAD(NEXT_PA(ptr)))!=0&&(ptr2_size = GET_SIZE(HEAD(PREV_PA(ptr)))+ptr_size)>=real_size)
         {
             ptr = PREV_PA(ptr);
             delete_node(ptr);
             if((real_size + 2*DSIZE) > ptr2_size)
             {  
                
                memcpy(ptr, old_ptr, copy_size);
                PUT(HEAD(ptr), PACK(ptr2_size, 1));                
                PUT(FOOT(ptr), PACK(ptr2_size, 1));
             }
             else
             {  
                memmove(ptr, old_ptr, copy_size);
                divice(ptr, real_size, ptr2_size - real_size);
                
             }
         }  
         else if(GET_ALLOC(HEAD(PREV_PA(ptr)))==0&&GET_ALLOC(HEAD(NEXT_PA(ptr)))==0&&
                 (ptr2_size =GET_SIZE(HEAD(NEXT_PA(ptr)))+ GET_SIZE(HEAD(PREV_PA(ptr)))+ptr_size)>=real_size)
         {
             delete_node(NEXT_PA(ptr));
             ptr = PREV_PA(ptr);
             delete_node(ptr);
             if((real_size + 2*DSIZE) > ptr2_size)
             {  
                
                memcpy(ptr, old_ptr, copy_size);
                PUT(HEAD(ptr), PACK(ptr2_size, 1));
                PUT(FOOT(ptr), PACK(ptr2_size, 1));
             }
             else
             {  
                memcpy(ptr, old_ptr, copy_size);
                divice(ptr, real_size, ptr2_size - real_size);
                
             }
         }
         else
         {
             if((ptr = mm_malloc(size))!=NULL)
            {   
                memcpy(ptr, old_ptr, copy_size);
                PUT(HEAD(old_ptr), PACK(ptr_size, 0));
                PUT(FOOT(old_ptr), PACK(ptr_size, 0));
                coalesec(old_ptr);
            }
         }
    }
    return ptr;
    
}
