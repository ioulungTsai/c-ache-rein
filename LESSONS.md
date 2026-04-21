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

## m2-ex05 — structs with heap allocation

### naming conflicts with system headers

register_t is typedef'd as long int in sys/types.h on Linux.
POSIX reserves all identifiers ending in _t — technically yours too,
but in practice short unique names like reg_t, sensor_t are safe.
if you see "conflicting types" error on a typedef — check system headers first.
quick fix: rename your type, add project prefix e.g. app_reg_t.

### cleanup on partial failure
free all successful allocations before returning NULL on inner failure.
without cleanup: partial allocations leak on every failed creation.
on embedded systems running months: repeated leaks exhaust heap silently.

### free in reverse order of allocation
always free inner members before freeing outer struct.
freeing outer first makes inner address invalid — undefined behavior.
rule: last allocated = first freed.

### pointer to pointer — sensor_t**
needed when function must modify the pointer itself not what it points to.
same rule as pointer to variable — pass address of what you want to change.

three real uses:
1. factory function: void create(sensor_t **out) — *out = malloc(...)
2. linked list: void insert(node_t **head) — modifies caller's head
3. array of pointers: sensor_t *sensors[4] — sensor_t **ptr = sensors

call site always passes address of pointer:
  sensor_t *s = NULL;
  create(&s);   /* &s = address of pointer s */

## m2-ex06 — linked list

### why struct node needs a name
anonymous struct has no name to self-reference during definition.
node_t doesn't exist yet while compiler reads the struct body.
struct node is available immediately — only way to point to same type.

### list_push_front pointer steps
before: head → A → B → NULL
step 1: n = malloc new node (0xFF)
step 2: n->next = *head    → new node points to A
step 3: *head = n          → head now points to new node
after:  head → 0xFF → A → B → NULL
order matters — swap steps 2 and 3 and head points to itself

### list_free — save next before free
free() releases memory — contents immediately undefined.
accessing curr->next after free = undefined behavior.
always save what you need before freeing:
  node_t *next = curr->next;  ← save while valid
  free(curr);                  ← now safe
  curr = next;                 ← use saved value

### linked list vs array
array:  fixed size, contiguous memory, O(1) random access
list:   dynamic size, scattered memory, O(n) traversal
embedded use: command queues, event lists, dynamic sensor lists

## m3-ex01 — bitwise macros

### four operations summary
SET_BIT    |=  (1U << bit)    force bit to 1
CLEAR_BIT  &= ~(1U << bit)    force bit to 0
TOGGLE_BIT ^=  (1U << bit)    flip bit
CHECK_BIT  (>> bit) & 1U      read bit value — 0 or 1

### CLEAR_BIT walk through
CLEAR_BIT(reg, 3):
  1U << 3    = 00001000   mask of target bit
  ~00001000  = 11110111   all bits flipped
  reg & 11110111          AND preserves all bits except target
  target bit forced to 0, all others unchanged

### 1U vs 1
1 is signed int — shifting into sign bit = undefined behavior.
1U is unsigned int — no sign bit, safe for 8/16/32 bit registers.
1UL for uint32_t registers on platforms where int is 16-bit.
same family as float literal suffixes: f, U, UL, ULL, L.

### why parentheses around every macro parameter
macros expand as text — operator precedence applies after expansion.
+ has HIGHER precedence than << — not lower.
so 1U << 3+1 without parens accidentally works — (3+1) evaluated first.
but dangerous case is when macro result used in larger expression:
  CHECK_BIT(reg, 2) + 1 without outer parens
  expands to: reg >> 2 & 1U + 1
  + higher than & → evaluates as reg >> 2 & (1U + 1) = reg >> 2 & 2 ← wrong
rule: wrap every parameter and entire macro expression in () always.

## m3-ex02 — volatile, const, static

### const vs #define
```c
#define MAX_SENSORS_MACRO  8    /* preprocessor — no type, no address, no memory */
const uint8_t MAX_SENSORS = 8;  /* typed, has address, compiler enforced read-only */
```
#define: text substitution before compile, no type checking, no memory allocated.
const: real variable, has type, has memory address, compiler enforces read-only.
embedded rule: prefer const for typed values, #define for flags and parameterized macros.

**use #define when:**
- value is used as array size or case label — must be compile-time constant
- creating function-like macros: #define SET_BIT(r,b) ((r) |= (1U<<(b)))
- conditional compilation: #ifdef, #ifndef, #endif
- no type needed or type would be misleading
```c
#define BUFFER_SIZE   256        /* array size — must be #define */
#define STATUS_BUSY   0x01       /* bit flag — no type needed */
#ifdef DEBUG                     /* conditional compile */
```

**use const when:**
- value has a specific type that matters
- you want compiler type checking at usage
- value is a pointer or struct — #define cannot express these cleanly
- debugging — const has an address, visible in debugger
```c
const uint8_t  MAX_SENSORS  = 8;       /* typed, debugger visible */
const float    VREF         = 3.3f;    /* type matters — float not int */
const uint8_t *LOOKUP_TABLE = table;   /* pointer — #define cannot do this */
```

**embedded specific rule:**
- hardware register addresses → always const volatile uint32_t *
- fixed lookup tables → const array
- bit flags and masks → #define
- buffer sizes used as array dimensions → #define
- everything else typed and fixed → const

### static local variable
```c
void counter_demo(void) {
    static uint32_t count = 0;  /* initialized once, persists between calls */
    count++;
}
```
regular local: stack, initialized every call, destroyed on return.
static local:  data segment, initialized once at program start, survives return.
use for: call counters, persistent state, one-time initialization flags.

### static global variable
```c
static uint32_t error_count = 0;  /* visible only in this file */
```
without static: any file can access with extern — exposed globally.
with static:    locked to current file — other files cannot see it.
static hides, extern exposes — opposite directions.
use for: module-private state, encapsulation in C without classes.

