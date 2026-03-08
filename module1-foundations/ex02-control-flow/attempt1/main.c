#include <stdio.h>
#include <stdint.h>

int main(void)
{
    // if / else if / else
    uint8_t register_val = 0x0F;

    if (register_val == 0xFF) {
        printf("refister full\n");
    } else if (register_val == 0x00) {
        printf("register empty\n");
    } else {
        printf("register = 0x%02X\n", register_val);
    }

    // while loop
    uint8_t bit = 0;
    while (bit < 8) {
        printf("bit %u = %u\n", bit, (register_val >> bit) & 1);
        bit++;
    }

    // for loop
    printf("counting down: ");
    for (int i = 7; i >= 0; i--) {
        printf("%d", i);
    }
    printf("\n");

    return 0;
}
