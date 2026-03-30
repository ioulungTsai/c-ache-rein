#include <stdio.h>
#include <stdint.h>

// fixed-point configuration
#define FIXED_SHIFT     8                       // 8 fractional bits
#define FIXED_SCALE     (1 << (FIXED_SHIFT))    // 256 - scaling factor

// conversion macros
#define FLOAT_TO_FIXED(x)   ((int32_t)((x) * (FIXED_SCALE))) 
#define FIXED_TO_FLOAT(x)   ((float)(x) / (FIXED_SCALE)) 
#define INT_TO_FIXED(x)     ((int32_t)(x) * (FIXED_SCALE)) 
#define FIXED_TO_INT(x)     ((int32_t)(x) / (FIXED_SCALE)) 

// arithmetic macros
#define FIXED_ADD(a, b)     ((a) + (b))
#define FIXED_SUB(a, b)     ((a) - (b))
#define FIXED_MUL(a, b)     ((a) * (b) >> (FIXED_SHIFT))
#define FIXED_DIV(a, b)     (((a) << (FIXED_SHIFT)) / (b))

// sensor scaling - real embedded use case
#define ADC_MAX             4095    // 12-bit ADC
#define VREF_MV             3300    // 3.3V reference in mV

// function declarations
int32_t adc_to_voltage_mv(uint16_t adc_raw);
int32_t voltage_to_temp(int32_t voltage_mv);
void print_fixed(const char *label, int32_t fixed);

int main(void)
{
    printf("=== fixed-point basics ===\n");

    int32_t a = FLOAT_TO_FIXED(3.14f);
    int32_t b = FLOAT_TO_FIXED(2.00f);

    printf("a = 3.14f -> fixed = %d\n", a);
    printf("b = 2.00f -> fixed = %d\n", b);

    print_fixed("a",        a);
    print_fixed("b",        b);
    print_fixed("a + b",    FIXED_ADD(a, b));
    print_fixed("a - b",    FIXED_SUB(a, b));
    print_fixed("a * b",    FIXED_MUL(a, b));
    print_fixed("a / b",    FIXED_DIV(a, b));

    printf("\n=== sensor pipeline ===\n");
    uint16_t adc_samples[4] = {0, 1024, 2048, 4095};

    for (int i = 0; i < 4; i++) {
        int32_t voltage = adc_to_voltage_mv(adc_samples[i]);
        int32_t temp    = voltage_to_temp(voltage);
        printf("ADC=%4u voltage=%4dmV temp=",
                adc_samples[i], voltage);
        print_fixed("", temp);
    }

    printf("\n=== float vs fixed comparison ===\n");
    float   f_result = 3.14f * 2.0f;
    int32_t x_result = FIXED_MUL(FLOAT_TO_FIXED(3.14f),
                                    FLOAT_TO_FIXED(2.0f));

    printf("float:      %.6f\n", f_result);
    printf("fixed:      %.6f\n", FIXED_TO_FLOAT(x_result));
    printf("difference: %.6f\n", f_result - FIXED_TO_FLOAT(x_result));

    return 0;
}

int32_t adc_to_voltage_mv(uint16_t adc_raw)
{
    // voltage_mv = adv_raw * VREF_MV / ADC_MAX
    return (int32_t) adc_raw * VREF_MV / ADC_MAX;
}

int32_t voltage_to_temp(int32_t voltage_mv)
{
    // simplified LM35 sensor: 10mV per degree C
    // temp = voltage_mv / 10 in fixed-point
    int32_t fixed_voltage   = INT_TO_FIXED(voltage_mv);
    int32_t fixed_ten       = INT_TO_FIXED(10);
    return FIXED_DIV(fixed_voltage, fixed_ten);
}

void print_fixed(const char *label, int32_t fixed)
{
    if (label[0] != '\0') {
        printf("%-10s = %8d (%.4f)\n",
                label, fixed, FIXED_TO_FLOAT(fixed));
    } else {
        printf("%.4f\n", FIXED_TO_FLOAT(fixed));
    }
}
