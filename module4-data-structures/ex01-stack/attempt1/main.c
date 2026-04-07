#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// stack definition
#define STACK_SIZE  8

typedef struct {
    uint32_t data[STACK_SIZE];
    int top;    // index of top element, -1 if empty
} stack_t;

// function declarations
void stack_init(stack_t *s);
int stack_push(stack_t *s, uint32_t value);
int stack_pop(stack_t *s, uint32_t *value);
int stack_peek(stack_t *s, uint32_t *value);
int stack_is_empty(stack_t *s);
int stack_is_full(stack_t *s);
void stack_print(stack_t *s);

int main(void)
{
    stack_t s;
    stack_init(&s);
    
    printf("=== push 5 values ===\n");
    stack_push(&s, 0x10);
    stack_push(&s, 0x20);
    stack_push(&s, 0x30);
    stack_push(&s, 0x40);
    stack_push(&s, 0x50);
    stack_print(&s);

    printf("\n=== peak top ===\n");
    uint32_t val;
    if (stack_peek(&s, &val) == 0) {
        printf("top = 0x%02X\n", val);
    }

    printf("\n=== pop 3 values ===\n");
    for (int i = 0; i < 3; i++) {
        if (stack_pop(&s, &val) == 0) {
            printf("poped = 0x%02X\n", val);
        }
    }
    stack_print(&s);

    printf("\n=== push until full ===\n");
    for (uint32_t i = 0xAA; !stack_is_full(&s); i++) {
        stack_push(&s, i);
        printf("pushed = 0x%02X\n", i);
    }
    stack_print(&s);

    printf("\n=== push when full ===\n");
    if (stack_push(&s, 0xFF) != 0) {
        printf("stack full - push rejected\n");
    }

    printf("\n=== pop until empty ===\n");
    while (!stack_is_empty(&s)) {
        stack_pop(&s, &val);
        printf("popped = 0x%02X\n", val);
    }

    printf("\n=== pop when empty ===\n");
    if (stack_pop(&s, &val) != 0) {
        printf("stack empty - pop rejected\n");
    }

    return 0;
}

void stack_init(stack_t *s)
{
    s->top = -1;
}

int stack_is_empty(stack_t *s)
{
    return s->top == -1;
}

int stack_is_full(stack_t *s) {
    return s->top == STACK_SIZE - 1;
}

int stack_push(stack_t *s, uint32_t value)
{
    if (stack_is_full(s)) return -1;
    s->data[++s->top] = value;
    return 0;
}

int stack_pop(stack_t *s, uint32_t *value)
{
    if (stack_is_empty(s)) return -1;
    *value = s->data[s->top--];
    return 0;
}

int stack_peek(stack_t *s, uint32_t *value)
{
    if (stack_is_empty(s)) return -1;
    *value = s->data[s->top];
    return 0;
}

void stack_print(stack_t *s)
{
    printf("top=%d | ", s->top);
    for (int i = s->top; i >= 0; i--) {
        printf("[0x%02X]", s->data[i]);
        if (i > 0) printf(" ");
    }
    if (stack_is_empty(s)) printf("empty");
    printf("\n");
}
