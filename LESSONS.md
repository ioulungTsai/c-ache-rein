# Lessons

## Index
- [%02X Format Specifier](#m1-ex01-variables-attempt2---2026-03-07)
- [Pass by Value (Copy) or Reference (Pointer)](#m1-ex05-struct-attempt1---2026-03-12)
- [What Does the Compiler Consider as Using a Variable?](#what-does-the-compiler-consider-as-using-a-variable)
- [How to Silence the "Unused-But-Set Parameter" Warning](#how-to-silence-the-unused-but-set-parameter-warning)

---

## m1-ex01-variables-attempt2 — 2026-03-07

### What is `%02X`?

#### `printf` Format Specifier Breakdown (`%X` / `%x`)

| Specifier  | Meaning |
|------------|---------|
| `%X`       | Uppercase hex, no padding |
| `%x`       | Lowercase hex, no padding |
| `%02X`     | Uppercase hex, **min 2 digits**, pad with `0` |
| `%08X`     | Uppercase hex, **min 8 digits**, pad with `0` (common for 32-bit addresses) |
| `%#X`      | Uppercase hex with `0X` prefix |
| `%#010X`   | With prefix, min 10 chars (`0X` + 8 digits) |

**General pattern:** `%[flags][width][specifier]`

| Part      | Options |
|-----------|---------|
| `flags`   | `0` = zero-pad, `-` = left-align, `#` = add `0x`/`0X` prefix |
| `width`   | Minimum field width (e.g. `8`) |
| `specifier` | `x` = lowercase hex, `X` = uppercase hex |

```c
printf("%X",    0xF);   // "F"
printf("%02X",  0xF);   // "0F"
printf("%08X",  0xF);   // "0000000F"
printf("%#X",   0xF);   // "0XF"
printf("%x",    0xAB);  // "ab"
printf("%X",    0xAB);  // "AB"
```

> The `0` in `%02X` is the **flag** (zero-pad); `2` is the **minimum width**.

---

## m1-ex05-struct-attempt1 — 2026-03-12

### Pass by Value (Copy) or Reference (Pointer)

`sensor_update_direct` and `sensor_update_pointer` side by side is the best proof possible of copy vs. pointer. The output shows it clearly:

- After `sensor_update_pointer` → `temp = 37.7` ✅ original changed
- After `sensor_update_direct`  → `temp = 37.7` ← still 37.7, **not** 38.8

`sensor_update_direct` silently did nothing to the original. The compiler even warned:

```
warning: parameter 's' set but not used
```

That warning means: *"you modified a copy that nobody will ever see."* The compiler caught the logical mistake automatically. This is why `-Wall -Wextra` flags in your Makefile matter.

#### The Complete Lesson from Ex05

| Signature         | Mechanism | Access | Effect |
|-------------------|-----------|--------|--------|
| `sensor_t  s`     | Copy      | `.`    | Changes lost after function returns |
| `sensor_t *s`     | Address   | `->`   | Changes persist to original |

> Every driver function that modifies hardware state uses `*s`. This is the single most important concept in C for embedded work.

---

## m1-ex05-struct-attempt2 — 2026-03-12

### What Does the Compiler Consider as Using a Variable?

A variable or parameter is considered **"used"** if its value is **read**, **modified**, or if its **address is taken** (`&var`) at any point after declaration. If a variable is declared and initialized but never subsequently read or manipulated to affect program output, the compiler considers it unused and issues a warning.

#### 1. Variables (Local & Global)

**Counts as used:**
- Reading the value in an expression: `y = x`
- Passing to a function: `func(x)`
- Taking the address: `&x`
- Returning the value from a function

**Does NOT count as used:**
- Assigning a value but never reading it: `int x = 5;` (with no further use of `x`)
- Initializing then immediately overwriting without reading the first value

#### 2. Parameters (Function Arguments)

**Counts as used:**
- Parameter name appears in an expression or is assigned within the function body
- Parameter is passed to another function

**Counts as unused:**
- Parameter is declared in the signature but never referenced in the body
- Note: `argc`/`argv` in `main` are a common case where parameters go intentionally unused

#### 3. Special Cases & Exceptions

| Case | Behavior |
|------|----------|
| `volatile` variable | Compiler assumes hardware can change it; never marked unused |
| `(void)x;` | Portable way to silence unused-variable warnings |
| `__attribute__((unused))` | GCC/Clang-specific suppression |
| `[[maybe_unused]]` | C23 standard attribute |

#### Summary Table

| Code | Status |
|------|--------|
| `int x = 5;` *(no further code)* | ⚠️ Unused (warning) |
| `int x = 5; y = x;` | ✅ Used |
| `void func(int a) {}` | ⚠️ Unused parameter (warning) |
| `void func(int a) { (void)a; }` | ✅ Used (explicitly silenced) |
| `&x` *(address taken)* | ✅ Used |

---

### How to Silence the "Unused-But-Set Parameter" Warning

To remove the warning while keeping pass-by-value (copy) semantics, you have several options:

#### Option 1 — The Portable `(void)` Cast

Most common and widely recognized. Generates no assembly code.

```c
void sensor_update_direct(sensor_t s, float temp, float humidity) {
    (void)s; // Explicitly silence the warning
    s.last_temp = temp;
    s.last_humidity = humidity;
}
```

#### Option 2 — C23 Attribute: `[[maybe_unused]]`

The official modern approach (GCC 11+, Clang, C23 standard).

```c
void sensor_update_direct([[maybe_unused]] sensor_t s, float temp, float humidity) {
    s.last_temp = temp;
    s.last_humidity = humidity;
}
```

#### Option 3 — GCC/Clang: `__attribute__((unused))`

Widely used in Linux kernel and system programming. Not portable to MSVC.

```c
void sensor_update_direct(sensor_t s __attribute__((unused)), float temp, float humidity) {
    s.last_temp = temp;
    s.last_humidity = humidity;
}
```

#### Option 4 — Omit the Parameter Name (C23 / C++)

Only works if you also remove the assignments to `s`.

```c
// Only valid if the s.last_temp = ... lines are also removed
void sensor_update_direct(sensor_t /* unused */, float temp, float humidity) {
    // code that doesn't touch s
}
```

#### Option 5 — Disable the Warning Globally (Compiler Flag)

Generally discouraged — may hide real bugs elsewhere.

```bash
gcc -Wall -Wextra -Wno-unused-but-set-parameter -g -o main main.c
```

## m1-ex06-file-io-attempt1 — 2026-03-14

### File I/O — fopen modes
- "r"=read 
- "w"=write+destroy 
- "a"=append 
- "r+"=read+write

Always use "a" for logging — "w" destroys existing content.

Always fclose() — unclosed files lose data on embedded systems.

### return values convention
- 0 = success
- -1 = error in functions
- 1 = error in main()

the pattern from STM32 work
- HAL_OK=0
- HAL_ERROR=1 

    

### const rule of thumb
- Use const when passing strings you won't modify.
- Use const for fixed values and read-only hardware registers.

### format specifier flags
- %-3u = left-align, minimum 3 chars wide, unsigned int.
- %hhu = unsigned char (uint8_t) — must match exact size in fscanf.
- %f in fscanf = float*
- %lf in fscanf = double* — must match exactly.

### File I/O function signatures
- fopen  → FILE* fopen(const char*, const char*) — returns NULL on fail
- fclose → int fclose(FILE*) — flushes buffer, always call
- fprintf → int fprintf(FILE*, const char*, ...) — pass values
- fscanf  → int fscanf(FILE*, const char*, ...) — pass addresses
- printf = fprintf(stdout, ...) — stdout is just a pre-opened FILE*
- fscanf returns number of items matched — use == N in while loop

### C naming conventions
- ALL_CAPS    → macros only (#define)
- lowercase_t → typedef types (sensor_log_t, uint8_t)
- lowercase   → functions and variables
- FILE uppercase = historical exception from 1970s C standard library
- Every type = memory size + interpretation, nothing more

### Type interpretation — two levels
- Declaration: compiler generates correct instructions for the type
- Read:        same bits mean different things through different types
- Cast:        deliberately reinterpret same bytes — type punning
- 0xFF as uint8_t = 255, as int8_t = -1 — same bits, different meaning
- Hardware registers = raw bits you interpret per datasheet field

### Type punning — safe vs dangerous
- Safe:      union with integer types — packet parsing, register overlays
- Dangerous: float↔integer via pointer cast — undefined behavior
- Safe float: use memcpy(&val, &raw, sizeof(float)) — no UB
- Embedded punning = integers only, union-based, datasheet-driven

## m2-ex03 — malloc, free, stack vs heap

### The complete free pattern
```c
free(ptr);
ptr = NULL;
```
free() releases memory but pointer still holds old address — dangling pointer.
Setting NULL immediately means accidental use crashes loudly instead of
silently corrupting memory. Silent corruption in embedded = worst category
of bug — random behavior, only reproducible under specific conditions.

### malloc NULL check
malloc fails when heap is exhausted, fragmented, or no contiguous block available.
Without NULL check — dereference NULL on next line — crash with no useful message.
With NULL check — controlled failure with meaningful error at the right place.

### Stack vs heap memory layout
```
High addresses
├── Stack  ← grows DOWN, each function call gets lower address
│
│   (free space between them)
│
├── Heap   ← grows UP, each malloc gets higher address
├── BSS    ← uninitialized globals
├── Data   ← initialized globals
└── Text   ← your code
Low addresses
```
On STM32F407: RAM starts at 0x20000000, stack at top grows down,
heap at bottom grows up — if they meet → stack overflow, no OS to catch it.
FreeRTOS task stack sizes defined precisely for this reason.

### Proving stack grows down
Local variables in same function — unreliable, compiler arranges freely.
Recursive function calls — reliable, each call gets lower address:
```
depth 0: 0x7ffc...e967
depth 1: 0x7ffc...e937  ← 0x30 (48 bytes) lower
depth 2: 0x7ffc...e907  ← 0x30 lower
depth 3: 0x7ffc...e8d7  ← 0x30 lower
```
48 bytes = return address + saved registers + local variables + alignment.

## m2-ex04 — implementing standard library functions

### why void* in my_memcpy
memcpy is generic — must accept any pointer type, not just uint8_t*.
void* accepts any pointer without cast at call site.
Inside function: cast to uint8_t* to walk byte by byte — smallest unit.
Same pattern used in real memcpy, memset, memmove in standard library.
```c
/* accepts uint32_t*, sensor_t*, anything */
void my_memcpy(void *dst, const void *src, size_t n)
{
    uint8_t *d = (uint8_t *)dst;   /* cast here, not at call site */
    const uint8_t *s = (const uint8_t *)src;
    ...
}
```

### why (unsigned char) in my_strcmp
char is signed on most platforms: range -128 to 127.
bytes above 127 become negative as signed char.
subtraction of signed chars gives wrong ordering for values >127.
(unsigned char) cast forces range 0-255 — correct comparison always.
C standard requires strcmp to compare as unsigned char for this reason.

### why my_strcpy saves start pointer
pointer arithmetic moves dst forward during copy.
without saving start, return value would point to end of string not beginning.
caller needs the beginning — save before moving, return saved start.
pattern: save head → walk → return head.

## m2-ex04 — my_strcmp return value

strcmp walks both strings until a mismatch or \0.
returns: (unsigned char)*s1 - (unsigned char)*s2 at first mismatch.

only the sign matters:
- 0        → strings identical
- negative → s1 alphabetically before s2
- positive → s1 alphabetically after s2

example:
  "DHT11" vs "DHT22" → mismatch at '1' vs '2' → 49 - 50 = -1
  "DHT22" vs "DHT11" → mismatch at '2' vs '1' → 50 - 49 =  1

always compare with == 0, < 0, > 0 — never rely on exact value.

## m2-ex04 — why implement standard library functions

### three purposes
1. understand what happens inside — no black box, know where bugs hide
2. classic embedded interview questions — memcpy, strlen, strcmp asked constantly
3. bare metal targets may have no libc — must implement yourself

### real embedded use cases
- my_memcpy: copy data to hardware register or DMA buffer
- my_memset: zero out buffer before DMA transfer
- my_strcmp: match sensor or device name from config

## m2-ex04 — my_strcmp real use cases

### three scenarios
1. exact match (== 0): match sensor type, select correct driver
2. ordering (< 0, > 0): sort device list, find alphabetical position
3. binary search (all three): search sorted command or config table

### rule
never rely on exact value (-1, 1, -17...)
only trust the sign — compiler and platform dependent exact values
always compare with == 0, < 0, > 0

## m2-ex04 — void* interface vs uint8_t* implementation

void* at interface = generality — caller passes any pointer type
uint8_t* inside    = implementation — must cast to do arithmetic

these are separate concerns:
- interface: what the caller sees — never changes
- implementation: how bytes are moved — always byte by byte

void* cannot do pointer arithmetic — compiler doesn't know step size
uint8_t* steps by 1 byte — correct for any type
caller passes byte count n — function doesn't know or care about element type

casting inside does not reduce generality — it enables the work