### volatile — concept
compiler optimization caches variable values in CPU registers.
without volatile: compiler assumes value only changes when your code changes it.
with volatile:    compiler forced to re-read from memory every access.
                  hardware or another thread might change it anytime.

### volatile — three assembly cases

case 1: empty body, no volatile
```c
uint32_t status = 0x00000001;
while (status & 0x01) { }
```
compiler: status never changes → loop runs forever → tight infinite jump
assembly: .L8: jmp .L8  ← infinite, no memory reads, no ret

case 2: assignment inside, no volatile
```c
uint32_t status = 0x00000001;
while (status & 0x01) { status = 0x00000000; }
```
compiler: status 1→0 in one iteration → calculates at compile time → deletes loop
assembly: ret  ← one instruction, entire loop gone

case 3: assignment inside, with volatile
```c
volatile uint32_t status = 0x00000001;
while (status & 0x01) { status = 0x00000000; }
```
compiler: cannot trust memory — hardware might write back → real loop required
assembly:
  movl $1, -4(%rsp)      write to memory
  movl -4(%rsp), %eax    READ from memory first time
  testb $1, %al          check bit 0
  je .L8                 skip if clear
.L10:
  movl $0, -4(%rsp)      write 0
  movl -4(%rsp), %eax    READ again — hardware might have changed it
  testb $1, %al          check bit 0
  jne .L10               loop back if still set
.L8:
  ret                    can exit — hardware clears bit

### volatile — critical difference
case 2 vs case 3: one keyword difference, completely different assembly
  without volatile → ret only        loop eliminated, one instruction
  with volatile    → real loop       memory read every iteration, can exit

### why printf inside loop blocks optimization proof
printf is external function — compiler cannot prove it doesn't modify status.
keeps loop as safety measure — volatile difference hidden.
always use clean isolated functions with no side effects for assembly proof.

### volatile — real embedded consequence
```c
/* waiting for I2C byte received flag — STM32 SR1 register */
volatile uint32_t *SR1 = (volatile uint32_t *)0x40005414;
while (!(*SR1 & I2C_SR1_RXNE)) { }
```
without volatile: compiler may delete polling loop silently.
with volatile:    correct loop generated, re-reads register every iteration.
always use volatile for memory-mapped hardware registers.

### volatile — runtime proof limitation
your code changes status inside loop — compiler sees change either way.
hardware changes register from outside — compiler cannot see it.
full runtime proof requires:
  real hardware: MCU register changed by peripheral
  threads: another thread modifies shared variable (module 5)
assembly proof is correct and complete proof on Linux without hardware.

## m3-ex03 — memory-mapped register simulation

### why volatile in REG_READ and REG_WRITE
```c
#define REG_READ(base, offset) \
    (*((volatile uint32_t *)((base) + (offset))))
```
without volatile: compiler caches first read in CPU register, never reads hardware again.
with volatile: every REG_READ generates real memory load, every REG_WRITE generates real store.
hardware sees every access — critical for status registers and data registers.

### why uintptr_t in macro body, volatile uint32_t* in parameter

function parameter:
```c
void periph_init(volatile uint32_t *base)  /* type-safe pointer */
```
macro body:
```c
(uintptr_t)base + offset   /* raw byte arithmetic */
```
pointer arithmetic steps by sizeof the pointed type:
  base + 0x01 on volatile uint32_t* = base + 4 bytes  ← wrong
  (uintptr_t)base + 0x01             = base + 1 byte   ← correct

uintptr_t: unsigned integer guaranteed to hold any pointer value.
  32-bit system: uintptr_t = 4 bytes — matches pointer size
  64-bit system: uintptr_t = 8 bytes — matches pointer size
  uint32_t would truncate pointer on 64-bit — wrong and dangerous

### register offset layout
```c
static volatile uint32_t fake_peripheral[3] = {
    0x00000000,   /* [0] CR offset 0x00 — uint32_t = 4 bytes each */
    0x00000002,   /* [1] SR offset 0x04 */
    0x00000000    /* [2] DR offset 0x08 */
};
```
array of uint32_t naturally aligns — each element 4 bytes apart.
matches REG_CR=0x00, REG_SR=0x04, REG_DR=0x08 exactly.

### simulation vs real STM32
```c
/* simulation */
static volatile uint32_t fake_peripheral[3];
volatile uint32_t *periph = fake_peripheral;  /* address assigned at runtime */

/* real STM32 */
#define GPIOA_BASE  0x40020000U
volatile uint32_t *periph = (volatile uint32_t *)GPIOA_BASE;  /* fixed datasheet address */
```
three differences:
  address: runtime-assigned vs fixed datasheet value
  initial values: set by your code vs set by hardware reset state
  who changes registers: your code only vs hardware AND your code

volatile is optional in simulation — required on real hardware.

### uintptr_t vs other patterns

uintptr_t primary purpose here: byte-accurate arithmetic, not portability.
pointer arithmetic steps by sizeof pointed type — wrong for byte offsets.
uintptr_t steps by 1 byte — correct for register offset arithmetic.

three MMIO patterns:
1. struct overlay (recommended — CMSIS style)
   typedef struct { volatile uint32_t CR; volatile uint32_t SR; } PERIPH_t;
   GPIOA->CR |= (1U << 0) — no arithmetic needed, cleanest approach
   used by ST, NXP, all major MCU vendors in CMSIS headers

2. uint8_t* cast (common alternative to uintptr_t)
   *((volatile uint32_t *)((uint8_t *)(base) + offset))
   uint8_t* steps by 1 byte — same result as uintptr_t

3. uintptr_t cast (what this exercise uses)
   *((volatile uint32_t *)((uintptr_t)(base) + offset))
   integer arithmetic — explicit byte stepping

