//lico22

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define pool_size 1024
#define block_size 16

char memory_pool[pool_size];
bool allocated[pool_size/block_size];
size_t num_blocks;


void mem_init(size_t size) {
    // Allocate memory static 
    for (int i = 0; i < pool_size / block_size; i++) {
        allocated[i] = false;
    }
}

void* mem_alloc(size_t size) {

    pthread_mutex_t mem_lock; // skapar en mjutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex

    if (size > block_size) {
        return NULL;
    }
    for (int i = 0; i < pool_size / block_size; i++) {
        if (!allocated[i]) {
            allocated[i] = true;
            return (void*)&memory_pool[i * block_size];
        }
    }
    pthread_mutex_unlock(&mem_lock); // låser upp
    return NULL;
}

void mem_free(void* block) {

    pthread_mutex_t mem_lock;
    pthread_mutex_init(&mem_lock, NULL); // skapar en mutex
    pthread_mutex_lock(&mem_lock); // låser när den används
    
    if (block >= (void*)memory_pool && block < (void*)(memory_pool + pool_size)) {
        pthread_mutex_unlock(&mem_lock); 
        return;
    }
    size_t offset = (unsigned char*)block - (unsigned char*)memory_pool;
    size_t index = offset / block_size;

    if (index < num_blocks) {
        allocated[index] = false;
    }
    pthread_mutex_unlock(&mem_lock); // låser upp
}

void* mem_resize(void* block, size_t size) {
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex

    void* new = mem_alloc(size);

    if (new == NULL) {
        pthread_mutex_unlock(&mem_lock); // faild unlock
        return NULL;
    }

    if (block != NULL) {
        mem_free(block); // om inte null, frigör
    }

    pthread_mutex_unlock(&mem_lock);
    return new;
}



void mem_deinit() {
    // Deallocate memory static
    for (int i = 0; i < pool_size / block_size; i++) {
        allocated[i] = false; // sätter allt som tomt
    }
}
