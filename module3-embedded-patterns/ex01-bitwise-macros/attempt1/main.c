#include <stdio.h>
#include <stdint.h>

// bitwise macros
#define SET_BIT(reg, bit)       ((reg) |=  (1U << (bit)))
#define CLEAR_BIT(reg, bit)     ((reg) &= ~(1U << (bit)))
#define TOGGLE_BIT(reg, bit)    ((reg) ^=  (1U << (bit)))
#define CHECK_BIT(reg, bit)     (((reg) >> (bit)) & 1U)

// register bit definitions - like real hardware headers
#define STATUS_BUSY_BIT      0
#define STATUS_READY_BIT     1
#define STATUS_ERROR_BIT     2
#define STATUS_OVERFLOW_BIT  7

void print_register(const char *name, uint8_t reg)
{
    printf("%s = 0x%02X [", name, reg);
    for (int i = 7; i >= 0; i--) {
        printf("%u", CHECK_BIT(reg, i));
        if (i > 0) printf(" ");
    }
    printf("]\n");
}

int main(void)
{
    // printf("let's code!\n");
    uint8_t status = 0x00;

    printf("=== initial state ===\n");
    print_register("status", status);

    printf("\n=== set BUST and READY ===\n");
    SET_BIT(status, STATUS_BUSY_BIT);
    SET_BIT(status, STATUS_READY_BIT);
    print_register("status", status);

    printf("\n=== clear BUSY ===\n");
    CLEAR_BIT(status, STATUS_BUSY_BIT);
    print_register("status", status);

    printf("\n=== toggle OVERFLOW ===\n");
    TOGGLE_BIT(status, STATUS_OVERFLOW_BIT);
    print_register("status", status);
    TOGGLE_BIT(status, STATUS_OVERFLOW_BIT);
    print_register("status", status);

    printf("\n=== check bits ===\n");
    printf("BUSY    = %u\n", CHECK_BIT(status, STATUS_BUSY_BIT));
    printf("READY   = %u\n", CHECK_BIT(status, STATUS_READY_BIT));
    printf("ERROR   = %u\n", CHECK_BIT(status, STATUS_ERROR_BIT));

    printf("\n=== set ERROR check with if ===\n");
    SET_BIT(status, STATUS_ERROR_BIT);
    print_register("status", status);
    if (CHECK_BIT(status, STATUS_ERROR_BIT)) {
        printf("error detected - clearing\n");
        CLEAR_BIT(status, STATUS_ERROR_BIT);
    }
    print_register("status", status);

    return 0;
}
