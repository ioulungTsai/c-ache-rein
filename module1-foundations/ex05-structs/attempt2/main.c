#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t address;
    uint8_t value;
    uint8_t is_readable;
    uint8_t is_writable;
} register_t;

typedef struct {
    char name[32];
    uint8_t i2c_address;
    register_t temp_reg;
    register_t humidity_reg;
    float last_temp;
    float last_humidity;
} sensor_t;

// function declarations
void sensor_init(sensor_t *s, char *name, uint8_t addr);
void sensor_print_direct(sensor_t s); // receives copy - uses dot
void sensor_print_pointer(sensor_t *s); // receives pointer - use arrow
void sensor_update_direct(sensor_t s, float temp, float humidity);
void sensor_update_pointer(sensor_t *s, float temp, float humidity);

int main(void)
{
    // direct variable - use dot
    sensor_t dht11;
    dht11.i2c_address = 0x27;
    dht11.last_temp = 0.0f;
    dht11.last_humidity = 0.0f;
    dht11.temp_reg.address = 0x00;
    dht11.temp_reg.is_readable = 1;
    dht11.temp_reg.is_writable = 0;
    dht11.humidity_reg.address = 0x01;
    dht11.humidity_reg.is_readable = 1;
    dht11.humidity_reg.is_writable = 0;

    // manually copy name using dot
    char *name = "DHT11";
    int i = 0;
    while (name[i] != '\0') {
        dht11.name[i] = name[i];
        i++; 
    }
    dht11.name[i] = '\0';

    printf("=== dot syntax (direct variable) ===\n");
    printf("name        = %s\n", dht11.name);
    printf("i2c addr    = 0x%02X\n", dht11.i2c_address);
    printf("temp reg    = 0x%02X\n", dht11.temp_reg.address);
    printf("temp        = %.1f\n", dht11.last_temp);
    printf("humidity    = %.1f\n", dht11.last_humidity);
    printf("---\n");

    // pointer - use arrow
    sensor_t *ptr = &dht11;
    ptr->last_temp = 36.6f;
    ptr->last_humidity = 65.0f;

    printf("=== arrow syntax (pointer) ===\n");
    printf("name        = %s\n", ptr->name);
    printf("i2c addr    = 0x%02X\n", ptr->i2c_address);
    printf("temp reg    = 0x%02X\n", ptr->temp_reg.address);
    printf("temp        = %.1f\n", ptr->last_temp);
    printf("humidity    = %.1f\n", ptr->last_humidity);
    printf("---\n");

    // prove they are the same thing
    printf("=== proving dot and arrow are identical ===\n");
    printf("dht11.last_temp = %.1f\n", dht11.last_temp); // dot
    printf("ptr->last_temp = %.1f\n", ptr->last_temp); // arrow
    printf("(*ptr).last_temp = %.1f\n", (*ptr).last_temp); // arrow expanded

    // pass to functions
    printf("=== function with copy (dot inside) ===\n");
    sensor_print_direct(dht11); // passes copy

    printf("=== function with pointer (arrow inside) ===\n");
    sensor_print_pointer(ptr); // passes address

    sensor_update_pointer(ptr, 37.7f, 77.7f);

    printf("=== after update_pointer ===\n");
    sensor_print_direct(dht11);
    sensor_print_pointer(ptr);

    sensor_update_direct(dht11, 38.8f, 88.8f);

    printf("=== after update_direct ===\n");
    sensor_print_direct(dht11);
    sensor_print_pointer(ptr);
    
    return 0;
}

void sensor_print_direct(sensor_t s) // copy - dot
{
    printf("name        = %s\n", s.name);
    printf("i2c addr    = 0x%02X\n", s.i2c_address);
    printf("temp        = %.1f\n", s.last_temp);
    printf("humidity    = %.1f\n", s.last_humidity);
    printf("---\n");
}

void sensor_print_pointer(sensor_t *s)
{
    printf("name        = %s\n", s->name);
    printf("i2c addr    = 0x%02X\n", s->i2c_address);
    printf("temp        = %.1f\n", s->last_temp);
    printf("humidity    = %.1f\n", s->last_humidity);
    printf("---\n");
}

void sensor_update_direct(sensor_t s, float temp, float humidity)
{
    // sometimes we need the copy and leave origin unchganged
    (void)s; // explicitly silence the warning
    s.last_temp = temp;
    s.last_humidity = humidity;
}

void sensor_update_pointer(sensor_t *s, float temp, float humidity)
{
    s->last_temp = temp;
    s->last_humidity = humidity;
}
