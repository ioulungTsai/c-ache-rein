#include <stdio.h>
#include <stdint.h>
#include <string.h>

// circular buffer definition
#define BUFFER_SIZE     8 // must be power of 2

typedef struct {
    uint8_t data[BUFFER_SIZE];
    uint32_t head;      // write index
    uint32_t tail;      // read index
    uint32_t count;     // elements currently in buffer
} circ_buf_t;

// function declarations
void cbuf_init(circ_buf_t *cb);
int cbuf_push(circ_buf_t *cb, uint8_t byte);
int cbuf_pop(circ_buf_t *cb, uint8_t *byte);
int cbuf_is_full(circ_buf_t *cb);
int cbuf_is_empty(circ_buf_t *cb);
void cbuf_print(circ_buf_t *cb);

int main(void)
{
    circ_buf_t cb;
    cbuf_init(&cb);

    printf("=== push 5 bytes ===\n");
    for (uint8_t i = 0x01; i <= 0x05; i++) {
        if (cbuf_push(&cb, i) == 0) {
            printf("pushed 0x%02X\n", i);
        }
    }
    cbuf_print(&cb);

    printf("\n=== pop 3 bytes ===\n");
    uint8_t byte;
    for (int i = 0; i < 3; i++) {
        if (cbuf_pop(&cb, &byte) == 0) {
            printf("popped 0x%02X\n", byte);
        }
    }
    cbuf_print(&cb);

    printf("\n=== push until full ===\n");
    for (uint8_t i = 0xAA; !cbuf_is_full(&cb); i++) {
        cbuf_push(&cb, i);
        printf("pushed 0x%02X\n", i);
    }
    cbuf_print(&cb);

    printf("\n=== push when full ===\n");
    if(cbuf_push(&cb, 0xFF) !=0) {
        printf("buffer full - push rejected\n");
    }

    printf("\n=== pop until empty ===\n");
    while (!cbuf_is_empty(&cb)) {
        cbuf_pop(&cb, &byte);
        printf("popped 0x%02X\n", byte);
    }
    cbuf_print(&cb);

    printf("\n=== pop when empty ===\n");
    if (cbuf_pop(&cb, &byte) != 0) {
        printf("buffer empty - pop rejected\n");
    }

    return 0;
}

void cbuf_init(circ_buf_t *cb)
{
    memset(cb->data, 0, BUFFER_SIZE);
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

int cbuf_push(circ_buf_t *cb, uint8_t byte)
{
    if (cbuf_is_full(cb)) return -1;
    cb->data[cb->head] = byte;
    cb->head = (cb->head + 1) % BUFFER_SIZE;
    cb->count++;
    return 0;
}

int cbuf_pop(circ_buf_t *cb, uint8_t *byte)
{
    if (cbuf_is_empty(cb)) return -1;
    *byte = cb->data[cb->tail];
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;
    cb->count--;
    return 0;
}

int cbuf_is_full(circ_buf_t *cb)
{
    return cb->count == BUFFER_SIZE;
}

int cbuf_is_empty(circ_buf_t *cb)
{
    return cb->count == 0;
}

void cbuf_print(circ_buf_t *cb)
{
    printf("head=%u tail=%u count=%u | ",
            cb->head, cb->tail, cb->count);
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        printf("[%02X]", cb->data[i]);
    }
    printf("\n");
}
