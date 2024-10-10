#ifndef LINKED_LIST_H
#define LINKED_LIST_H

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

typedef struct Node {
    uint16_t data;
    struct Node* next;
} Node; 


void list_init(Node** head, size_t size);
void list_insert(Node** head, uint16_t data);
void list_insert_after(Node* prev_node, uint16_t data);
void list_insert_before(Node** head, Node* next_node, uint16_t data);
void list_delete(Node** head, uint16_t data);
Node* list_search(Node** head, uint16_t data);
void list_display(Node** head);
void list_display_range(Node** head, Node* start_node, Node* end_node);
int list_count_nodes(Node** head);
void list_cleanup(Node** head);

#endif // LINKED_LIST_H