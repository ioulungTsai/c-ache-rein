#include <stdio.h>
#include <stdint.h>

void my_memcpy(void *dst, const void *src, size_t n);
void my_memset(void *ptr, uint8_t value, size_t n);
int my_strlen(const char *str);
int my_strcmp(const char *s1, const char *s2);
char *my_strcpy(char *dst, const char *src);

int main(void)
{
    // my_memcpy
    uint8_t src[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t dst[4] = {0x00, 0x00, 0x00, 0x00};

    printf("=== my_memcpy ===\n");
    printf("before: dst = %02X %02X %02X %02X\n",
            dst[0], dst[1], dst[2], dst[3]);
    my_memcpy(dst, src, 4);
    printf("after: dst = %02X %02X %02X %02X\n",
            dst[0], dst[1], dst[2], dst[3]);

    // my_memset
    printf("\n=== my_memset ===\n");
    my_memset(dst, 0xFF, 4);
    printf("after memset 0xFF: %02X %02X %02X %02X\n",
            dst[0], dst[1], dst[2], dst[3]);

    // my_strlen
    printf("\n=== my_strlen ===\n");
    char *name = "DHT11";
    printf("strlen(\"%s\") = %d\n", name, my_strlen(name));
    
    // my_strcmp
    printf("\n=== my_strcmp ===\n");
    printf("strcmp(\"DHT11\", \"DHT11\") = %d\n",
            my_strcmp("DHT11", "DHT11"));
    printf("strcmp(\"DHT11\", \"DHT22\") = %d\n",
            my_strcmp("DHT11", "DHT22"));
    printf("strcmp(\"DHT22\", \"DHT11\") = %d\n",
            my_strcmp("DHT22", "DHT11"));

    // my_strcpy
    printf("\n=== my_strcpy ===\n");
    char copy[32];
    my_strcpy(copy, "sensor_01");
    printf("copy = %s\n", copy);

    return 0;
}

void my_memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < n; i++) {
        d[i]=  s[i];
    }
}

void my_memset(void *ptr, uint8_t value, size_t n)
{
    uint8_t *p = (uint8_t *)ptr;
    for (size_t i = 0; i < n; i++) {
        p[i] = value;
    }
}

int my_strlen(const char *str)
{
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

int my_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

char *my_strcpy(char *dst, const char *src)
{
    char *start = dst;
    while (*src != '\0') {
        *dst = *src;
        dst++;
        src++;
    }
    *dst = '\0';
    return start;
}