uintptr_t defined in C99 stdint.h — not new, just less common in older embedded code.
your STM32/ESP32 projects use pattern 1 — struct overlay via CMSIS headers.
pattern 1 avoids all manual arithmetic — compiler handles offsets via struct layout.

## m3-ex04 — fixed-point math

### why fixed-point
MCUs without FPU use software-emulated float — slow and expensive.
fixed-point stores scaled integers — all runtime math stays in integer domain.
convert at boundary once, compute in integers always.

### Q8 format
```c
#define FIXED_SHIFT  8
#define FIXED_SCALE  (1 << FIXED_SHIFT)  /* 256 */
```
lower 8 bits = fractional part (0 to 255)
upper 24 bits = integer part
precision floor = 1/256 = 0.00390625 exactly
fractional range = 0/256=0.0000 to 255/256=0.9961
256/256 = 1.0000 — becomes integer part, fractional resets to 0

### precision floor — values lost below threshold
any value smaller than 1/FIXED_SCALE truncated to zero:
  0.001 * 256 = 0.256 → stored as 0 → completely lost
  0.003 * 256 = 0.768 → stored as 0 → completely lost
  0.004 * 256 = 1.024 → stored as 1 → recovers as 0.00390625

choose FIXED_SHIFT based on smallest value needed:
  need 0.001 precision → Q10 minimum (1/1024 = 0.000977)
  need 0.01  precision → Q7  minimum (1/128  = 0.0078)
  need 0.1   precision → Q4  minimum (1/16   = 0.0625)

### why FIXED_MUL shifts right
```c
#define FIXED_MUL(a, b) (((a) * (b)) >> FIXED_SHIFT)
```
a = 3.14 × 256 = 803   b = 2.00 × 256 = 512
a × b = 803 × 512 = 411136  — scale factor now 256 × 256 = 65536
>> 8 removes extra scale: 411136 / 256 = 1606  — back to Q8
1606 / 256 = 6.2734  ✓
multiplication doubles scale factor — shift right removes the extra.

### why FIXED_DIV shifts left first
```c
#define FIXED_DIV(a, b) (((a) << FIXED_SHIFT) / (b))
```
a = 803   b = 512
a / b = 803 / 512 = 1  — integer division discards fraction
(a << 8) / b = (803 × 256) / 512 = 401  — Q8 result
401 / 256 = 1.5664  ✓
division discards fractional bits — shift left first preserves them.

### FPU boundary rule
```c
FLOAT_TO_FIXED  → FPU — boundary conversion, used once
FIXED_TO_FLOAT  → FPU — display only, used once
INT_TO_FIXED    → no FPU — integer multiply
FIXED_ADD/SUB   → no FPU — integer add/subtract
FIXED_MUL       → no FPU — integer multiply + right shift
FIXED_DIV       → no FPU — left shift + integer divide
```
convert at boundary, compute in integers — FPU touched minimally.
critical on MCUs without FPU: STM32F0, STM32F1, Cortex-M0 targets.

### sensor pipeline pattern
```
ADC raw → integer voltage (mV) → fixed-point temp → display
```
adc_to_voltage_mv: pure integer, no fixed-point needed
voltage_to_temp: converts to fixed-point, divides by 10
print_fixed: FIXED_TO_FLOAT for display only — float never used in pipeline
82.5 preserved correctly — Q8 fractional bits hold the .5 exactly (128/256)

## m3-ex05 — circular buffer

### what it is
fixed-size array where head and tail wrap around — no shifting needed.
FIFO: first in first out — tail always points to oldest data.
used everywhere in embedded: UART RX/TX, SPI DMA, sensor data queues.

### head and tail roles
```c
head → next write position  (where next push goes)
tail → next read position   (where next pop comes from)
count → how many valid bytes currently in buffer
```
push: write to data[head], advance head, count++
pop:  read from data[tail], advance tail, count--

### why modulo wraps correctly
```c
head = (head + 1) % BUFFER_SIZE
```
head reaches BUFFER_SIZE-1 → next push: (7+1) % 8 = 0 → wraps to start.
without modulo: head goes past array end → undefined behavior.
power of 2 optimization: (head+1) & (BUFFER_SIZE-1) — bitwise AND, no division.
critical on MCUs without hardware divider — AND is one instruction.

### why count not head==tail for full/empty
```
head == tail with no count:
  empty: tail caught up to head after popping all
  full:  head lapped tail after filling completely
  same position — impossible to distinguish

with count:
  head==tail AND count==0 → empty ✓
  head==tail AND count==8 → full  ✓
```

### data never cleared
old bytes stay in array after pop — tail just moves forward.
only head and tail define what is valid.
reading past tail is undefined — stale data from previous cycles.
no memset needed on pop — saves cycles on MCU.

### embedded use case — UART RX
```c
/* ISR pushes received byte — runs in interrupt context */
void UART_IRQHandler(void) {
    cbuf_push(&rx_buf, UART->DR);   /* hardware fills buffer */
}

/* main loop pops and processes */
while (!cbuf_is_empty(&rx_buf)) {
    cbuf_pop(&rx_buf, &byte);
    process(byte);
}
```
ISR and main loop share buffer — volatile and atomic access needed in production.
covered in module 5 with threads.

## m3-ex06 — state machine

### three layers

```
layer 1 — data (what exists):
  state_t enum    → all possible states, values 0,1,2...
  event_t enum    → all possible triggers, values 0,1,2...
  sm_ctx_t struct → runtime state: current + application data

layer 2 — rules (what can happen):
  transition_t         → one rule: {next_state, action}
  transitions[S][E]    → complete rulebook, every combination

layer 3 — engine (makes it run):
  sm_handle_event()    → lookup + call action + update state
```

engine never changes. rules change per use case. data holds current reality.

