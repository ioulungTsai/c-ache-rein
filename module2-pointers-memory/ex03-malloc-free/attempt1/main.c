#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void print_stack_address(int depth);

int main(void)
{
    // allocate a single uint8_t on heap
    uint8_t *reg = malloc(sizeof(uint8_t));
    if (reg == NULL) {
        printf("error: malloc failed\n");
        return 1;
    }

    *reg = 0xFF;
    printf("reg = 0x%02X\n", *reg);
    free(reg);
    reg = NULL;

    // allocate an array on heap
    int count = 4;
    uint8_t *registers = malloc(count * sizeof(uint8_t));
    if (registers == NULL) {
        printf("error: mallox failed\n");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        registers[i] = (uint8_t)(1 << i);
    }

    printf("\nregisters:\n");
    for (int i = 0; i < count; i++) {
        printf("registers[%d] = 0x%02X\n", i, registers[i]);
    }

    free(registers);
    registers = NULL;

    // stack vs heap comparison
    uint8_t stack_buf[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t *heap_buf = malloc(4 * sizeof(uint8_t));
    if (heap_buf == NULL) {
        printf("error: malloc failed\n");
        return 1;
    }

    for (int i =0; i < 4; i++) {
        heap_buf[i] = stack_buf[i];
    }

    printf("\nstack_buf address = %p\n", (void *)stack_buf);
    printf("heap_buf address = %p\n", (void *)heap_buf);

    free(heap_buf);
    heap_buf = NULL;

    // prove stack grows down
    printf("\n=== proving stack grows down ===\n");
    uint8_t stack1 = 0x01;
    uint8_t stack2 = 0x02;
    uint8_t stack3 = 0x03;
    printf("stack1 = %p\n", (void *)&stack1);
    printf("stack2 = %p\n", (void *)&stack2);
    printf("stack3 = %p\n", (void *)&stack3);

    // prove heap grows up
    printf("\n=== proving heap grows up ===\n");
    uint8_t *heap1 = malloc(1);
    uint8_t *heap2 = malloc(1);
    uint8_t *heap3 = malloc(1);
    printf("heap1 = %p\n", (void *)heap1);
    printf("heap2 = %p\n", (void *)heap2);
    printf("heap3 = %p\n", (void *)heap3);
    free(heap1);
    free(heap2);
    free(heap3);
    heap1 = NULL;
    heap2 = NULL;
    heap3 = NULL;

    // prove stack grows down (function calls)
    printf("\n=== proving stack grows down (function calls) ===\n");
    print_stack_address(0);
    
    return 0;
}

void print_stack_address(int depth)
{
    uint8_t local = 0;
    printf("depth %d: %p\n", depth, (void *)&local);
    if (depth < 3) {
        print_stack_address(depth + 1);
    }
}
