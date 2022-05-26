
 /*
  * mm.c
  *
  * 2000012824 南希
  * 分离的空闲链表——分离适配
  * 函数：
 extend_heap(size_t words);
place(void* bp, size_t asize);
find_fit(size_t asize);
coalesce(void* bp);
calloc(size_t nmemb, size_t size);
以下是自己加的
recoalese(void* bp,size_t need_size);
用于realloc函数，看能否直接合并后面的空闲块来满足要求
putfree(void* bp);
将这个块加入到空闲链表中
void deletefree(void* bp);
将这个块从空闲链表中删除
getstart(size_t asize);
用于找到对应的空闲链表的开始位置

对于已分配的块，有头部和脚部
对于未分配的块，有头部、脚部、以及PRED和SUCC，
其中PRED放在距离有效载荷起始位置偏移量为0的位置
SUCC放在距离有效载荷起始位置偏移量为8的位置
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

  /* If you want debugging output, use the following macro.  When you hand
   * in, remove the #define DEBUG line. */
#define DEBUG
#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

   /* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7)

 /* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/*
 * If NEXT_FIT defined use next fit search, else use first-fit search
 */
#define NEXT_FITx

 /* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ 
#define DSIZE       8       /* Double word size (bytes) */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */  

#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc)) 

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            
#define PUT(p, val)  (*(unsigned int *)(p) = (val))    
#define GETADD(p)    (*(void **)(p))
#define PUTADD(p,addr)    (*(void **)(p) = (addr)) 
/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)                   
#define GET_ALLOC(p) (GET(p) & 0x1)                    

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)                      
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) 
#define GETPRED(bp)    (*(void **)(bp))
#define GETSUCC(bp)    (*(void **)((char*)(bp)+DSIZE))
#define SETPRED(bp,addr)    (*(void **)(bp)=(addr))
#define SETSUCC(bp,addr)    (*(void **)((char*)(bp)+DSIZE)=(addr))
/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) 
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) 

/* Global variables */
static char* heap_listp = 0;  /* Pointer to first block */
#ifdef NEXT_FIT
static char* rover;           /* Next fit rover */
#endif

/* Function prototypes for internal helper routines */
static void* extend_heap(size_t words);
static void place(void* bp, size_t asize);
static void* find_fit(size_t asize);
static void* coalesce(void* bp);

//以下是自己加的
void* calloc(size_t nmemb, size_t size);
static void* recoalese(void* bp,size_t need_size);
static void putfree(void* bp);
static void deletefree(void* bp);
static void* getstart(size_t asize);
/*
 * calloc - initialize the block which is malloced
 */
void* calloc(size_t nmemb, size_t size)
{
    size_t bytes = nmemb * size;
    void* newptr;

    newptr = malloc(bytes);
    memset(newptr, 0, bytes);
    mm_checkheap(__LINE__);
    return newptr;

}
/*
 * mm_init - Initialize the memory manager
 * set each position of the first unallocated block of some size as NULL 
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(24 * WSIZE)) == (void*)-1)
        return -1;
    PUT(heap_listp, 0); /* Alignment padding */

    /* 对应大小的链表的第一个块的地址 */
    PUTADD(heap_listp + (1 * WSIZE), NULL);//0-31 bytes
    PUTADD(heap_listp + (3 * WSIZE), NULL);//32-63 bytes
    PUTADD(heap_listp + (5 * WSIZE), NULL);//64-127 bytes
    PUTADD(heap_listp + (7 * WSIZE), NULL);//128-255 bytes
    PUTADD(heap_listp + (9 * WSIZE), NULL);//256-511 bytes
    PUTADD(heap_listp + (11 * WSIZE), NULL);//512-1023 bytes
    PUTADD(heap_listp + (13 * WSIZE), NULL);//1024-2047 bytes
    PUTADD(heap_listp + (15 * WSIZE), NULL);//2048-4095 bytes
    PUTADD(heap_listp + (17 * WSIZE), NULL);//4096-8191 bytes
    PUTADD(heap_listp + (19 * WSIZE), NULL);//8192-无穷 bytes
    

    PUT(heap_listp + (21 * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (22 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (23 * WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += (1 * WSIZE);//在存放链表的开始处

#ifdef NEXT_FIT
    rover = heap_listp;
#endif

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    //mm_checkheap(__LINE__);
    return 0;
}

/*
 * malloc - Allocate a block with at least size bytes of payload
 */
void* malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    void* bp;

    if (heap_listp == 0) {
        mm_init();
    }
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= 2 * DSIZE)
        asize = 3 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE)+(DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        //mm_checkheap(__LINE__);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    //mm_checkheap(__LINE__);
    return bp;
}

/*
 * free - Free a block
 */
void free(void* bp)
{
    if (bp == 0)
        return;

    size_t size = GET_SIZE(HDRP(bp));
    if (heap_listp == 0) {
        mm_init();
    }

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
    //mm_checkheap(__LINE__);
}

/*
 * realloc 
 * 调用了recoalese函数进行了一些初级的合并
 */
void* realloc(void* ptr, size_t size)
{
    size_t oldsize;
    void* newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0) {
        free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL) {
        return malloc(size);
    }
    //先判断能不能直接合并这个块后面的块得到想要的大小
    
    if ((newptr=recoalese(ptr,size))!=NULL)
    {
        //mm_checkheap(__LINE__);
        return newptr;
    }
    //如果后面块的大小不够大，则重新分配一个地方
    else
    {
        newptr = malloc(size);

        /* If realloc() fails the original block is left untouched  */
        if (!newptr) {
            return 0;
        }

        /* Copy the old data. */
        oldsize = GET_SIZE(HDRP(ptr));
        if (size < oldsize) oldsize = size;
        memcpy(newptr, ptr, oldsize);

        /* Free the old block. */
        free(ptr);
        //mm_checkheap(__LINE__);
        return newptr;
    }
}


