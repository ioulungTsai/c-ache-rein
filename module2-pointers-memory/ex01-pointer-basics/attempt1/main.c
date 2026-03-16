#include <stdio.h>
#include <stdint.h>

int main(void)
{
    // a variable and a pointer to it
    uint8_t reg = 0x0F;
    uint8_t *ptr = &reg;

    // what is in each
    printf("reg = 0x%02X\n", reg);
    printf("&reg = %p\n", (void *)&reg);
    printf("ptr = %p\n", (void *)ptr);
    printf("*ptr = 0x%02X\n", *ptr);

    // modify through pointer
    *ptr = 0xFF;
    printf("\nafter *ptr = 0xFF:\n");
    printf("reg = 0x%02X\n", reg);
    printf("*ptr = 0x%02X\n", *ptr);

    // pointer to pointer
    uint8_t **pptr = &ptr;
    printf("\npptr = %p\n", (void *)pptr);
    printf("*pptr = %p\n", (void *)*pptr);
    printf("**pptr = 0x%02X\n", **pptr);

    // different types, different sizes
    uint8_t a = 0;
    uint16_t b = 0;
    uint32_t c = 0;

    uint8_t *pa = &a;
    uint16_t *pb = &b;
    uint32_t *pc = &c;

    printf("\nsizeof(pa) = %zu\n", sizeof(pa));
    printf("sizeof(pb) = %zu\n", sizeof(pb));
    printf("sizeof(pc) = %zu\n", sizeof(pc));

    printf("\nsizeof(*pa) = %zu\n", sizeof(*pa));
    printf("sizeof(*pb) = %zu\n", sizeof(*pb));
    printf("sizeof(*pc) = %zu\n", sizeof(*pc));

    return 0;
}
