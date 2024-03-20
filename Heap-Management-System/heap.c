#include <stdio.h>

#define HEAP_SIZE 1024

char heap[HEAP_SIZE];

typedef struct FreeHeader
{
    unsigned int size;
    struct FreeHeader * next;
    struct FreeHeader * prev;
} FreeHeader;

typedef unsigned int AllocHeader;


FreeHeader * FreeList = (void *) heap;


void InitHeap()
{
    FreeList -> size = HEAP_SIZE;
    FreeList -> next = NULL;
    FreeList -> prev = NULL;
}


void printFreeList()
{
    FreeHeader * ptr = FreeList;

    printf("\nFree List:\n");

    if (ptr != NULL)
    {
        while (ptr -> next != NULL)
        {
            printf("(%llu, ", ptr);
            printf("%u) -> ", ptr -> size);

            ptr = ptr -> next;
        }

        printf("(%llu, ", ptr);
        printf("%u)\n\n", ptr -> size);
    }
}


void * alloc(size_t reqSize)
{
    static int init = 1;
    if (init) InitHeap();
    init = 0;

    FreeHeader * cur = FreeList;
    AllocHeader * address = NULL;

    while (cur != NULL && cur -> size < reqSize + sizeof(AllocHeader) )
    {
        cur = cur -> next;
    }

    if (cur != NULL)
    {
        if (cur -> size > reqSize + sizeof(AllocHeader) + sizeof(FreeHeader))
        {
            address = (void *) cur + (cur -> size - reqSize - sizeof(AllocHeader));
            *address = reqSize + sizeof(AllocHeader);
            address++;
            cur -> size -= reqSize + sizeof(AllocHeader);
        }
        else
        {
            if (cur -> prev == NULL)
            {
                FreeList = cur -> next;
            }
            else
            {
                cur -> prev -> next = cur -> next;
            }

            address = (void *) cur;
            *address = cur -> size;
            address++;
        }
    }

    if (address != NULL) printf("Allocated: \n%llu\n%u\n", address, *(address - 1));
    else printf("Allocation failed\n");

    printFreeList();
    
    return (void *) address;
}


void free(void * address)
{
    printf("Freeing: %llu\n", address);

    FreeHeader * ptr, * block;
    AllocHeader * sizeptr;

    if (address > (void *) heap && address < (void *) heap + HEAP_SIZE)
    {
        ptr = FreeList;
        sizeptr = address - sizeof(AllocHeader);
        block = (void *) sizeptr;
        block -> size = *sizeptr;

        if (ptr == NULL)
        {
            FreeList = block;
            block -> next = block -> prev = NULL;
        }
        else if (block < ptr)
        {
            FreeList = block;
            block -> next = ptr;
            block -> prev = NULL;

            if ((void *) block + block -> size == (void *) ptr)
            {
                block -> next = ptr -> next;
                block -> size += ptr -> size;
                if (block -> next != NULL) block -> next -> prev = block;
            }
        }
        else
        {
            while (ptr -> next != NULL && block > ptr -> next)
            {
                ptr = ptr -> next;
            }

            if ((void *) ptr + ptr -> size + block -> size == (void *) ptr -> next)
            {
                ptr -> size += block -> size + ptr -> next -> size;
                ptr -> next = ptr -> next -> next;
                if (ptr -> next != NULL) ptr -> next -> prev = ptr;
            }
            else if ((void *) ptr + ptr -> size == (void *) block)
            {
                ptr -> size += block -> size;
            }
            else if ((void *) block + block -> size == (void *) ptr -> next)
            {
                block -> size += ptr -> next -> size;
                block -> next = ptr -> next -> next;
                ptr -> next = block;
                block -> prev = ptr;
                if (block -> next != NULL) block -> next -> prev = block;
            }
            else
            {
                block -> next = ptr -> next;
                block -> prev = ptr;
                ptr -> next = block;
                if (block -> next != NULL) block -> next -> prev = block;
            }
        }
    }
    printFreeList();
}



void main()
{
    printf("sizeof(FreeHeader) = %d\n", sizeof(FreeHeader));
    printf("sizeof(AllocHeader) = %d\n", sizeof(AllocHeader));
    printf("Heap start: %llu\n\n", heap);
    
    char * charptr1 = alloc(sizeof(char)*20);
    char * charptr2 = alloc(sizeof(char)*100);
    char * charptr3 = alloc(sizeof(char)*500);
    char * charptr4 = alloc(sizeof(char)*20);

    free(charptr2);
    free(charptr4);

    char * charptr5 = alloc(sizeof(char)*200);
    char * charptr6 = alloc(sizeof(char)*50);
    char * charptr7 = alloc(sizeof(char)*150);

    free(charptr1);
    free(charptr5);
    free(charptr6);
    free(charptr3);
}