#include <stdio.h>
#include <stdint.h>

// const
#define MAX_SENSORS_MACRO    8 // preprocessor - no type, no memory
const uint8_t MAX_SENSORS = 8; // typed, has memory, compiler enforced

// static global - file scope only
static uint8_t error_count = 0;

// function declarations
void sensor_read(void);
void counter_demo(void);
uint32_t get_error_count(void);

// volatile proof — NO side effects inside loops
// pattern: status register polling — UART/SPI/I2C ready flag 
void poll_without_volatile(void);
void poll_with_volatile(void);

int main(void)
{
    printf("=== const ===\n");
    printf("MAX_SENSORS_MACRO   = %d\n", MAX_SENSORS_MACRO);
    printf("MAX_SENSORS         = %u\n", MAX_SENSORS);

    printf("\n=== static local counter ===\n");
    counter_demo();
    counter_demo();
    counter_demo();

    printf("\n=== static global error count ===\n");
    sensor_read();
    sensor_read();
    printf("total errors = %u\n", get_error_count());

    printf("\n=== volatile proof ===\n");
    printf("run: make asm - see assembly difference\n");
    printf("without volatile    -> compiler deletes pooling loop\n");
    printf("with volatile       -> compiler generates real polling loop\n");

    return 0;
}

void counter_demo(void)
{
    static uint32_t count = 0; // initialized once, persists between calls
    count++;
    printf("count = %u\n", count);
}

void sensor_read(void)
{
    // simulate sensor read failure
    error_count++;
    printf("sensor read failed\n");
}

uint32_t get_error_count(void)
{
    return error_count;
}

//  poll_without_volatile
//  simulates: waiting for UART TX ready flag
//  without volatile — compiler sees status never changes
//  from outside — deletes loop entirely at -O2

void poll_without_volatile(void)
{
    uint32_t status = 0x00000001; // bit 0 = BUSY;
    while (status & 0x01) { 
        status = 0x00000000;
    }// wait for hardware to clear BUSY
    // compiler removes this - status never change inside loop
}


//  poll_with_volatile
//  simulates: waiting for UART TX ready flag
//  with volatile — compiler forced to re-read status every iteration
//  cannot optimize away — hardware could clear BUSY anytime

void poll_with_volatile(void)
{
    volatile uint32_t status = 0x00000001; // bit 0 = BUSY
    while (status & 0x01) {
        status = 0x00000000;
    } // correctly waits for hardware
}