/*
 * The remaining routines are internal helper routines
 */

 /*
  * extend_heap - Extend heap with free block and return its block pointer
  */
static void* extend_heap(size_t words)
{
    char* bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    /* Coalesce if the previous block was free */
    //mm_checkheap(__LINE__);
    return coalesce(bp);
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 * 遇到前后有空闲块的情况，先将这些空闲块从对应的空闲链表中删除
 * 将合并后的空闲块加入到空闲链表中
 */
static void* coalesce(void* bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */         
        putfree(bp);        
        //mm_checkheap(__LINE__);
        return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */        
        deletefree(NEXT_BLKP(bp));//把后面的空闲块从空闲链表中删除
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));        
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));        
        putfree(bp);
        //将这个合并了的空闲块加进去        
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */        
        deletefree(PREV_BLKP(bp));//把前面的空闲块从空闲链表中删除
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);         
        putfree(bp);        
    }

    else {     
        /* Case 4 */
        deletefree(PREV_BLKP(bp));//把前面的空闲块从空闲链表中删除        
        deletefree(NEXT_BLKP(bp));//把后面的空闲块从空闲链表中删除
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
            GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);         
        putfree(bp);
    }

    //mm_checkheap(__LINE__);
    return bp;
}

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 * 将这个空闲块从空闲链表中删除
 * 如果还有富裕的地方将它添加到对应的空闲链表处
 * 并且对块的头部脚步进行标记
 */
static void place(void* bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (3 * DSIZE)) {
        //将这个块分成两部分，第一部分放置，设置头部脚部，
        //并将这个空块从对应的空闲链表中删除
        if(csize>=24)//只有大于等于24字节的空闲块才会被放进空闲链表
            deletefree(bp);
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        //下一个块为空，要将它的标记设置，并且将这个空块放回对应的空闲链表中
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
        putfree(bp);//将这个空块放回对应空闲链表中
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        if (csize >= 24)
            deletefree(bp);//将这个块完全分配，这个块不再空闲，从空闲链表中回收
    }
    //mm_checkheap(__LINE__);
}

/*
 * find_fit - Find a fit for a block with asize bytes
 * 先找到合适的大小的空闲链表的起始处，遍历这个链表，如果这个链表到头都没有找到
 * 就去遍历大小范围更大的链表
 * 如果找到了合适大小的空闲块，就返回他，否则返回NULL
 */
static void* find_fit(size_t asize)
{

    /* First-fit search */
    char* start = (char*)(getstart(asize));//从位置在start的这个链表开始找起
    char* end = (char*)(heap_listp + (18 * WSIZE));//最后一个链表的位置    

    while (start <= end)
    {
        void* bp = GETADD(start);//得到这个地址的内容
        //这个内容是一个指针，是符合这个大小区间的第一个空闲块的块指针
        while (bp != NULL && GET_SIZE(HDRP(bp)) < asize)
        {
            bp = GETSUCC(bp);
        }
        if (bp != NULL)
        {            
            return bp;
        }
        start = (char*)((start) + DSIZE);
    }
    //mm_checkheap(__LINE__);
    return NULL;

}

