#include <stdio.h>
#include <stdint.h>

// function declarations
void print_array(uint8_t *arr, int len);
int find_max(uint8_t *arr, int len);
void reverse_array(uint8_t *arr, int len);
int my_strlen(char *str);
void my_strcpy(char *dst, char *src);

int main(void)
{
    // arrays
    uint8_t registers[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

    int len = 8;

    printf("original: ");
    print_array(registers, len);

    printf("max = 0x%02X\n", find_max(registers, len));

    reverse_array(registers, len);
    printf("reversed: ");
    print_array(registers, len);

    // strings
    char sensor_name[32] = "DHT11";
    char copy[32];

    printf("name = %s\n", sensor_name);
    printf("length = %d\n", my_strlen(sensor_name));

    my_strcpy(copy, sensor_name);
    printf("copy = %s\n", copy);

    return 0;
}

void print_array(uint8_t *arr, int len)
{
    for (int i = 0; i < len; i++) {
        printf("0x%02X ", arr[i]);
    }
    printf("\n");
}

int find_max(uint8_t *arr, int len)
{
    uint8_t max = arr[0];
    for (int i = 0; i < len; i++) {
        if(arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

void reverse_array(uint8_t *arr, int len)
{
    int left = 0;
    int right = len - 1;
    while (left < right) {
        uint8_t temp = arr[left];
        arr[left] = arr[right];
        arr[right] = temp;
        left++;
        right--;
    }
}

int my_strlen(char *str)
{
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void my_strcpy(char *dst, char *src)
{
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}
