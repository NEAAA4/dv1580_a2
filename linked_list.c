#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Mutex for thread safety
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void list_init(Node** head, size_t size) {
    pthread_mutex_lock(&lock);
    *head = NULL;
    mem_init(size); 
    pthread_mutex_unlock(&lock);
}

void list_insert(Node** head, uint16_t data) {
    pthread_mutex_lock(&lock);
    Node* new = (Node*) mem_alloc(sizeof(Node));
    if (new == NULL) {
        printf("Memory allocation failed \n");
        pthread_mutex_unlock(&lock);
        return;
    }

    new->data = data;
    new->next = NULL;

    if (*head == NULL) {
        *head = new; 
    } else {
        Node* here = *head;
        while (here->next != NULL) { // Traversing to empty
            here = here->next;
        }
        here->next = new;
    }
    pthread_mutex_unlock(&lock);
}

void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        printf("Prev_node is NULL \n");
        return;
    }

    pthread_mutex_lock(&lock);
    Node* new = (Node*) mem_alloc(sizeof(Node));
    if (new == NULL) {
        printf("Memory allocation failed \n");
        pthread_mutex_unlock(&lock);
        return;
    }

    new->data = data;
    new->next = prev_node->next;

    prev_node->next = new;
    pthread_mutex_unlock(&lock);
}

void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    pthread_mutex_lock(&lock);
    if (next_node == *head) {
        Node* new = (Node*) mem_alloc(sizeof(Node));
        if (new == NULL) {
            printf("Memory allocation failed \n");
            pthread_mutex_unlock(&lock);
            return;
        }

        new->data = data;
        new->next = *head;
        *head = new;
        pthread_mutex_unlock(&lock);
        return;
    }

    Node* prev_node = *head; // Check for prev node
    while (prev_node != NULL && prev_node->next != next_node) {
        prev_node = prev_node->next;
    }

    if (prev_node == NULL) {
        printf("Next_node not in list \n");
        pthread_mutex_unlock(&lock);
        return;
    }

    Node* new = (Node*) mem_alloc(sizeof(Node));
    if (new == NULL) {
        printf("Memory allocation failed \n");
        pthread_mutex_unlock(&lock);
        return;
    }

    new->data = data;
    new->next = next_node;
    prev_node->next = new;
    pthread_mutex_unlock(&lock);
}

void list_delete(Node** head, uint16_t data) {
    pthread_mutex_lock(&lock);
    if (*head == NULL || head == NULL) {
        printf("List is empty \n");
        pthread_mutex_unlock(&lock);
        return;
    }

    Node* temp = NULL;

    if ((*head)->data == data) { // Delete head
        temp = *head;
        *head = (*head)->next;
        mem_free(temp);
        pthread_mutex_unlock(&lock);
        return;
    }

    Node* here = *head;
    while (here->next != NULL) {
        if (here->next->data == data) { // Search for data
            temp = here->next;
            here->next = here->next->next;
            mem_free(temp);
            pthread_mutex_unlock(&lock);
            return;
        }
        here = here->next;
    }
    printf("Data not found in list \n");
    pthread_mutex_unlock(&lock);
}

Node* list_search(Node** head, uint16_t data) {
    pthread_mutex_lock(&lock);
    if (head == NULL || *head == NULL) {
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    Node* here = *head;
    while (here != NULL) {
        if (here->data == data) {
            pthread_mutex_unlock(&lock);
            return here;
        }
        here = here->next;
    }

    pthread_mutex_unlock(&lock);
    return NULL;
}

void list_display(Node** head) {
    pthread_mutex_lock(&lock);
    Node* here = *head;
    printf("[");
    while (here) {
        printf("%u", here->data);
        if (here->next) {
            printf(", ");
        }
        here = here->next;
    }
    printf("]\n");
    pthread_mutex_unlock(&lock);
}

void list_display_range(Node** head, Node* start_node, Node* end_node) {
    pthread_mutex_lock(&lock);
    if (*head == NULL) {
        printf("[]");
        pthread_mutex_unlock(&lock);
        return;
    }

    if (start_node == NULL) {
        start_node = *head;
    }

    Node* here = start_node;
    printf("[");
    int start = 1;

    while (here != NULL) {
        if (!start) {
            printf(", ");
        }
        printf("%u", here->data);
        if (here == end_node) {
            break;
        }
        here = here->next;
        start = 0;
    }
    printf("]");
    pthread_mutex_unlock(&lock);
}

int list_count_nodes(Node** head) {
    pthread_mutex_lock(&lock);
    int count = 0;
    Node* here = *head;
    while (here != NULL) {
        count++;
        here = here->next;
    }
    pthread_mutex_unlock(&lock);
    return count;
}

void list_cleanup(Node** head) {
    pthread_mutex_lock(&lock);
    Node* here = *head;
    while (here != NULL) {
        Node* next = here->next;
        mem_free(here);
        here = next;
    }
    *head = NULL;
    mem_deinit();
    pthread_mutex_unlock(&lock);
}
