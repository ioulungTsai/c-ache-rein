#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t address;
    uint8_t value;
    uint8_t is_readable;
    uint8_t is_writable;
} reg_t;

typedef struct {
    char name[32];
    uint8_t i2c_address;
    reg_t *regs; // points to array of registers
    size_t reg_count;
    float last_temp;
} sensor_t;

// function declarations
sensor_t *sensor_create(const char *name, uint8_t addr, size_t reg_count);
void sensor_destroy(sensor_t *s);
void sensor_add_reg(sensor_t *s, size_t idx, uint8_t addr,
                    uint8_t readable, uint8_t writable);
void sensor_print(sensor_t *s);

int main(void)
{
    // create sensor on heap
    sensor_t *dht11 = sensor_create("DHT11", 0x27, 3);
    if (dht11 == NULL) {
        printf("error: sensor_create failed\n");
        return 1;
    }

    // add registers
    sensor_add_reg(dht11, 0, 0x00, 1, 0); // temp reg
    sensor_add_reg(dht11, 1, 0x01, 1, 0); // humidity reg
    sensor_add_reg(dht11, 2, 0x02, 1, 1); // config reg

    dht11->last_temp = 36.6f;

    sensor_print(dht11);

    // pointer to sensor pointer
    sensor_t **ptr_to_sensor = &dht11;
    printf("sensor name via **: %s\n", (*ptr_to_sensor)->name);

    sensor_destroy(dht11);
    dht11 = NULL;

    return 0;
}

sensor_t *sensor_create(const char *name, uint8_t addr, size_t reg_count)
{
    sensor_t *s = malloc(sizeof(sensor_t));
    if (s == NULL) return NULL;

    s->regs = malloc(reg_count * sizeof(reg_t));
    if (s->regs == NULL) {
        free(s);
        return NULL;
    }

    size_t i = 0;
    while (name[i] != '\0' && i < 31) {
        s->name[i] = name[i];
        i++;
    }
    s->name[i] = '\0';
    s->i2c_address = addr;
    s->reg_count = reg_count;
    s->last_temp = 0.0f;

    return s;
}

void sensor_destroy(sensor_t *s)
{
    if (s == NULL) return;
    free(s->regs);
    s->regs = NULL;
    free(s);
}

void sensor_add_reg(sensor_t *s, size_t idx, uint8_t addr,
                    uint8_t readable, uint8_t writable)
{
    if (idx >= s->reg_count) return;
    s->regs[idx].address = addr;
    s->regs[idx].value = 0x00;
    s->regs[idx].is_readable = readable;
    s->regs[idx].is_writable = writable;
}

void sensor_print(sensor_t *s)
{
    printf("sensor      = %s\n", s->name);
    printf("i2c addr    = 0x%02X\n", s->i2c_address);
    printf("temp        = %.1f\n", s->last_temp);
    printf("resisters:\n");
    for (size_t i = 0; i < s->reg_count; i++) {
        printf("    [%zu] addr=0x%02X r=%u w=%u\n",
                i,
                s->regs[i].address,
                s->regs[i].is_readable,
                s->regs[i].is_writable);
    }
}
