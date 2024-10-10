#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>

#define pool_size 1024 // storlek på memory pool
#define block_size 16 // storlek på memory block

extern char memory_pool[pool_size]; 
extern bool allocated[pool_size/block_size];
extern size_t num_blocks;

void mem_init(size_t size);
void* mem_alloc(size_t size);
void mem_free(void* block);
void* mem_resize(void* block, size_t size);
void mem_deinit();

#endif