/*
* 先比较需要的大小是否比这个块原来的大小小，如果是的，那么直接考虑截断这个块
* 如果有足够大小的剩余部分，那么就将它添加到对应的空闲链表中，并且返回bp
* 否则看这个块的后面是不是空闲块，如果是的，
那么看如果加上这个空闲块大小是否满足要求
如果满足要求，那么就把这个空闲块从空闲链表中删除，分配合适的大小，
将剩下的部分继续添加到对应大小的空闲链表中,返回bp
否则返回NULL
*/
static void* recoalese(void* bp, size_t need_size)
{
    size_t asize;
    if (need_size <= 2 * DSIZE)
        asize = 3 * DSIZE;
    else
        asize = DSIZE * ((need_size + (DSIZE)+(DSIZE - 1)) / DSIZE);
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    if (next_alloc) {            //后面的被分配了
        if (asize <= size)
            //如果这个需要的大小比原来的块大小还要小，那么可能需要分割
        {
            if ((size - asize) >= (3 * DSIZE))//需要分割
            {
                PUT(HDRP(bp), PACK(asize, 1));
                PUT(FTRP(bp), PACK(asize, 1));
                bp = NEXT_BLKP(bp);
                PUT(HDRP(bp), PACK(size - asize, 0));
                PUT(FTRP(bp), PACK(size - asize, 0));
                putfree(bp);//将这个空块放回对应空闲链表中
                //mm_checkheap(__LINE__);
                return PREV_BLKP(bp);
            }
            else
            {
                //mm_checkheap(__LINE__);
                return bp;//直接返回不用管
            }
        }
        else
        {    
            //mm_checkheap(__LINE__);
            return NULL;//后面的被分配了，这个块现在的大小也小于需要的大小
        }
    }
    else//后面的没有被分配
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        if (size >= asize)
        {       
            deletefree(NEXT_BLKP(bp));
            //后面的被占用了，要从对应的空闲链表中删掉它
            if ((size - asize) >= (3 * DSIZE))//需要分割
            {
                PUT(HDRP(bp), PACK(asize, 1));
                PUT(FTRP(bp), PACK(asize, 1));
                bp = NEXT_BLKP(bp);
                PUT(HDRP(bp), PACK(size - asize, 0));
                PUT(FTRP(bp), PACK(size - asize, 0));
                putfree(bp);//将这个空块放回对应空闲链表中
                //mm_checkheap(__LINE__);
                return PREV_BLKP(bp);
            }
            else
            {
                PUT(HDRP(bp), PACK(size, 1));
                PUT(FTRP(bp), PACK(size, 1));
                //mm_checkheap(__LINE__);
                return bp;//直接返回不用管
            }
        }
        else//后面的也不够用，只能返回了
        {
            //mm_checkheap(__LINE__);
            return NULL;
        }
    }
}


/*
* 找到在堆中存对应大小空闲链表的块指针的指针，这个指针指向一个bp，
bp为对应大小的空闲链表中的第一个空闲块
*/
static void* getstart(size_t asize)
{
    if( asize < 32)
        return (void*)heap_listp;
    else if (32 <= asize && asize < 64)
        return (void*)(heap_listp + (2 * WSIZE));
    else if (64 <= asize && asize < 128)
        return (void*)(heap_listp + (4 * WSIZE));
    else if (128 <= asize && asize < 256)
        return (void*)(heap_listp + (6 * WSIZE));
    else if (256 <= asize && asize < 512)
        return (void*)(heap_listp + (8 * WSIZE));
    else if (512 <= asize && asize < 1024)
        return (void*)(heap_listp + (10 * WSIZE));
    else if (1024 <= asize && asize < 2048)
        return (void*)(heap_listp + (12 * WSIZE));
    else if (2048 <= asize && asize < 4096)
        return (void*)(heap_listp + (14 * WSIZE));
    else if (4096 <= asize && asize < 8192)
        return (void*)(heap_listp + (16 * WSIZE));
    else if (8192 <= asize)
        return (void*)(heap_listp + (18 * WSIZE));
    return NULL;
}

/*
* 将这个块放回到对应的空闲链表中
* 采用放在空闲链表最开始的地方
* 修改对应的空闲块的PRED和SUCC指针
*/
static void putfree(void* bp)//先进先出
{
    size_t size = GET_SIZE(HDRP(bp));
    if (size < 24)
        return;
    void* start = getstart(size);
    void* prevbp = GETADD(start);
    if (prevbp == NULL)
    {
        PUTADD(start, bp);
        SETPRED(bp, start);//对这个空闲块的pred做标记
        SETSUCC(bp, NULL);//对这个空闲块的succ做标记
    }
    else
    {
        SETPRED(prevbp, bp);
        //对原来这个链表里面的第一个空闲块的pred做标记，标记为这个新加的块
        SETSUCC(bp, prevbp);
        //对新加的块的succ做标记，标记为原来的这个链表中的第一个空闲块
        PUTADD(start, bp);
    }
    //mm_checkheap(__LINE__);
}