---

### enum vs #define

```c
/* #define — no type, no checking */
#define EVENT_START 0
/* compiler cannot catch: sm_handle_event(&ctx, STATE_IDLE) — wrong type, no warning */

/* enum — typed, checked */
typedef enum { EVENT_START=0, ... } event_t;
/* compiler warns if state_t passed where event_t expected */
```

enum value IS the array index — elegant connection:
- `EVENT_START=0` → `transitions[state][0]` → correct table column
- `STATE_IDLE=0`  → `transitions[0][event]` → correct table row

`STATE_COUNT`/`EVENT_COUNT` trick: add state before COUNT → table size updates automatically.

---

### ctx vs transition table

```c
/* ctx — per instance, runtime only */
typedef struct {
    state_t  current;       /* where machine is right now */
    uint32_t sensor_value;  /* application data */
    uint32_t error_count;   /* application data */
} sm_ctx_t;

/* table — static, shared, never changes */
static const transition_t transitions[STATE_COUNT][EVENT_COUNT] = { ... };
```

two sensors → two `sm_ctx_t` instances → same table.
ctx is the state, table is the rules.

---

### function pointer as dispatch mechanism

```c
typedef void (*action_fn)(sm_ctx_t *ctx);  /* consistent interface for all actions */

/* dispatch — one line handles all transitions */
t->action(ctx);   /* calls whichever function is stored — no if/else needed */
```

function pointer IS the dispatch — table stores which function to call.
same concept: Redux reducer, Linux VFS `file_operations`, HTTP route handler.

---

### why action_ignore not NULL

```c
/* NULL = address 0x00000000 */
t->action(ctx);
/* STM32: address 0 = vector table → hard fault */
/* Linux: segfault */

/* action_ignore = safe, logs, returns */
void action_ignore(sm_ctx_t *ctx) {
    (void)ctx;
    printf("event ignored\n");
}
```

every table cell populated → no unhandled transition possible.
`action_ignore` makes ignored events visible in debug logs.

---

### switch vs transition table

```
switch:
  pros: explicit, compiler catches missing cases (-Wswitch)
        familiar, debugger steps through naturally
  cons: N states × M events = huge nested switch
        adding state requires touching multiple places
        logic scattered, copy-paste errors common

table:
  pros: entire machine visible in one place
        add state/event = add one row/column
        O(1) lookup, scales to large machines
  cons: function pointer harder to debug
        all cells must be defined explicitly

rule: 3-4 states → switch fine, 5+ states → table wins
```

---

### how states and events are designed

states and events come from system design — not the MCU:

```
step 1: describe system in plain English
step 2: identify distinct situations → states
step 3: identify what causes change  → events
step 4: fill table — every state+event combination
step 5: write action functions — real or stub
```

why each next_state in sensor machine:

```
IDLE + START → SENSING              natural first step
SENSING + DATA_READY → PROCESSING   hardware signaled ready
PROCESSING + DONE → IDLE            work complete, return to wait
ANY + ERROR → ERROR                 error can happen anytime, safe state
ANY + RESET → IDLE                  always return to known good state
SENSING + START → SENSING           already sensing, ignore duplicate
```

---

### state_names and event_names

```c
static const char *state_names[] = {"IDLE", "SENSING", "PROCESSING", "ERROR"};
```

enum value = array index → `state_names[STATE_IDLE]` = `"IDLE"` automatically.
debug utility only — strip in production:

```c
#ifdef DEBUG
    printf("[%s]\n", state_names[ctx->current]);
#endif
```

saves flash memory on MCU — strings are expensive.

---

### dispatch — broad concept

input → lookup → execute — appears everywhere:

```
C state machine:  transitions[state][event] → function pointer
Redux/React:      dispatch(action) → reducer → new state
Linux VFS:        file->f_op->read() → driver's read function
HTTP router:      GET /sensors/1 → getSensor handler
```

what differs: lookup mechanism. concept identical.

## m4-ex01 — stack

### what it is
LIFO: last in first out — last pushed is first popped.
fixed array with top index tracking current position.
top = -1 means empty, top = SIZE-1 means full.

### why top starts at -1
0 is a valid array index — cannot use it to mean empty.
-1 is outside array bounds — unambiguous empty signal.
top == -1 → nothing in stack, any access would be invalid.

### push vs pop index order

```c
/* push — increment first, then write */
s->data[++s->top] = value;
/* top was 4 → becomes 5 → write to data[5] */

/* pop — read first, then decrement */
*value = s->data[s->top--];
/* read data[5] → top becomes 4 */
```

order matters — wrong order corrupts data or reads wrong slot.

### LIFO in embedded systems

1. CPU call stack — hardware stack, same structure
   every function call: CPU pushes return address + registers
   function return: CPU pops them back
   proved in m2-ex03 — each recursive call gets lower address

2. interrupt nesting — STM32 NVIC
   main running
   → UART interrupt fires (priority 2) → CPU pushes context, runs ISR
       → Timer interrupt fires (priority 1) → CPU pushes context, runs ISR
       → Timer ISR returns (function return — finished completely)
       → CPU pops Timer context → UART ISR resumes (continues from pause)
   → UART ISR returns
   → CPU pops UART context → main resumes
   last interrupted = first restored — LIFO exactly

3. undo/redo in configuration
   change setting → push to history stack
   undo → pop last change first — LIFO naturally implements undo

### return vs resume

```
return:
  function finishes its job completely
  caller gets result
  function is done — never continues

  void action_complete(sm_ctx_t *ctx) {
      printf("done\n");
      return;    ← function finished, gone
  }

resume:
  execution was interrupted mid-way
  CPU saved exact position (pushed context)
  after interrupt handled — continues from exact same point
  nothing was finished — just temporarily paused

  sensor_value = read_adc();   ← interrupted HERE
  /* interrupt fires → ISR runs → ISR returns */
  /* CPU restores context → resumes from this exact line */
  process(sensor_value);       ← continues here
```

