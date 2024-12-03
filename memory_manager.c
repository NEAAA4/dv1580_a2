#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Structure for memory block
typedef struct Memory {
    size_t size;
    size_t starting;
    struct Memory* next;
    int freeing;
} Memory;

// Global variables
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static char* memory_pool = NULL;
static Memory* lista = NULL;
static size_t pool_size = 0;

// Initialize the memory manager
void mem_init(size_t size) {
    pthread_mutex_lock(&lock);
    memory_pool = (char*)malloc(size);
    if (memory_pool == NULL) {
        printf("Memory allocation failed\n");
        pthread_mutex_unlock(&lock);
        return;
    }

    pool_size = size;
    lista = (Memory*)malloc(sizeof(Memory)); // metadata
    if (lista == NULL) {
        printf("Allocation of block list failed\n");
        free(memory_pool);
        memory_pool = NULL;
        pthread_mutex_unlock(&lock);
        return;
    }

    lista->starting = 0; 
    lista->size = size; 
    lista->freeing = 1;  // Block is free
    lista->next = NULL; 
    pthread_mutex_unlock(&lock);
}

// Allocate memory
void* mem_alloc(size_t size) {
    pthread_mutex_lock(&lock);
    Memory* here = lista;
    while (here != NULL) {
        if (here->freeing && here->size >= size) {
            if (here->size > size) { // Split block
                Memory* new = (Memory*)malloc(sizeof(Memory)); // new block
                if (new == NULL) {
                    printf("Allocation for new block failed\n");
                    pthread_mutex_unlock(&lock);
                    return NULL;  
                }
                
                new->starting = here->starting + size; 
                new->size = here->size - size;
                new->freeing = 1;  
                new->next = here->next;

                here->size = size;
                here->freeing = 0; // not free
                here->next = new;
            } else {
                here->freeing = 0;
            }
            void* allocated = memory_pool + here->starting;
            pthread_mutex_unlock(&lock);
            return allocated;
        }
        here = here->next;
    }
    pthread_mutex_unlock(&lock);
    return NULL; 
}

// Free memory
void mem_free(void* block) {
    if (!block) {
        return;
    }

    pthread_mutex_lock(&lock);
    size_t starting = (char*)block - memory_pool;
    Memory* here = lista;
    Memory* bfore = NULL;

    while (here) {
        if (here->starting == starting) {
            if (here->freeing) {
                printf("Block already freed\n");
                pthread_mutex_unlock(&lock);
                return; 
            }
            here->freeing = 1;

            if (here->next && here->next->freeing) { // Merge with next
                Memory* bnext = here->next;
                here->size += bnext->size;
                here->next = bnext->next;
                free(bnext);
            }

            if (bfore && bfore->freeing) { // Merge with previous
                bfore->size += here->size;
                bfore->next = here->next;
                free(here);
            }
            pthread_mutex_unlock(&lock);
            return;
        }
        bfore = here; 
        here = here->next;
    }
    pthread_mutex_unlock(&lock);
}

// Resize memory block
void* mem_resize(void* block, size_t size) {
    if (block == NULL) {
        return mem_alloc(size);
    }
    if (size == 0) {
        mem_free(block);
        return NULL;
    }

    pthread_mutex_lock(&lock);
    Memory* here = lista;
    while (here != NULL) {
        if (memory_pool + here->starting == block) {
            break;
        }
        here = here->next;
    }
    if (here == NULL) {
        printf("Block not found in list\n");
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    size_t shere = here->size;
    if (shere >= size) { 
        pthread_mutex_unlock(&lock);
        return block;
    } else {
        pthread_mutex_unlock(&lock);
        void* new = mem_alloc(size); // allocate new block
        if (new == NULL) {
            printf("Allocation failed for new block\n");
            return NULL;  
        }

        memcpy(new, block, shere); // copy data
        mem_free(block);

        return new;
    }
}

// Deinitialize memory manager
void mem_deinit() {
    pthread_mutex_lock(&lock);
    if (memory_pool) { 
        free(memory_pool); // Free pool directly
        memory_pool = NULL;
        pool_size = 0;
    }

    Memory* here = lista;
    while (here) {
        Memory* next = here->next;
        free(here); // Free metadata
        here = next;
    }
    lista = NULL;
    pthread_mutex_unlock(&lock);
}