/*
* 将一个空闲块从对应的空闲链表中删除
* 修改对应的空闲块的PRED和SUCC指针
*/
static void deletefree(void* bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    if (size < 24)
        return;
    void* start = getstart(size);
    void* prevbp = GETADD(start);
    if (bp==prevbp)//是这个空闲链表的头
    {
        PUTADD(start, GETSUCC(bp));
        if(GETSUCC(bp)!=NULL)
            SETPRED(GETSUCC(bp), start);
    }
    else
    {
        void* bp1 = GETPRED(bp);
        void* bp2 = GETSUCC(bp);
        SETSUCC(bp1, bp2);
        if (bp2 != NULL)
            SETPRED(bp2, bp1);
    }
    //mm_checkheap(__LINE__);
}

//for debugging


/*
 * Return whether the pointer is in the heap.
 * May be useful for debugging.
 */
static int in_heap(const void* p) {
    return p <= mem_heap_hi() && p >= mem_heap_lo();
}

/*
 * Return whether the pointer is aligned.
 * May be useful for debugging.
 */
static int aligned(const void* p) {
    return (size_t)ALIGN(p) == (size_t)p;
}

/*
 * mm_checkheap - Check the heap for correctness. Helpful hint: You
 *                can call this function using mm_checkheap(__LINE__);
 *                to identify the line number of the call site.
 */
void mm_checkheap(int lineno)
{
    int cnt = 0;
    for (void* bp = (void*)(heap_listp+(23*WSIZE)); GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (mem_heap_lo() >= bp || bp >= mem_heap_hi())
        {
            printf("a block is out of the heap range\n");
            exit(0);
        }
        if (GET_SIZE(HDRP(bp)) != GET_SIZE(FTRP(bp))) {
            printf("%d\n", lineno);
            printf("HDRP:%d\n", GET_SIZE(HDRP(bp)));
            printf("FTRP:%d\n", GET_SIZE(FTRP(bp)));
            printf("the size in head is not equal to the size in foot\n");
            exit(0);
        }
        if (GET_ALLOC(HDRP(bp)) != GET_ALLOC(FTRP(bp))) {
            printf("the alloc in head is not equal to the alloc in foot\n");
            exit(0);
        }
        if (GET_SIZE(HDRP(bp)) % 8 != 0)
        {
            printf("bad align\n");
            exit(0);
        }
        if (GET_SIZE(HDRP(bp)) < 24&&!GET_ALLOC(HDRP(bp)))
        {
            printf("%d\n", lineno);
            printf("no enough space for unallocated block\n");
            exit(0);
        }
        if (GET_SIZE(HDRP(bp)) < 16)
        {
            printf("no enough space for a block\n");
            exit(0);
        }
        if (!GET_ALLOC(HDRP(bp)))
        {
            if(GET_SIZE(HDRP(NEXT_BLKP(bp))) > 0)
                if (!GET_ALLOC(HDRP(NEXT_BLKP(bp))))
                {
                    printf("not to put together two unallocated blocks together\n");
                    exit(0);
                }
        }
        if (!GET_ALLOC(HDRP(bp)))
            cnt++;
    }
    for (int i = 0; i < 10; i++)
    {
        char* start = (char*)(heap_listp)+DSIZE * i;
        void* bp = GETADD(start);//得到这个地址的内容
        //这个内容是一个指针，是符合这个大小区间的第一个空闲块的块指针
        while (bp != NULL)
        {
            cnt--;
            if (GET_ALLOC(HDRP(bp)))
            {
                printf("%d\n", lineno);
                printf("an allocated block is put in free block list\n");
                if (bp == GETADD(start))
                    printf("in the beginning of the list\n");
                else
                    printf("not in the beginning of the list\n");
                exit(0);
            }
            void* succbp = GETSUCC(bp);
            if (succbp != NULL)
            {
                if (GETPRED(succbp) != bp)
                {
                    printf("mismatch the pred of a free block's succ and it\n");
                    exit(0);
                }
            }
            if (mem_heap_lo() >= bp || bp >= mem_heap_hi())
            {
                printf("a block in free block list is out of the heap range\n");
                exit(0);
            }
            bp = GETSUCC(bp);
        }        
    }
    if (cnt > 0)
    {
        printf("%d\n", lineno);
        printf("some unallocated block not in the free block list\n");
        exit(0);
    }
    else if(cnt<0)
    {
        printf("%d\n", lineno);
        printf("some unallocated block in the free block list is not deleted\n");
        exit(0);
    }
}