ISR uses return — it finishes completely.
main uses resume — continues from where it was paused.

### push context — what CPU saves

```
program counter  → which instruction to return to
registers        → r0-r12 values at moment of interrupt
stack pointer    → where the stack was
status flags     → carry, zero, overflow flags
```

push context = save all of this to hardware stack.
pop context  = restore all of it — CPU continues as if nothing happened.
on STM32 Cortex-M: hardware does this automatically on interrupt entry/exit.

## m4-ex02 — queue

### what it is
FIFO: first in first out — first enqueued is first dequeued.
fixed array with head (read) and tail (write) indices, count for full/empty.
same mechanical structure as circular buffer — different naming convention.

### FIFO vs LIFO

```
stack (LIFO):
  last pushed = first popped
  use: call stack, interrupt nesting, undo history

queue (FIFO):
  first enqueued = first dequeued
  use: UART RX buffer, CAN messages, task scheduler,
       command parser, DMA transfer list
```

### head and tail roles

```
enqueue → writes to tail, advances tail
dequeue → reads from head, advances head

circular buffer used opposite names:
  push → modifies head (write)
  pop  → modifies tail (read)

mechanically identical — naming convention differs by author
what matters: write pointer advances on write, read pointer advances on read
```

### why queue_print uses (head+i)%SIZE

```c
/* wrong — assumes valid data starts at index 0 */
q->data[i]

/* correct — walks from actual head position */
q->data[(q->head + i) % QUEUE_SIZE]
```

after dequeuing 3 items head=3 — valid data starts at index 3 not 0.
walking from index 0 prints stale dequeued data.
modulo wraps correctly when head+i crosses QUEUE_SIZE boundary.

### circular buffer vs queue

```
circular buffer (m3-ex05):
  element size: uint8_t — byte oriented
  use case:     UART RX/TX, byte streams, ISR-safe data passing
  focus:        raw byte throughput

queue (m4-ex02):
  element size: uint32_t — word oriented
  use case:     task messages, command passing, event queues
  focus:        structured data passing between tasks

both:
  fixed array, two indices, count, modulo wrap
  head==tail ambiguity resolved by count
  O(1) enqueue and dequeue
```

in production code these terms are often used interchangeably.
what matters: implementation matches the use case element size and access pattern.

### embedded FIFO use cases

```
UART RX buffer:    bytes arrive in order → processed in order
CAN message queue: messages queued in arrival order → handled in order
FreeRTOS queue:    tasks at same priority → FIFO scheduling
command parser:    commands received → executed in submission order
DMA transfer list: transfers → processed in submission order
```
## m4-ex03 — circular buffer production quality

### ever-increasing head and tail

```c
/* learning version — modulo reset */
cb->head = (cb->head + 1) % BUFFER_SIZE;   /* resets to 0 at SIZE */

/* production version — ever-increasing */
cb->data[cb->head & CBUF_MASK] = byte;     /* AND extracts array index */
cb->head++;                                 /* never resets */
```

ever-increasing advantages:
- no modulo division — single AND instruction
- count math works correctly through uint32_t overflow
- head always logically >= tail — no ambiguity

requires: CBUF_SIZE must be power of 2 for bitwise AND to work correctly.

### bitwise AND vs modulo

```
CBUF_SIZE = 16, CBUF_MASK = 15 = 0x0F

modulo:  head % 16     — division instruction, slow on no-divider MCUs
AND:     head & 0x0F   — single instruction, always fast

head=17: 17 & 0x0F = 1  — same as 17 % 16 = 1
head=32: 32 & 0x0F = 0  — same as 32 % 16 = 0
```

only valid when size is exact power of 2: 4, 8, 16, 32, 64...

### count through overflow

```c
uint32_t cbuf_count(cbuf_t *cb)
{
    return (cb->head - cb->tail) & CBUF_MASK;
}
```

normal case:
  head=21, tail=10
  (21 - 10) & 0x0F = 11 & 0x0F = 11 ✓

overflow case (uint32_t wraps at 2^32):
  tail=0xFFFFFFF8, head=0x00000003
  logical items: 8 before overflow + 3 after = 11

  unsigned subtraction wraps correctly:
  0x00000003 - 0xFFFFFFF8 = 0x0000000B = 11
  11 & 0x0F = 11 ✓

why it works:
  unsigned subtraction always gives distance between two values
  wraparound is defined behavior in C for unsigned types
  & CBUF_MASK extracts only the meaningful lower bits
  same principle as clock arithmetic — 2 minus 10 = 4 steps apart, not -8

### volatile on head/tail not data

```
head: written by ISR (producer), read by main (consumer)
      two execution contexts share this variable
      without volatile: compiler caches in CPU register
      ISR updates memory — main reads stale cached value — silent failure
      volatile: forces re-read from memory every access

tail: written by main (consumer), read by ISR (producer)
      same problem in reverse — volatile required

data[]: each byte written once by producer, read once by consumer
        never accessed simultaneously by two contexts
        no stale cache problem — volatile not needed
```

ISR scenario:
```c
void UART_IRQHandler(void) {
    cb->data[cb->head & CBUF_MASK] = UART->DR;
    cb->head++;   /* volatile — main sees this immediately */
}

while (1) {
    if (!cbuf_is_empty(&cb)) {   /* reads head — must be fresh */
        cbuf_pop(&cb, &byte);    /* reads tail — must be fresh */
        process(byte);
    }
}
```
without volatile on head — main never sees ISR increment — buffer appears always empty.

### flush resets indices only

```c
void cbuf_flush(cbuf_t *cb)
{
    cb->head = 0;
    cb->tail = 0;
    /* data[] untouched — stale bytes remain until overwritten */
}
```

