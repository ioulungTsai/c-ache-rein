# Lessons

## Index
- [%02X Format Specifier](#m1-ex01-variables-attempt2---2026-03-07)

## m1-ex01-variables-attempt2 - 2026-03-07
- what is %02X?

### printf Format Specifier Breakdown (%X/%x)
    | Specifier | Meaning |
    |-----------|---------|
    | `%X` | Uppercase hex, no padding |
    | `%x` | Lowercase hex, no padding |
    | `%02X` | Uppercase hex, **min 2 digits**, pad with `0` |
    | `%08X` | Uppercase hex, **min 8 digits**, pad with `0` (common for 32-bit addresses) |
    | `%#X` | Uppercase hex with `0X` prefix |
    | `%#010X` | With prefix, min 10 chars (`0X` + 8 digits) |

    **General pattern:** `%[flags][width][specifier]`

    | Part | Options |
    |------|---------|
    | flags | `0` = zero-pad, `-` = left-align, `#` = add `0x`/`0X` prefix |
    | width | minimum field width (e.g. `8`) |
    | specifier | `x` = lowercase hex, `X` = uppercase hex |

    ```c
    printf("%X",   0xF);   // "F"
    printf("%02X", 0xF);   // "0F"
    printf("%08X", 0xF);   // "0000000F"
    printf("%#X",  0xF);   // "0XF"
    printf("%x",   0xAB);  // "ab"
    printf("%X",   0xAB);  // "AB"
    ```

    The `0` in `%02X` is the **flag** (zero-pad), `2` is the **minimum width**.
