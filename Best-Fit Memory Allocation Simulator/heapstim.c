#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_MEM 1024
#define MIN_BLOCK_SIZE 16
typedef struct Block
{
    size_t size;
    int free;
    struct Block *next;
} Block;

Block *free_list = NULL;

void init_memory()
{
    free_list = (Block *)malloc(sizeof(Block));
    if (!free_list)
    {
        printf("Memory initialization failed!\n");
        exit(1);
    }
    free_list->size = MAX_MEM;
    free_list->free = 1;
    free_list->next = NULL;
}

void *allocate_best_fit(size_t size)
{
    
    if (size < MIN_BLOCK_SIZE) size = MIN_BLOCK_SIZE;
    size = (size + sizeof(Block) + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE * MIN_BLOCK_SIZE;

    Block *best = NULL, *curr = free_list;
    while (curr)
    {
        if (curr->free && curr->size >= size)
        {
            if (!best || curr->size < best->size)
            {
                best = curr;
            }
        }
        curr = curr->next;
    }

    if (!best)
    {
        printf("No suitable block found!\n");
        return NULL;
    }

    if (best->size > size + sizeof(Block))
    {
        Block *new_block = (Block *)((char *)best + size + sizeof(Block));
        new_block->size = best->size - size - sizeof(Block);
        new_block->free = 1;
        new_block->next = best->next;
        best->next = new_block;
    }

    best->size = size;
    best->free = 0;
    return (void *)(best + 1);
}

void free_memory(void *ptr)
{
    if (!ptr)
        return;

    Block *block = (Block *)ptr - 1;
    block->free = 1;

    Block *curr = free_list;
    while (curr && curr->next)
    {
        if (curr->free && curr->next->free &&
            (char *)curr + curr->size + sizeof(Block) == (char *)curr->next)
        {

            curr->size += curr->next->size + sizeof(Block);
            curr->next = curr->next->next;
        }
        else
        {
            curr = curr->next;
        }
    }
}

void display_memory()
{
    Block *curr = free_list;
    while (curr)
    {
        printf("%p",curr);
        printf("[Size: %lu, Free: %d] -> ", curr->size, curr->free);
        curr = curr->next;
    }
    printf("NULL\n");
}

int main()
{
    init_memory();
    int choice;
    size_t size;
    void *allocations[100] = {NULL};
    int index = 0;

    while (1)
    {
        printf("\nMemory Management System\n");
        printf("1. Allocate Best Fit\n");
        printf("2. Free Memory\n");
        printf("3. Display Memory\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter size to allocate: ");
            scanf("%lu", &size);
            allocations[index++] = allocate_best_fit(size);
            break;
        case 2:
            printf("Enter index to free\n");
            int i;
            scanf("%d", &i);

            if (i >= 0 && i < index && allocations[i])
            {
                free_memory(allocations[i]);
                allocations[i] = NULL;

                if (i == index - 1)
                {
                    while (index > 0 && allocations[index - 1] == NULL)
                    {
                        index--; 
                    }
                }
            }
            else
            {
                printf("Invalid index!\n");
            }

        case 3:
            display_memory();
            break;
        case 4:
            exit(0);
        default:
            printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}
