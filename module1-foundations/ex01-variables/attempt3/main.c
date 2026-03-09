// this is m1-ex01-attempt3
// study for the f in 36.6f is a suffix 
// that specifies the literal is a float (single-precision floating-point) type 
// rather than the default double type
// and more ...

#include <stdio.h>
#include <stdint.h>

int main(void)
{
    // floating point
    float temperature1 = 36.6; // double literal assigned to float
    float temperature2 = 36.6f; // float literal assigned to float

    // prove 36.6 is double by checking the literal's size directly
    printf("sizeof(36.6) = %zu\n", sizeof(36.6)); // literal double
    printf("sizeof(36.6f) = %zu\n", sizeof(36.6f)); // literal float

    // both variables are still float - sizeof variable vs literal
    printf("sizeof(temperature1) = %zu\n", sizeof(temperature1));
    printf("sizeof(temperature2) = %zu\n", sizeof(temperature2));

    // The key insight:
    // sizeof(36.6) measures the literal — proves compiler treats it as double (8 bytes). 
    // sizeof(temperature1) measures the variable — always 4 because you declared it as float. 
    // The conversion from double to float happened silently during assignment.

    float f1 = 36.6; // double truncated to float
    float f2 = 36.6f; // removes implicit conversion but does NOT remove floating point precision loss
    double d = 36.6; // stays double

    printf("float = %.10f\n", f1);
    printf("float = %.10f\n", f2);
    printf("double = %.10lf\n", d);

    return 0;
}