head == tail signals empty — producer and consumer see empty buffer.
stale bytes invisible — head/tail define valid region.
add memset only when zeroing sensitive data: passwords, keys, private data.

### why uint32_t for head and tail

wrap-around subtraction with uint8_t is mathematically safe if stored back correctly:
  uint8_t count = (uint8_t)(head - tail)   — defined, works
  but uint32_t is chosen for two real reasons:

reason 1 — buffer size capacity (primary):
  uint8_t indices  → max 255 buffer slots
  uint16_t indices → max 65535 buffer slots
  uint32_t indices → ~4 billion buffer slots
  head and tail values directly limit usable buffer size

reason 2 — expression safety:
  uint8_t and uint16_t get promoted to signed int in expressions
  requires careful store-back to uint8_t to stay correct
  uint32_t: already register width on 32-bit MCU — no promotion
  stays unsigned natively — no truncation concern anywhere

Linux kernel kfifo confirms this:
  struct __kfifo uses unsigned int (32-bit) for in and out
  matches CPU register width, supports large buffers,
  no promotion concern in any expression context

### production vs learning version comparison

```
m3-ex05 (learning):
  head/tail reset with modulo
  count member in struct
  no volatile
  purpose: understand the concept

m4-ex03 (production):
  head/tail ever-increasing, AND for array access
  count calculated from head-tail, no struct member needed
  volatile on head/tail for ISR safety
  bulk push/pop for efficiency
  flush without memset
  uint32_t indices for size and safety
  purpose: use in real firmware
```

## m4-ex04 — binary search tree

### BST property
every node satisfies:
  all values in left subtree  < node value
  all values in right subtree > node value
  no duplicates (in this implementation)

this property is established at insert time and maintained always.
inorder traversal exploits it — left→node→right = sorted output guaranteed.

### insert — why it returns node*

```c
bst_node_t *bst_insert(bst_node_t *root, uint32_t value)
{
    if (root == NULL) return bst_create_node(value);  /* base case */
    if (value < root->value)
        root->left = bst_insert(root->left, value);   /* assign returned ptr */
    else if (value > root->value)
        root->right = bst_insert(root->right, value); /* assign returned ptr */
    return root;
}
```

recursive call chain for inserting 20 into tree rooted at 50:
  bst_insert(50, 20) → 20<50, go left
    bst_insert(30, 20) → 20<30, go left
      bst_insert(NULL, 20) → create node(20), return it
    30->left = node(20), return node(30)
  50->left = node(30), return node(50)
root = node(50)

each level receives returned pointer and assigns it — tree gets connected
without needing **root parameter.

### inorder traversal — why sorted

```c
void bst_inorder(bst_node_t *root)
{
    if (root == NULL) return;
    bst_inorder(root->left);    /* visit all smaller values first */
    printf("%u ", root->value); /* visit current */
    bst_inorder(root->right);   /* visit all larger values after */
}
```

BST property guarantees left < current < right at every node.
inorder visits left subtree (all smaller) → current → right subtree (all larger).
result: values printed in ascending sorted order always.

inserted: 50 30 70 20 40 60 80
inorder output: 20 30 40 50 60 70 80  ← sorted ✓

### bst_free — why free children before root

```c
void bst_free(bst_node_t *root)
{
    if (root == NULL) return;
    bst_free(root->left);    /* free entire left subtree */
    bst_free(root->right);   /* free entire right subtree */
    free(root);              /* free current node last */
}
```

same rule as nested malloc — free inner before outer.
freeing root first loses the only pointers to children — memory leak.

every node gets freed — recursion reaches every node in the tree.
as recursion unwinds each node is freed individually.

### why only root = NULL in main

```c
bst_free(root);
root = NULL;    /* only this pointer still in scope — NULL it */
```

after bst_free returns — every node freed.
internal node pointers (left, right) now dangling — point to freed memory.
but they live inside freed nodes — nobody can reach them.
only live accessible pointers need NULLing after free.
root in main is the only pointer still in scope — the only one to NULL.

### BST vs linked list physically

```
linked list node:
  value + one next pointer → linear chain

BST node:
  value + left pointer + right pointer → branching tree

both: scattered heap memory connected by pointers
BST: struct needs name for self-reference — same reason as linked list
```

### search complexity

```
balanced BST: O(log n) — halves search space at each node
unbalanced BST: O(n) — degenerates to linked list
  insert sorted values: 10→20→30→40 → all go right → linear chain

balanced example (our values: 50 30 70 20 40 60 80):
  search 40: 40<50→left, 40>30→right, found — 3 comparisons for 7 nodes
  height = 3 = log2(7) ≈ 3 ✓
```

### embedded use cases

```
device registry:    look up device by ID — O(log n) faster than linear scan
error code table:   binary search through sorted error codes
sensor calibration: sorted lookup table for ADC-to-value mapping
priority queues:    ordered task scheduling (simpler than heap)
```

BST less common in embedded than arrays/circular buffers — requires dynamic memory.
used when: dataset changes at runtime and sorted access needed.
avoid when: memory is tight or real-time determinism required.

## m4-ex05 — state machine advanced (UART frame parser)

### why action takes uint8_t data parameter

m3-ex06 actions had no data — they were pure state triggers:
  void (*action_fn)(sm_ctx_t *ctx)

m4-ex05 actions carry a byte value — same event, different data each time:
  void (*action_fn)(sm_ctx_t *ctx, uint8_t data)

EVENT_BYTE_RECEIVED fires once per byte — each call carries a different byte.
without data parameter: action cannot store the actual received value.
the event says WHAT happened, data says WHAT VALUE came with it.

### XOR checksum — why it works

