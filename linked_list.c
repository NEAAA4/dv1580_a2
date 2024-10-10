// lico22

#include "memory_manager.h"
#include "common_defs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>

#define pool_size 1024
#define block_size 16

char memory_pool[pool_size];
bool allocated[pool_size/block_size];
size_t num_blocks;


typedef struct Node {
    uint16_t data;
    struct Node* next;
} Node; 

void list_init(Node** head, size_t size) {
    *head = NULL;
    Node* node = NULL;

    for (size_t i = 0; i < size; i++) {
        Node* node = (Node*)mem_alloc(sizeof(Node));
        if (node == NULL) {
            return; // failed tomt
        }
    }
    node->next = *head;
    *head = node;
}

void list_insert(Node** head, uint16_t data) {
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex

    Node* node = (Node*)mem_alloc(sizeof(Node));
    if (node == NULL) {
        printf("Memory allocation failed, no node \n");
        pthread_mutex_unlock(&mem_lock); // faild unlock
        return;
    }

    node->data = data;
    node->next = NULL;

    if (*head == NULL) {
        *head = node;
    }
    else {
        Node* here = *head;
        while (here->next != NULL) {
            here = here->next;
        }
        here->next = node;
    }
    pthread_mutex_unlock(&mem_lock);
}

void list_insert_after(Node* prev_node, uint16_t data) {
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex
    
    Node* node = (Node*)mem_alloc(sizeof(Node));
    if (node == NULL) {
        printf("Memory allocation failed, no node \n");
        pthread_mutex_unlock(&mem_lock); // faild unlock
        return;
    }
    
    node->data = data;
    node->next = prev_node->next;
    prev_node->next = node;

    pthread_mutex_unlock(&mem_lock);
}

void list_insert_before(Node** head, Node* next_node, uint16_t data){
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex

    Node* node = (Node*)mem_alloc(sizeof(Node));
    if (next_node == NULL) {
        printf("Memory allocated failed, no node \n");
        pthread_mutex_unlock(&mem_lock);
        return;
    }
    
    node->data = data; // node får datan

    if (*head == next_node) {
        node->next = *head; // ifall pekaren för head är på nextnode så flyttas den en steg
        *head = node;
        pthread_mutex_unlock(&mem_lock);
        return;
    }

    Node* here = *head;
    while (here->next != next_node && here->next != NULL) {
        here = here->next;
    }

    if (here->next == next_node) {
        node->next = next_node; // pekar på nästa
        here->next = node; // pekaren updateras till nästa
    }
    pthread_mutex_unlock(&mem_lock);
}

void list_delete(Node** head, uint16_t data) {
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex

    if (*head == NULL) {
        pthread_mutex_unlock(&mem_lock);
        return;
    }

    Node* here = *head;
    Node* bfore = NULL;

    if (here != NULL && here->data == data) {
        *head = here->next;
        mem_free(here);
        pthread_mutex_unlock(&mem_lock);
        return;
    }

    while (here->data != data && here != NULL ) {
        bfore = here;
        here = here->next;
    }

    if (here == NULL) {
        pthread_mutex_unlock(&mem_lock);
        return; // finns ej
    }

    bfore->next = here->next;
    mem_free(here);
}

Node* list_search(Node** head, uint16_t data) {
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex

    Node* here = *head;
    while (here != NULL) {
        if (here->data == data) {
            pthread_mutex_unlock(&mem_lock);
            return here;
        }
        here = here->next;
    }
    return NULL;
}

void list_display(Node** head) {
    Node* here = *head; // sätter pekaren på start värdet
    printf("[");
    while (here != NULL) {
        printf("%d", here->data);
        if (here->next != NULL) {
            printf(", ");
        }
        here = here->next; // flyttar pekaren ett steg
    }
    printf("]");
}

void list_display_range(Node** head, Node* start_node, Node* end_node){
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex

    Node* here = *head;

    if (start_node == NULL) {
        start_node = *head;
    }

    while (here != NULL && here != start_node) {
        here = here->next;
    }

    if (here == NULL) {
        printf("[] \n");
        pthread_mutex_unlock(&mem_lock);
        return;
    }
    while (here != NULL) {
        printf("%d", here->data);
        if (here == end_node) {
            pthread_mutex_unlock(&mem_lock);
            break;
        }
        here = here->next;
    }

    printf("\n");
    pthread_mutex_unlock(&mem_lock);
}

int list_count_nodes(Node** head) {
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex

    Node* here = *head;
    int count = 0;
    while (here != NULL) {
        count++;
        here = here->next;
    }
    pthread_mutex_unlock(&mem_lock);
    return count;
}

void list_cleanup(Node** head) {
    pthread_mutex_t mem_lock; // skapar en mutex
    pthread_mutex_init(&mem_lock, NULL); 
    pthread_mutex_lock(&mem_lock); // låser med mutex
    
    Node* here = *head;
    Node* node;

    while (here != NULL) {
        node = here->next;
        mem_free(here);
        here = node;
    }
    *head = NULL;
    pthread_mutex_unlock(&mem_lock);
}
