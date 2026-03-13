#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t sensor_id;
    float temperature;
    float humidity;
    uint32_t timestamp;
} sensor_log_t;

// function declarations
int log_write(const char *filename, sensor_log_t *entry);
int log_read(const char *filename);
void log_print(sensor_log_t *entry);

int main(void)
{
    // write three sensor readings to file
    sensor_log_t entries[3] = {
        {1, 36.6f, 65.0f, 1000},
        {1, 37.1f, 63.5f, 2000},
        {1, 36.9f, 64.0f, 3000}
    };

    for (int i = 0; i < 3; i++) {
        if (log_write("sensor.log", &entries[i]) != 0) {
            printf("error: failed to write entry %d\n", i);
            return 1;
        }
    }

    printf("wrote 3 entries to sensor.log\n\n");

    // read them back
    printf("reading sensor.log:\n");
    log_read("sensor.log");

    return 0;
}

int log_write(const char *filename, sensor_log_t *entry)
{
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) {
        printf("error: cannot open %s for writing\n", filename);
        return -1;
    }

    fprintf(fp, "id=%u temp=%.1f humidity=%.1f time=%u\n",
            entry->sensor_id,
            entry->temperature,
            entry->humidity,
            entry->timestamp);

    fclose(fp);
    return 0;
}

int log_read(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("error: cannot open %s for reading\n", filename);
        return -1;
    }

    sensor_log_t entry;
    while (fscanf(fp, "id=%hhu temp=%f humidity=%f time=%u\n",
                    &entry.sensor_id,
                    &entry.temperature,
                    &entry.humidity,
                    &entry.timestamp) == 4) {
        log_print(&entry);
    }

    fclose(fp);
    return 0;
}

void log_print(sensor_log_t *entry)
{
    printf("id=%-3u temp=%-6.1f humidity=%-6.1f time=%u\n",
        entry->sensor_id,
        entry->temperature,
        entry->humidity,
        entry->timestamp);
}