```
three properties that make XOR useful for checksums:

1. self-inverse:
   A XOR A = 0    anything XORed with itself = 0
   A XOR 0 = A    anything XORed with 0 = itself

2. commutative and associative:
   order doesn't matter — same result regardless of byte order

3. single-bit error detection:
   flip any one bit → checksum changes → mismatch detected — guaranteed
```

how checksum computed across frame:
```
bytes: 0x01, 0x02, 0x03
checksum = 0x01 XOR 0x02 XOR 0x03
  0x01 = 00000001
  0x02 = 00000010  XOR → 00000011
  0x03 = 00000011  XOR → 00000000 = 0x00

receiver computes same XOR → gets 0x00 → match → valid
any bit flipped in transmission → non-zero result → mismatch detected
```

limitation: misses even number of flipped bits in same position.
for stronger detection: CRC (cyclic redundancy check) — used in UART, CAN, Ethernet.
XOR is lightweight — good for simple embedded protocols with low noise.

### m3-ex06 vs m4-ex05 — same pattern, richer context

```
same:
  transition table [STATE][EVENT] → {next_state, action}
  function pointer dispatch: t->action(ctx, data)
  action_ignore for undefined transitions
  state_names/event_names debug arrays
  sm_handle_event engine — identical structure

genuinely different:
  action signature includes uint8_t data
  context holds frame_t buffer — accumulates bytes across events
  context holds statistics: rx_count, error_count, processed_count
  real protocol: UART frame parsing with checksum validation
  state persists meaningful data between events — not just current state
```

the pattern is identical — richer context and real use case make it advanced.
m3-ex06 could use m4-ex05's engine unchanged — same dispatch mechanism.

### UART frame parser state flow

```
IDLE:
  BYTE_RECEIVED → RECEIVING   first byte starts a frame

RECEIVING:
  BYTE_RECEIVED → RECEIVING   accumulate bytes into frame buffer
  FRAME_COMPLETE → VALIDATING end-of-frame marker received, validate
  TIMEOUT → ERROR             no byte received within timeout window

VALIDATING:
  CHECKSUM_OK → PROCESSING    checksum matches — process frame data
  CHECKSUM_FAIL → ERROR       checksum mismatch — discard frame

PROCESSING:
  PROCESS_DONE → IDLE         processing complete — ready for next frame
  TIMEOUT → ERROR             processing took too long

ERROR:
  RESET → IDLE                only RESET escapes error state
  everything else ignored     stay in error until explicitly cleared
```

### embedded connection — real UART ISR

```c
/* real embedded — ISR fires on each received byte */
void UART1_IRQHandler(void) {
    uint8_t byte = UART1->DR;             /* read hardware register */
    sm_handle_event(&ctx, EVENT_BYTE_RECEIVED, byte);  /* dispatch */
}

/* timeout — timer ISR fires if no byte received */
void TIM2_IRQHandler(void) {
    sm_handle_event(&ctx, EVENT_TIMEOUT, 0x00);
}
```

state machine decouples protocol logic from hardware — ISR just fires events.
action functions handle protocol behavior — clean separation.

## m5-ex01 — file descriptors

### what a file descriptor is

file descriptor = integer index into kernel's per-process file table.
open() does not return the file — it returns a key to look up the file.

```
kernel table (per process):
  fd 0 → stdin  (keyboard)
  fd 1 → stdout (terminal)
  fd 2 → stderr (terminal)
  fd 3 → your opened file  ← next available slot
```

the integer itself is meaningless — what matters is what the kernel maps it to.
same concept as array index — index points to the real data.

### open/read/write/close vs fopen/fread/fwrite/fclose

```
C library (FILE*):         Linux syscalls (fd):
fopen()     →              open()
fread()     →              read()
fwrite()    →              write()
fclose()    →              close()
```

FILE* adds buffering on top of raw fd:
  fwrite collects small writes → flushes to write() in larger chunks
  raw write(fd) → goes straight to kernel, no buffering

printf chain:
  printf() → fprintf(stdout) → fwrite → write(STDOUT_FILENO, buf, len)

scanf chain:
  scanf() → fscanf(stdin) → fread → read(STDIN_FILENO, buf, len)

stdout and stdin are FILE* wrappers around fd 1 and fd 0.

### ssize_t — why signed for byte counts

```c
ssize_t written = write(fd, msg, strlen(msg));
ssize_t bytes_read = read(fd, buf, BUF_SIZE);
```

size_t is unsigned — cannot represent -1.
ssize_t is signed — allows error return:
  >= 0  → bytes successfully written or read
  -1    → error occurred

on error: check errno global variable for specific reason:
  ENOENT → file not found
  EACCES → permission denied
  EBADF  → bad file descriptor

### open() flags and permissions

```c
int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

flags:
  O_RDONLY  → read only
  O_WRONLY  → write only
  O_RDWR    → read and write
  O_CREAT   → create file if it doesn't exist
  O_TRUNC   → truncate existing file to zero length
  O_APPEND  → append to end of file

permissions (0644 — octal):
  6 = rw-  → owner: read + write
  4 = r--  → group: read only
  4 = r--  → others: read only

### lseek — repositioning within file

```c
lseek(fd, 0, SEEK_SET);   /* go to beginning */
lseek(fd, 0, SEEK_CUR);   /* get current position */
lseek(fd, 0, SEEK_END);   /* go to end */
```

useful for: random access files, re-reading from beginning,
            finding file size (lseek to SEEK_END returns file size).

### embedded connection

Linux embedded targets use fd-based interface for hardware:
  /dev/ttyS0   → UART as file descriptor
  /dev/spidev  → SPI as file descriptor
  /dev/i2c-1   → I2C as file descriptor

read/write to these fds = read/write to hardware.
ioctl(fd, command, arg) = configure hardware — covered in ex05.
same open/read/write/close pattern for files and devices.
```

