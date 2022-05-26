
 /*
  * mm.c
  *
  * 2000012824 ��ϣ
  * ����Ŀ�����������������
  * ������
 extend_heap(size_t words);
place(void* bp, size_t asize);
find_fit(size_t asize);
coalesce(void* bp);
calloc(size_t nmemb, size_t size);
�������Լ��ӵ�
recoalese(void* bp,size_t need_size);
����realloc���������ܷ�ֱ�Ӻϲ�����Ŀ��п�������Ҫ��
putfree(void* bp);
���������뵽����������
void deletefree(void* bp);
�������ӿ���������ɾ��
getstart(size_t asize);
�����ҵ���Ӧ�Ŀ�������Ŀ�ʼλ��

�����ѷ���Ŀ飬��ͷ���ͽŲ�
����δ����Ŀ飬��ͷ�����Ų����Լ�PRED��SUCC��
����PRED���ھ�����Ч�غ���ʼλ��ƫ����Ϊ0��λ��
SUCC���ھ�����Ч�غ���ʼλ��ƫ����Ϊ8��λ��
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

//�������Լ��ӵ�
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

    /* ��Ӧ��С������ĵ�һ����ĵ�ַ */
    PUTADD(heap_listp + (1 * WSIZE), NULL);//0-31 bytes
    PUTADD(heap_listp + (3 * WSIZE), NULL);//32-63 bytes
    PUTADD(heap_listp + (5 * WSIZE), NULL);//64-127 bytes
    PUTADD(heap_listp + (7 * WSIZE), NULL);//128-255 bytes
    PUTADD(heap_listp + (9 * WSIZE), NULL);//256-511 bytes
    PUTADD(heap_listp + (11 * WSIZE), NULL);//512-1023 bytes
    PUTADD(heap_listp + (13 * WSIZE), NULL);//1024-2047 bytes
    PUTADD(heap_listp + (15 * WSIZE), NULL);//2048-4095 bytes
    PUTADD(heap_listp + (17 * WSIZE), NULL);//4096-8191 bytes
    PUTADD(heap_listp + (19 * WSIZE), NULL);//8192-���� bytes
    

    PUT(heap_listp + (21 * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (22 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (23 * WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += (1 * WSIZE);//�ڴ������Ŀ�ʼ��

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
 * ������recoalese����������һЩ�����ĺϲ�
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
    //���ж��ܲ���ֱ�Ӻϲ���������Ŀ�õ���Ҫ�Ĵ�С
    
    if ((newptr=recoalese(ptr,size))!=NULL)
    {
        //mm_checkheap(__LINE__);
        return newptr;
    }
    //��������Ĵ�С�����������·���һ���ط�
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
 * ����ǰ���п��п��������Ƚ���Щ���п�Ӷ�Ӧ�Ŀ���������ɾ��
 * ���ϲ���Ŀ��п���뵽����������
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
        deletefree(NEXT_BLKP(bp));//�Ѻ���Ŀ��п�ӿ���������ɾ��
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));        
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));        
        putfree(bp);
        //������ϲ��˵Ŀ��п�ӽ�ȥ        
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */        
        deletefree(PREV_BLKP(bp));//��ǰ��Ŀ��п�ӿ���������ɾ��
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);         
        putfree(bp);        
    }

    else {     
        /* Case 4 */
        deletefree(PREV_BLKP(bp));//��ǰ��Ŀ��п�ӿ���������ɾ��        
        deletefree(NEXT_BLKP(bp));//�Ѻ���Ŀ��п�ӿ���������ɾ��
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
 * ��������п�ӿ���������ɾ��
 * ������и�ԣ�ĵط�������ӵ���Ӧ�Ŀ�������
 * ���ҶԿ��ͷ���Ų����б��
 */
static void place(void* bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (3 * DSIZE)) {
        //�������ֳ������֣���һ���ַ��ã�����ͷ���Ų���
        //��������տ�Ӷ�Ӧ�Ŀ���������ɾ��
        if(csize>=24)//ֻ�д��ڵ���24�ֽڵĿ��п�Żᱻ�Ž���������
            deletefree(bp);
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        //��һ����Ϊ�գ�Ҫ�����ı�����ã����ҽ�����տ�Żض�Ӧ�Ŀ���������
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
        putfree(bp);//������տ�Żض�Ӧ����������
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        if (csize >= 24)
            deletefree(bp);//���������ȫ���䣬����鲻�ٿ��У��ӿ��������л���
    }
    //mm_checkheap(__LINE__);
}

/*
 * find_fit - Find a fit for a block with asize bytes
 * ���ҵ����ʵĴ�С�Ŀ����������ʼ�������������������������ͷ��û���ҵ�
 * ��ȥ������С��Χ���������
 * ����ҵ��˺��ʴ�С�Ŀ��п飬�ͷ����������򷵻�NULL
 */
