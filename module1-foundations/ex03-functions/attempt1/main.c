#include <stdio.h>
#include <stdint.h>

// function declarations
uint8_t read_register(uint8_t reg);
void write_register(uint8_t reg, uint8_t value);
uint8_t get_bit(uint8_t reg, uint8_t bit);
void set_bit(uint8_t *reg, uint8_t but);
void clear_bit(uint8_t *reg, uint8_t bit);

int main(void)
{
    uint8_t reg = read_register(0x40);
    printf("register = 0x%02X\n", reg);

    printf("bit 3 = %u\n", get_bit(reg, 3));

    set_bit(&reg, 7);
    printf("after set bit 7 = 0x%02X\n", reg);

    clear_bit(&reg, 0);
    printf("after clear bit 0 = 0x%02X\n", reg);

    write_register(0x40, reg);

    return 0;
}

// function definitions
uint8_t read_register(uint8_t reg)
{
    printf("reading register 0x%02X\n", reg);
    return 0x0F;
}

void write_register(uint8_t reg, uint8_t value)
{
    printf("writing 0x%02X to register 0x%02X\n", value, reg);
}

uint8_t get_bit(uint8_t reg, uint8_t bit)
{
    return (reg >> bit) & 1;
}

void set_bit(uint8_t *reg, uint8_t bit)
{
    *reg |= (1U << bit);
}

void clear_bit(uint8_t *reg, uint8_t bit)
{
    *reg &= ~(1U << bit);
}
