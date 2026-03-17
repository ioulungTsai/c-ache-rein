#include <stdio.h>
#include <stdint.h>

int main(void)
{
    // array in memory
    uint8_t registers[4] = {0x01, 0x02, 0x04, 0x08};
    
    // pointer to first element
    uint8_t *ptr = registers;

    printf("=== walking array with pointer arithmetic ===\n");
    for (int i = 0; i < 4; i++) {
        printf("ptr+%d = %p -> value = 0x%02X\n",
               i, (void *)(ptr + i), *(ptr + i));
    }

    printf("\n=== ptr++ vs ptr+1 ===\n");
    printf("before: ptr = %p\n", (void *)ptr);
    ptr++;
    printf("after++: ptr = %p\n", (void *)ptr);
    ptr--;
    printf("after--: ptr = %p\n", (void *)ptr);

    printf("\n=== different type sizes ===\n");
    uint8_t arr8[4] = {0x01, 0x02, 0x03, 0x04};
    uint16_t arr16[4] = {0x0001, 0x0002, 0x0003, 0x0004};
    uint32_t arr32[4] = {0x00000001, 0x00000002,
                         0x00000003, 0x00000004};

    uint8_t *p8 = arr8;
    uint16_t *p16 = arr16;
    uint32_t *p32 = arr32;

    for (int i = 0; i < 4; i++) {
        printf("p8+%d = %p\n", i, (void *)(p8 + i));
        printf("p16+%d = %p\n", i, (void *)(p16 + i));
        printf("p32+%d = %p\n", i, (void *)(p32 + i));
        printf("---\n");
    }

    printf("\n=== pointer difference ===\n");
    uint8_t *start = arr8;
    uint8_t *end = &arr8[3];
    printf("end - start = %td\n", end - start);

    return 0;
}