## m5-ex02 — /proc interface

### what /proc is — virtual filesystem

real filesystem: data stored on physical media (SSD, flash, HDD)
  bytes exist on storage — kernel fetches them on read

/proc virtual filesystem: exists only in kernel memory — nothing on disk
  open("/proc/uptime") → kernel calculates uptime from internal timer
                       → generates text on the fly
                       → returns as if it were a file
                       → nothing stored anywhere

same open/read/write interface as real files.
Unix philosophy: everything is a file — hardware, processes, kernel state
all accessible through the same file descriptor API.

### key /proc files for embedded Linux

```
/proc/uptime          → system uptime and idle time
/proc/meminfo         → live memory: total, free, available, cached
/proc/cpuinfo         → CPU model, cores, clock speed, features
/proc/self/status     → current process: name, state, PID, memory
/proc/self/maps       → current process memory map — debug without gdb
/proc/self/fd         → open file descriptors of current process
/proc/PID/status      → any process by PID — monitoring tool use case
```

no special API needed — just open/read like any file.

### /proc/self — how Linux thinks about processes

```
/proc/self → symlink → /proc/7168  (resolved to caller's PID)
```

Linux treats every process as a resource with an ID.
kernel doesn't distinguish "current process" from "any other process."
/proc/self: kernel resolves to caller's PID automatically.
two processes reading /proc/self/status simultaneously → each gets their own info.

embedded use: monitoring tool reads /proc/self/status at runtime
  checks own memory usage, fd count, state — no external tools needed.

### raw fd vs FILE* — when to choose each

```
raw fd (open/read/write):
  no buffering — every call goes straight to kernel
  correct for: /proc, /sys, device files, binary data
  correct for: exact syscall timing control
  correct for: bare metal or restricted environments (no libc)

FILE* (fopen/fgets/fprintf):
  buffered — fewer syscalls, better performance for text
  formatted I/O — fgets, fscanf, fprintf handle parsing
  correct for: text files, line-by-line reading, formatted output
  correct for: when convenience matters more than exact control

embedded bare metal: no C library available
  only raw syscalls — open/read/write — no FILE* option
  know both, choose correctly per context
```

### embedded Linux connection

/proc makes kernel internals visible without special tools:

```c
/* check memory before large allocation */
FILE *fp = fopen("/proc/meminfo", "r");
/* parse MemFree — decide if safe to proceed */

/* monitor another process */
char path[64];
snprintf(path, sizeof(path), "/proc/%d/status", target_pid);
FILE *fp = fopen(path, "r");
/* read state, memory usage of target process */
```

same pattern used in system monitoring tools, watchdog daemons,
resource managers — all in embedded Linux production firmware.

## m5-ex03 — /sys interface

### /proc vs /sys — practical difference

```
/proc → process and kernel runtime information (read-mostly)
  /proc/uptime        system uptime
  /proc/meminfo       memory usage
  /proc/cpuinfo       CPU details
  /proc/PID/status    process state
  tells you WHAT IS HAPPENING

/sys → hardware device configuration and control (read AND write)
  /sys/class/thermal  CPU/GPU temperatures
  /sys/class/net      network interface stats and config
  /sys/class/gpio     GPIO control (Raspberry Pi, BeagleBone)
  /sys/block          block device info
  lets you CONTROL HARDWARE
```

key difference: /proc is informational, /sys is the control interface.

### writing to /sys changes hardware

```
read /sys/class/net/eth0/statistics/rx_bytes → get received byte count
write /sys/class/leds/led0/brightness → change LED brightness
write /sys/class/gpio/gpio17/value → set GPIO pin high or low
write /sys/class/thermal/cooling_device0/cur_state → change fan speed
```

write syscall path:
  userspace write() → kernel sysfs infrastructure
  → routes to driver's store() function
  → driver executes hardware change

read syscall path:
  userspace read() → kernel sysfs infrastructure
  → routes to driver's show() function
  → driver reads hardware state → returns as text

### why millidegrees not float

kernel space uses integers only — no floating point in kernel code.
millidegrees = integer representation with 3 decimal places:
  25350 = 25.350°C
  no float needed in kernel or driver
  userspace divides by 1000 for display — one integer division

consistent across all architectures regardless of FPU presence.
same pattern: voltage in microvolts, frequency in Hz, time in nanoseconds.
always integers in /sys — always scale factor — always divide in userspace.

### /sys on real embedded Linux targets

```
Raspberry Pi:
  /sys/class/thermal/thermal_zone0/temp  → CPU temperature
  /sys/class/gpio/gpio17/value           → GPIO control
  /sys/class/leds/led0/brightness        → onboard LED

BeagleBone:
  /sys/class/pwm/pwmchip0/pwm0/duty_cycle → PWM control
  /sys/bus/iio/devices/iio:device0/in_voltage0_raw → ADC reading

i.MX6 (COM Express/SMARC — your Advantech work):
  /sys/class/thermal → multiple thermal zones
  /sys/class/net → Ethernet interface control
  /sys/bus/spi → SPI device configuration
```

WSL2 limitation: virtual machine — no direct hardware access.
/sys/class/thermal not exposed — no real CPU temperature sensors.
test on real embedded Linux target for full /sys capability.

### snprintf buffer sizing

```c
/* warning: potential truncation */
char path[256];
snprintf(path, 256, "/sys/class/net/%s/statistics/rx_bytes", entry->d_name);
/* entry->d_name max = 255 bytes (NAME_MAX) */
/* format string = 36 chars + 255 = 291 bytes > 256 buffer */

/* fix: larger buffer */
char path[512];
snprintf(path, 512, "/sys/class/net/%s/statistics/rx_bytes", entry->d_name);
```

compiler -Wformat-truncation catches this at compile time.
always size path buffers for worst-case string length.
```
