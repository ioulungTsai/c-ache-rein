#include <stdio.h>

int main(void)
{
    /* intergers */
    int age = 10;
    unsigned int count = 100;

    /* characters */
    char letter = 'A';

    /* floating point */
    float temperature = 36.6f;

    /* print them */
    printf("age = %d\n", age);
    printf("count = %u\n", count);
    printf("letter = %c\n", letter);
    printf("temperature = %.1f\n", temperature);

    return 0;
}
