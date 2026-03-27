#include <stdio.h>
#include <stdint.h>

// simulated periphral base address
#define PERIPH_BASE     0x40000000U
#define GPIO_OFFSET     0x00000000U
#define UART_OFFSET     0x00001000U

// register offsets
#define REG_CR          0x00U // control register
#define REG_SR          0x04U // status register
#define REG_DR          0x08U // data register

// control register bits
#define CR_ENABLE       0U // bit 0 - peripheral enable
#define CR_TXEN         1U // bit 1 - TX enable
#define CR_RXEN         2U // bit 2 - RX enable

// status register bits
#define SR_BUSY         0U // bit 0 - peripheral busy
#define SR_TXREADY      1U // bit 1 - TX ready
#define SR_RXREADY      2U // bit 2 - RX ready
#define SR_ERROR        7U // bit 7 - error flag

// register access macros
#define REG_READ(base, offset) \
    (*((volatile uint32_t *)((base) + (offset))))

#define REG_WRITE(base, offset, value) \
    (*((volatile uint32_t *)((base) + (offset))) = (value))

#define REG_SET_BIT(base, offset, bit) \
    (*((volatile uint32_t *)((base) + (offset))) |= (1U << (bit)))

#define REG_CLEAR_BIT(base, offset, bit) \
    (*((volatile uint32_t *)((base) + (offset))) &= ~(1U << (bit))) 

#define REG_CHECK_BIT(base, offset, bit) \
    ((*((volatile uint32_t *)((base) + (offset))) >> (bit)) & 1U)

// simulated register bank - stands in for real hardware
static volatile uint32_t fake_peripheral[3] = {
    0x00000000, // CR - control register
    0x00000002, // SR - statu: TX ready
    0x00000000  // DR - data register
};

// function declarations
void periph_init(volatile uint32_t *base);
void periph_send(volatile uint32_t *base, uint8_t data);
void periph_print(volatile uint32_t *base);

int main(void)
{
    volatile uint32_t *periph = fake_peripheral;

    printf("=== before init ===\n");
    periph_print(periph);

    printf("\n=== init peripheral ===\n");
    periph_init(periph);
    periph_print(periph);

    printf("\n=== send data ===\n");
    periph_send(periph, 0xAB);
    periph_print(periph);

    printf("\n=== simulate error ===\n");
    REG_SET_BIT((uintptr_t)periph, REG_SR, SR_ERROR);
    periph_print(periph);
    if (REG_CHECK_BIT((uintptr_t)periph, REG_SR, SR_ERROR)) {
        printf("!!error detected - clearing!!\n");
        REG_CLEAR_BIT((uintptr_t)periph, REG_SR, SR_ERROR);
    }
    periph_print(periph);

    return 0;
}

void periph_init(volatile uint32_t *base)
{
    // enable peripheral, TX, RX
    REG_SET_BIT((uintptr_t)base, REG_CR, CR_ENABLE);
    REG_SET_BIT((uintptr_t)base, REG_CR, CR_TXEN);
    REG_SET_BIT((uintptr_t)base, REG_CR, CR_RXEN);
    printf("peripheral initialized\n");
}

void periph_send(volatile uint32_t *base, uint8_t data)
{
    // wait for TX ready
    while (!REG_CHECK_BIT((uintptr_t)base, REG_SR, SR_TXREADY)) {
        printf("waiting for TX ready..\n");
    }

    // write data
    REG_WRITE((uintptr_t)base, REG_DR, data);
    printf("sent 0x%02X\n", data);

    // simulate TX busy then complete
    REG_SET_BIT((uintptr_t)base, REG_SR, SR_BUSY);
    REG_CLEAR_BIT((uintptr_t)base, REG_SR, SR_BUSY);
}

void periph_print(volatile uint32_t *base)
{
    uint32_t cr = REG_READ((uintptr_t)base, REG_CR);
    uint32_t sr = REG_READ((uintptr_t)base, REG_SR);
    uint32_t dr = REG_READ((uintptr_t)base, REG_DR);

    printf("CR = 0x%08X EN=%u TXEN=%u RXEN=%u\n",
            cr,
            (cr >> CR_ENABLE) & 1U,
            (cr >> CR_TXEN)   & 1U,
            (cr >> CR_RXEN)   & 1U);

    printf("SR = 0x%08X BUSY=%u TXRDY=%u RXRDY=%u ERR=%u\n",
            sr,
            (sr >> SR_BUSY)    & 1U,
            (sr >> SR_TXREADY) & 1U,
            (sr >> SR_RXREADY) & 1U,
            (sr >> SR_ERROR)   & 1U);

    printf("DR = 0x%08X\n", dr);
}
