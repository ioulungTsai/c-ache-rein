#include <stdio.h>
#include <stdint.h>
#include <string.h>

// configuration
#define CBUF_SIZE       16U                 // must be power of 2
#define CBUF_MASK       (CBUF_SIZE - 1U)    // 0x0F - bitwise AND mask

// production circular buffer
typedef struct {
    uint8_t data[CBUF_SIZE];
    volatile uint32_t head; // write index - modified by producer/ISR
    volatile uint32_t tail; // read index - modified by consumer/main
    uint32_t size;          // always CBUF_SIZE - for reference
} cbuf_t;

// error codes
#define CBUF_OK         0
#define CBUF_FULL      -1
#define CBUF_EMPTY     -2

// function declarations
void cbuf_init(cbuf_t *cb);
int cbuf_push(cbuf_t *cb, uint8_t byte);
int cbuf_pop(cbuf_t *cb, uint8_t *byte);
int cbuf_push_buf(cbuf_t *cb, const uint8_t *buf, uint32_t len);
int cbuf_pop_buf(cbuf_t *cb, uint8_t *buf, uint32_t len);
uint32_t cbuf_count(cbuf_t *cb);
uint32_t cbuf_free(cbuf_t *cb);
int cbuf_is_empty(cbuf_t *cb);
int cbuf_is_full(cbuf_t *cb);
void cbuf_flush(cbuf_t *cb);
void cbuf_print(cbuf_t *cb);

int main(void)
{
    cbuf_t cb;
    cbuf_init(&cb);

    printf("=== single byte push/pop ===\n");
    cbuf_push(&cb, 0x41);   // 'A'
    cbuf_push(&cb, 0x42);   // 'B'
    cbuf_push(&cb, 0x43);   // 'C'
    cbuf_print(&cb);

    uint8_t byte;
    cbuf_pop(&cb, &byte);
    printf("popped = 0x%02X (%c)\n", byte, byte);
    cbuf_print(&cb);

    printf("\n=== buffer push/pop ===\n");
    uint8_t tx[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    cbuf_push_buf(&cb, tx, 5);
    printf("pushed 5 bytes\n");
    cbuf_print(&cb);

    uint8_t rx[4];
    cbuf_pop_buf(&cb, rx, 4);
    printf("popped 4 bytes ");
    for (int i = 0; i < 4; i++) printf("0x%02X ", rx[i]);
    printf("\n");
    cbuf_print(&cb);

    printf("\n=== wrap-around test ===\n");
    cbuf_flush(&cb);

    // fill to near end of array
    for (uint8_t i = 0; i < 13; i++) cbuf_push(&cb, i);
    // pop 10 - head wraps around
    for (int i = 0; i < 10; i++) cbuf_pop(&cb, &byte);
    // push more - tail wrap around
    for (uint8_t i = 0xA0; i < 0xA8; i++) cbuf_push(&cb, i);
    cbuf_print(&cb);

    printf("\n=== capability checks ===\n");
    printf("count = %u\n", cbuf_count(&cb));
    printf("free = %u\n", cbuf_free(&cb));
    printf("full = %u\n", cbuf_is_full(&cb));
    printf("empty = %u\n", cbuf_is_empty(&cb));

    printf("\n=== flush ===\n");
    cbuf_flush(&cb);
    cbuf_print(&cb);
    
    return 0;
}

void cbuf_init(cbuf_t *cb)
{
    memset(cb->data, 0, CBUF_SIZE);
    cb->head = 0;
    cb->tail = 0;
    cb->size = CBUF_SIZE;
}

int cbuf_is_empty(cbuf_t *cb)
{
    return cb->head == cb->tail;
}

int cbuf_is_full(cbuf_t *cb)
{
    return cbuf_count(cb) == CBUF_SIZE;
}

uint32_t cbuf_count(cbuf_t *cb)
{
    return (cb->head - cb->tail) & CBUF_MASK;
}

uint32_t cbuf_free(cbuf_t *cb)
{
    return CBUF_SIZE - cbuf_count(cb);
}

int cbuf_push(cbuf_t *cb, uint8_t byte)
{
    if(cbuf_is_full(cb)) return CBUF_FULL;
    cb->data[cb->head & CBUF_MASK] = byte;
    cb->head++;                             // never wrap - ever-increasing
    return CBUF_OK;
}

int cbuf_pop(cbuf_t *cb, uint8_t *byte)
{
    if(cbuf_is_empty(cb)) return CBUF_EMPTY;
    *byte = cb->data[cb->tail & CBUF_MASK];
    cb->tail++;
    return CBUF_OK;
}

int cbuf_push_buf(cbuf_t *cb, const uint8_t *buf, uint32_t len)
{
    if(cbuf_free(cb) < len) return CBUF_FULL;
    for (uint32_t i = 0; i < len; i++) {
        cbuf_push(cb, buf[i]);
    }
    return CBUF_OK;
}

int cbuf_pop_buf(cbuf_t *cb, uint8_t *buf, uint32_t len)
{
    if(cbuf_count(cb) < len) return CBUF_EMPTY;
    for (uint32_t i = 0; i < len; i++) {
        cbuf_pop(cb, &buf[i]);
    }
    return CBUF_OK;
}

void cbuf_flush(cbuf_t *cb)
{
    cb->head = 0;
    cb->tail = 0;
}

void cbuf_print(cbuf_t *cb)
{
    printf("head=%u tail=%u count=%u free=%u | ",
            cb->head, cb->tail,
            cbuf_count(cb),
            cbuf_free(cb));

    uint32_t count = cbuf_count(cb);
    for (uint32_t i = 0; i < count; i++) {
        printf("[0x%02X]", cb->data[(cb->tail + i) & CBUF_MASK]);
    }
    if (cbuf_is_empty(cb)) printf("empty");
    printf("\n");
}
