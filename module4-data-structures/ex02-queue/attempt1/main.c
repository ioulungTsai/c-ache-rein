#include <stdio.h>
#include <stdint.h>

// queue definition
#define QUEUE_SIZE      8

typedef struct {
    uint32_t data[QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} queue_t;

// function declarations
void queue_init(queue_t *q);
int queue_enqueue(queue_t *q, uint32_t value);
int queue_dequeue(queue_t *q, uint32_t *value);
int queue_peek(queue_t *q, uint32_t *value);
int queue_is_empty(queue_t *q);
int queue_is_full(queue_t *q);
void queue_print(queue_t *q);

int main(void)
{
    queue_t q;
    queue_init(&q);

    printf("=== enqueue 5 values ===\n");
    queue_enqueue(&q, 0x10);
    queue_enqueue(&q, 0x20);
    queue_enqueue(&q, 0x30);
    queue_enqueue(&q, 0x40);
    queue_enqueue(&q, 0x50);
    queue_print(&q);

    printf("\n=== peek front ===\n");
    uint32_t val;
    if (queue_peek(&q, &val) == 0) {
        printf("front = 0x%02X\n", val);
    }

    printf("\n=== dequeue 3 values ===\n");
    for (int i = 0; i < 3; i++) {
        if (queue_dequeue(&q, &val) == 0) {
            printf("dequeued = 0x%02X\n", val);
        }
    }
    queue_print(&q);

    printf("\n=== enqueue until full ===\n");
    for (uint32_t i = 0xAA; !queue_is_full(&q); i++) {
        queue_enqueue(&q, i);
        printf("enqueued = 0x%02X\n", i);
    }
    queue_print(&q);

    printf("\n=== enqueue when full ===\n");
    if (queue_enqueue(&q, 0xFF) != 0) {
        printf("queue full - enqueue rejected\n");
    }

    printf("\n=== dequeue until empty ===\n");
    while (!queue_is_empty(&q)) {
        queue_dequeue(&q, &val);
        printf("dequeued = 0x%02X\n", val);
    }
    queue_print(&q);

    printf("\n=== dequeue when empty ===\n");
    if (queue_dequeue(&q, &val) != 0) {
        printf("queue empty - dequeue rejected\n");
    }

    return 0;
}

void queue_init(queue_t *q)
{
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

int queue_is_empty(queue_t *q)
{
    return q->count == 0;
}

int queue_is_full(queue_t *q)
{
    return q->count == QUEUE_SIZE;
}

int queue_enqueue(queue_t *q, uint32_t value)
{
    if (queue_is_full(q)) return -1;
    q->data[q->tail] = value;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;
    return 0; 
}

int queue_dequeue(queue_t *q, uint32_t *value)
{
    if (queue_is_empty(q)) return -1;
    *value = q->data[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;
    return 0;
}

int queue_peek(queue_t *q, uint32_t *value)
{
    if (queue_is_empty(q)) return -1;
    *value = q->data[q->head];
    return 0;
}

void queue_print(queue_t *q)
{
    printf("head=%u tail=%u count=%u | ", q->head, q->tail, q->count);
    for (uint32_t i = 0; i < q->count; i++) {
        printf("[0x%02X]", q->data[(q->head + i) % QUEUE_SIZE]);
    }
    if (queue_is_empty(q)) printf("empty");
    printf("\n");
}