static void* find_fit(size_t asize)
{

    /* First-fit search */
    char* start = (char*)(getstart(asize));//��λ����start���������ʼ����
    char* end = (char*)(heap_listp + (18 * WSIZE));//���һ�������λ��    

    while (start <= end)
    {
        void* bp = GETADD(start);//�õ������ַ������
        //���������һ��ָ�룬�Ƿ��������С����ĵ�һ�����п�Ŀ�ָ��
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
* �ȱȽ���Ҫ�Ĵ�С�Ƿ�������ԭ���Ĵ�СС������ǵģ���ôֱ�ӿ��ǽض������
* ������㹻��С��ʣ�ಿ�֣���ô�ͽ�����ӵ���Ӧ�Ŀ��������У����ҷ���bp
* ���������ĺ����ǲ��ǿ��п飬����ǵģ�
��ô���������������п��С�Ƿ�����Ҫ��
�������Ҫ����ô�Ͱ�������п�ӿ���������ɾ����������ʵĴ�С��
��ʣ�µĲ��ּ�����ӵ���Ӧ��С�Ŀ���������,����bp
���򷵻�NULL
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
    if (next_alloc) {            //����ı�������
        if (asize <= size)
            //��������Ҫ�Ĵ�С��ԭ���Ŀ��С��ҪС����ô������Ҫ�ָ�
        {
            if ((size - asize) >= (3 * DSIZE))//��Ҫ�ָ�
            {
                PUT(HDRP(bp), PACK(asize, 1));
                PUT(FTRP(bp), PACK(asize, 1));
                bp = NEXT_BLKP(bp);
                PUT(HDRP(bp), PACK(size - asize, 0));
                PUT(FTRP(bp), PACK(size - asize, 0));
                putfree(bp);//������տ�Żض�Ӧ����������
                //mm_checkheap(__LINE__);
                return PREV_BLKP(bp);
            }
            else
            {
                //mm_checkheap(__LINE__);
                return bp;//ֱ�ӷ��ز��ù�
            }
        }
        else
        {    
            //mm_checkheap(__LINE__);
            return NULL;//����ı������ˣ���������ڵĴ�СҲС����Ҫ�Ĵ�С
        }
    }
    else//�����û�б�����
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        if (size >= asize)
        {       
            deletefree(NEXT_BLKP(bp));
            //����ı�ռ���ˣ�Ҫ�Ӷ�Ӧ�Ŀ���������ɾ����
            if ((size - asize) >= (3 * DSIZE))//��Ҫ�ָ�
            {
                PUT(HDRP(bp), PACK(asize, 1));
                PUT(FTRP(bp), PACK(asize, 1));
                bp = NEXT_BLKP(bp);
                PUT(HDRP(bp), PACK(size - asize, 0));
                PUT(FTRP(bp), PACK(size - asize, 0));
                putfree(bp);//������տ�Żض�Ӧ����������
                //mm_checkheap(__LINE__);
                return PREV_BLKP(bp);
            }
            else
            {
                PUT(HDRP(bp), PACK(size, 1));
                PUT(FTRP(bp), PACK(size, 1));
                //mm_checkheap(__LINE__);
                return bp;//ֱ�ӷ��ز��ù�
            }
        }
        else//�����Ҳ�����ã�ֻ�ܷ�����
        {
            //mm_checkheap(__LINE__);
            return NULL;
        }
    }
}


/*
* �ҵ��ڶ��д��Ӧ��С��������Ŀ�ָ���ָ�룬���ָ��ָ��һ��bp��
bpΪ��Ӧ��С�Ŀ��������еĵ�һ�����п�
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
* �������Żص���Ӧ�Ŀ���������
* ���÷��ڿ��������ʼ�ĵط�
* �޸Ķ�Ӧ�Ŀ��п��PRED��SUCCָ��
*/
static void putfree(void* bp)//�Ƚ��ȳ�
{
    size_t size = GET_SIZE(HDRP(bp));
    if (size < 24)
        return;
    void* start = getstart(size);
    void* prevbp = GETADD(start);
    if (prevbp == NULL)
    {
        PUTADD(start, bp);
        SETPRED(bp, start);//��������п��pred�����
        SETSUCC(bp, NULL);//��������п��succ�����
    }
    else
    {
        SETPRED(prevbp, bp);
        //��ԭ�������������ĵ�һ�����п��pred����ǣ����Ϊ����¼ӵĿ�
        SETSUCC(bp, prevbp);
        //���¼ӵĿ��succ����ǣ����Ϊԭ������������еĵ�һ�����п�
        PUTADD(start, bp);
    }
    //mm_checkheap(__LINE__);
}

/*
* ��һ�����п�Ӷ�Ӧ�Ŀ���������ɾ��
* �޸Ķ�Ӧ�Ŀ��п��PRED��SUCCָ��
*/
static void deletefree(void* bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    if (size < 24)
        return;
    void* start = getstart(size);
    void* prevbp = GETADD(start);
    if (bp==prevbp)//��������������ͷ
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
        void* bp = GETADD(start);//�õ������ַ������
        //���������һ��ָ�룬�Ƿ��������С����ĵ�һ�����п�Ŀ�ָ��
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

