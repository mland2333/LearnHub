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

#define HDRP(bp)            ((char*)(bp) - WSIZE)
#define FTRP(bp)            ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp)       ((char *) (bp) + GET_SIZE(HDRP(bp))) 
#define PREV_BLKP(bp)       ((char *) (bp) - GET_SIZE(((char *) (bp)-DSIZE)))

#define NEXT_FREE(bp)       (GET(bp))                      
#define PREV_FREE(bp)       (GET(bp + WSIZE))

#define SET_NEXT(bp, val)   (PUT(bp, val))
#define SET_PREV(bp, val)   (PUT(bp+WSIZE, val))
#define GET_NEXT(bp)        ((unsigned int)NEXT_BLKP(bp))
#define GET_PREV(bp)        ((unsigned int)PREV_BLKP(bp))
#define CHUNKSIZE (1<<12)


void *head_listp;
void *free_head;
/* 
 * mm_init - initialize the malloc package.
 */
void* coalesec(void* bp)
{
    //printf("beforecoale\n");
    size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
    //printf("beforecoale\n");
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    unsigned int prev_ptr = (unsigned int)PREV_BLKP(bp);
    unsigned int next_ptr = (unsigned int)NEXT_BLKP(bp);
    size_t size = GET_SIZE(HDRP(bp));
    //printf("beforecoale\n");
    if(prev_alloc && next_alloc)
        return bp;
    else if(prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        SET_PREV(GET_PREV(GET_NEXT(bp)), next_ptr);
        SET_NEXT(next_ptr, prev_ptr);
    }
    else if(!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    //printf("aftercoale\n");
    return bp;
}

void *extend_heap(size_t words)
{
   // printf("enter extend ");
    char *bp;
    size_t size;
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    PUT(HDRP(bp), PACK(size, 0));/* Free block header */
    PUT(FTRP(bp), PACK(size, 0));/* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    //printf("extend:%d,size:%d\n", a++, size);
    SET_NEXT(bp, (unsigned int)free_head);
    SET_PREV(bp, (unsigned int)0);
    SET_PREV(free_head, (unsigned int)bp);
    free_head = bp;
    return coalesec(bp);
}

static void *find_fit(size_t size)
{
    //printf("enter find"); 
    void* head = head_listp ;
     while(GET_SIZE(HDRP(head))!=0)
     {
         //printf(" find size : %d\n", GET_SIZE(HDRP(head))); 
         if((!GET_ALLOC(HDRP(head))) && (GET_SIZE(HDRP(head)) >=size))
              return head;
          head += GET_SIZE(HDRP(head));
     }
     //printf(" No find\n");
     return NULL;
}

static void place(void* bp, size_t size)
{
    size_t remain_size, bp_size = GET_SIZE(HDRP(bp));
    
    if((remain_size = bp_size - size) >= 2*DSIZE)
    {
         
         PUT(HDRP(bp), PACK(size, 1));
         PUT(FTRP(bp), PACK(size, 1));
         bp += size;
         
         PUT(HDRP(bp), PACK(remain_size, 0));
         
         PUT(FTRP(bp), PACK(remain_size, 0));
         //printf("diveplace:%d\n", c);
    }
    else
    {
        //printf("normalplace\n");
         PUT(HDRP(bp), PACK(bp_size, 1));
         PUT(FTRP(bp), PACK(bp_size, 1));
    }
    //printf("afterplace:%d\n", c++);
    return;
}

int mm_init(void)
{
    if((head_listp = mem_sbrk(4*WSIZE)) == (void*)-1)
        return -1;
    PUT(head_listp, 0);
    PUT(head_listp + WSIZE, PACK(DSIZE, 1));
    PUT(head_listp + 2*WSIZE, PACK(DSIZE, 1));
    PUT(head_listp+3*WSIZE, PACK(0, 1));

    head_listp += DSIZE;

    if((head_listp = extend_heap(CHUNKSIZE/WSIZE)) == NULL)
       return -1;
    //printf("heap address:%p\n", mem_heap_lo());
    //printf("mem address:%p\n", head_listp);
    SET_NEXT(head_listp, 0);
    SET_PREV(head_listp, 0);
    free_head = head_listp;
    //SET_PREV(head_listp, 0);
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize;
    if(size == 0) return NULL;
    if(size < DSIZE)
        newsize = 2*DSIZE;
    else 
        newsize = (size + DSIZE + DSIZE - 1) & ~0X07;
    char *bp;
    //printf("enter malloc ");
    if((bp = find_fit(newsize)) != NULL)
    {
        place(bp, newsize);
        //printf("find malloc:%d,size:%d, address:%p\n",b++,newsize,bp);
        return bp;
    }

    
    size_t extendsize = newsize > CHUNKSIZE?newsize:CHUNKSIZE;
    if((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, newsize);

     //printf("malloc:%d,size:%d, address:%p\n",b++,newsize,bp);
    return bp;
    /*int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }*/
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if(ptr == NULL)
        return;
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    SET_NEXT(ptr, (unsigned int)free_head);
    SET_PREV(ptr, (unsigned int)0);
    SET_PREV(free_head, (unsigned int)ptr);
    free_head = ptr;
    
    coalesec(ptr);
     //printf("free, size:%d, address:%p\n", size, ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    /*void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;*/

    if(ptr == NULL)
        return mm_malloc(size);
    if(size == 0)
    {
        mm_free(ptr);
        return NULL;
    }

    void* old_ptr = ptr;
    size_t ptr_size = GET_SIZE(HDRP(ptr));
    //printf(" old_ptr=%p, ptr_size=%d,size=%d\n",ptr, ptr_size,size);
    size_t copy_size = size > ptr_size-2*WSIZE ? ptr_size-2*WSIZE:size;
    
    //size += 2 * WSIZE;
    //size_t copy_size = (size > ptr_size?ptr_size:size) - 2*WSIZE;

    PUT(HDRP(ptr), PACK(ptr_size, 0));
    PUT(FTRP(ptr), PACK(ptr_size, 0));
    ptr = coalesec(ptr);
    size_t new_ptr_size = GET_SIZE(HDRP(ptr));
    //printf("  new_ptr=%p, ptr_size=%d,size = %d\n",ptr,new_ptr_size,size);
    if(new_ptr_size >=((size + DSIZE + DSIZE - 1) & ~0X07))
    {
        if(size < DSIZE)
            size = 2*DSIZE;
        else 
            size = (size + DSIZE + DSIZE - 1) & ~0X07;
        memcpy(ptr, old_ptr, copy_size);
        
        place(ptr, size);
        //printf("satisfy, ptr=%p, size=%d, copy_size=%d,ptr_size=%d\n",ptr, size,copy_size,GET_SIZE(HDRP(ptr)));
        return ptr;
    }
    else
    {
        if((ptr = mm_malloc(size)) != NULL)
        {
            memcpy(ptr, old_ptr, copy_size);
            //printf("not satisfy, ptr=%p, size=%d,copy_size=%d\n",ptr,GET_SIZE(HDRP(ptr)),copy_size);
            return ptr;
        }
        else
            return NULL;
    }
    
}

