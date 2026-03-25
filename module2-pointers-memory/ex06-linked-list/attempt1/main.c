#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct node {
    uint8_t data;
    struct node *next;
} node_t;

// function declarations
node_t *node_create(uint8_t data);
void list_push_front(node_t **head, uint8_t data);
void list_push_back(node_t **head, uint8_t data);
void list_print(node_t *head);
size_t list_length(node_t *head);
void list_free(node_t **head);

int main(void)
{
    node_t *head = NULL;

    // build list
    list_push_back(&head, 0x01);
    list_push_back(&head, 0x02);
    list_push_back(&head, 0x03);
    list_push_front(&head, 0xFF);

    printf("=== list ===\n");
    list_print(head);
    printf("length = %zu\n", list_length(head));

    // free everything
    list_free(&head);
    printf("\nafter free:\n");
    printf("head: %p\n", (void *)head);

    return 0;
}

node_t *node_create(uint8_t data)
{
    node_t *n = malloc(sizeof(node_t));
    if (n == NULL) return NULL;
    n->data = data;
    n->next = NULL;
    return n;
}

void list_push_front(node_t **head, uint8_t data)
{
    node_t *n = node_create(data);
    if (n == NULL) return;
    n->next = *head;
    *head = n;
}

void list_push_back(node_t **head, uint8_t data)
{
    node_t *n = node_create(data);
    if (n == NULL) return;

    if (*head == NULL) {
        *head = n;
        return;
    }

    node_t *curr = *head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = n;
}

void list_print(node_t *head)
{
    node_t *curr = head;
    while (curr != NULL) {
        printf("0x%02X", curr->data);
        if(curr->next != NULL) printf(" -> ");
        curr = curr->next;
    }
    printf(" -> NULL\n");
}

size_t list_length(node_t *head)
{
    size_t count = 0;
    node_t *curr = head;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }
    return count;
}

void list_free(node_t **head)
{
    node_t *curr = *head;
    while (curr != NULL) {
        node_t *next = curr->next;
        free(curr);
        curr = next;
    }
    *head = NULL;
}
