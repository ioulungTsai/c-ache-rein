#include <stdio.h>
#include <stdint.h>

int main(void)
{
    // integers
    int age = 10;
    unsigned int count = 100;

    // characters
    char letter = 'A';

    // floating points
    float temp = 36.6f;

    // print them
    printf("age = %d\n", age);
    printf("count = %u\n", count);
    printf("letter = %c\n", letter);
    printf("temp = %.1f\n", temp);

    // embedded system: fixed-width size
    uint8_t register_val = 0xFF;
    uint16_t sensor_raw = 1023;
    uint32_t timestamp_ms = 4294967295U;

    printf("reister = 0x%02X\n", register_val);
    printf("sensor = %u\n", sensor_raw);
    printf("timestamp = %u\n", timestamp_ms);

    return 0;
}
