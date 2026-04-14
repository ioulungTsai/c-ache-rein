# Notes

## m1-ex01-variables-attempt1 - 2026-03-06
- recipe in a Makefile always specify existing files 
- what is f in the 36.6f?

## m1-ex01-variables-attempt2 - 2026-03-07
- what is %02X? > Please see the LESSONS.md
- the compiled executable in the Linux won't have extensions
- cannot catch in .gitignore without specify the file name precisely, for example in this exercise: module*/*/attempt*/main
- above approach may not be a best practice but works for this project, perhaps revisit this later
- if the file already staged then "git rm --cached (file needs to unstage)" would be required

## m1-ex02-control-flow-attempt1 - 2026-03-08
- in Makefile, even [target: prerequisites] works but [target : prerequisites] shown in the [GNU make](https://www.gnu.org/software/make/manual/make.html#Rule-Introduction)

## m1-ex01-variables-attempt3 - 2026-03-09
- sizeof() returns size_t
- always use %zu for size_t — portable everywhere
- %lu only works by coincidence on 64-bit Linux 

## m1-ec03-functions-attempt1 - 2026-03-09
- get_bit takes uint8_t (copy) — original unchanged
- set_bit takes uint8_t* (address) — modifies original directly
- * in function parameter = "I need the real address, not a copy"
- same concept as writing to a hardware register address

## m1-ex04-arrays-and-strings-attempt1 - 2026-03-11
- reverse_array = endianness byte swap — same concept
- strings in C are just char arrays ending with '\0'
- my_strcpy must manually add '\0' — nothing does it for you
- array name passed to function = pointer to first element

## m1-ex05-struct-attempt1 - 2026-03-12
- What does the compiler consider as using a variable?

## m1-ex05-struct-attempt2 - 2026-03-12
- GCC warning options full list:
  https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
- difference between -Wall and -Wextra — what does each enable?
- can warnings be treated as errors? (-Werror flag)

## m1-ex06-file-io-attempt1 - 2026-03-13
- is '_t' in the sensor_log_t form a keyword in C? (if with '_type' no highlight in code)

  > No, _t is not a C keyword, but it is a naming convention reserved by the POSIX standard for system-defined types, which is why VS Code recognizes it by default for syntax highlighting. It can be customized to make '_type' highlighted. - 2026-03-14

- don't have file io core concepts in C to fully understnd the code
  >check below in LESSONS.md
- what is the rule of thumb using const?
- why return 1, -1?
- what are %-3u, %-6.1f?
- why use %hhu, and only %f in fscanf()?
  >check above in LESSONS.md

- why .log is excluded by git even .gitignore don't mention it?

  >I missed that the .log file was included. I'll untrack it and add it to .gitignore.
  
  ```git rm --cached [file needs to be untracked]```

## m2-ex01-pointer-basics-attempt1 - 2026-03-15
- what is the purpose of the casting (void *)?
- uint8_t *pa and uint8_t* pa identical — star belongs to variable not type
- never uint8_t* pa, pb — only pa would be pointer, pb would be uint8_t
- (void*) cast for %p — printf %p requires void* exactly
- void* = generic pointer, no type, accepts any address

## m2-ex03-malloc-free-attempt1 - 2026-03-17
- always free(ptr) then ptr = NULL — two lines, never one without the other
- malloc returns NULL on failure — always check before using
- stack grows down at function call level — proven with recursion
- heap grows up — proven with sequential malloc addresses
- within one stack frame compiler arranges variables freely — not reliable for direction proof
- dangling pointer = pointer still holds address after free — silent corruption risk

## m2-ex04-string-functions-attempt1 - 2026-03-17
- my_memcpy uses void* to accept any pointer type — casts to uint8_t* inside to walk byte by byte
- my_strcmp casts to unsigned char — char is signed, values >127 give wrong comparison without cast
- my_strcmp subtracts ASCII values of first differing characters
- result sign matters, not exact value: 0=equal, <0=s1 before s2, >0=s1 after s2
- ASCII '1'=49, '2'=50 — that's why DHT11 vs DHT22 gives exactly -1
- my_strcpy saves start pointer before arithmetic — returns original head not moved pointer
- void* = generic pointer, accepts any type, no arithmetic allowed until cast to concrete type

## m2-ex05-structs-pointers-attempt1 - 2026-03-22
- register_t conflicts with system header typedef in sys/types.h
- system headers reserve certain _t names — always check if name compiles
- renamed to reg_t to avoid conflict

## m2-ex05-structs-pointers-attempt1 - 2026-03-23
- register_t conflicts with sys/types.h — renamed to reg_t
- free inner allocations before outer — reverse order of allocation
- cleanup on partial failure: free completed allocations before returning NULL
- sensor_t** needed when function must modify the pointer itself not what it points to

## m2-ex06-linked-list-attempt1 - 2026-03-25
- struct needs name for self-reference — node_t not defined yet during typedef
- &curr prints stack address of local variable, curr prints heap node address
- push_front: new->next = *head then *head = new — two steps, order matters
- list_free: save next before free — reading freed memory is undefined behavior

## m3-ex01-bitwise-macros-attempt1 - 2026-03-25
- always wrap macro params in () — protects against operator precedence at expansion
- 1U not 1 — unsigned avoids undefined behavior when shifting into sign bit
- CLEAR_BIT: shift 1 to position → ~ flips all bits → & preserves others, clears target
- 1U/1UL/1ULL/f/L — literal suffixes specify type, same family as 36.6f
- + has higher precedence than << — missing () in macros breaks when result used in larger expression

## m3-ex02-volatile-const-static-attempt1 - 2026-03-26
- #define: no type, no memory, preprocessor text substitution
- const: typed, has memory address, compiler enforced read-only
- static local: initialized once, persists between calls, lives in data segment not stack
- static global: restricts visibility to current file only — hides from other files
- extern exposes to other files, static hides — opposite directions
- without volatile + assignment inside loop → compiler deletes loop entirely → ret only
- with volatile + assignment inside loop → real loop, movl re-reads memory every iteration
- printf inside loop blocks optimization — use isolated functions for assembly proof
- empty loop body without volatile → infinite tight jump (jmp .L8), not deleted
- assignment inside loop without volatile → calculated at compile time → deleted (ret)
- make asm target: critical proof — compile -O2, grep isolated functions
- runtime proof requires real hardware or threads — assembly is correct proof on Linux

## m3-ex03-mmio-simulation-attempt1 - 2026-03-27
- volatile in REG_READ/WRITE — forces real memory access every time, no caching
- uintptr_t in macro arithmetic — pointer arithmetic steps by sizeof type, not bytes
- (uintptr_t)base + 0x04 = correct byte offset, base + 0x04 = wrong (steps by 4 bytes)
- uintptr_t portable — matches pointer size on any platform (4 bytes 32-bit, 8 bytes 64-bit)
- function parameter uses volatile uint32_t* — type safety at call site
- macro body casts to uintptr_t — needs raw byte arithmetic for correct offsets
- real STM32: fixed datasheet address, hardware resets values, hardware changes registers
- simulation: array address runtime-assigned, values set by code only

## m3-ex04-fixed-point-attempt1 - 2026-03-30
- fixed-point: integers with imaginary decimal point — no FPU needed at runtime
- Q8 format: FIXED_SHIFT=8, scale=256, precision=1/256=0.00390625
- FLOAT_TO_FIXED and FIXED_TO_FLOAT touch FPU — boundary only, not runtime math
- INT_TO_FIXED, ADD, SUB, MUL, DIV — pure integer, no FPU
- FIXED_MUL shifts right after multiply — removes extra scale factor introduced by multiplication
- FIXED_DIV shifts left before divide — preserves fractional bits before integer division discards them
- values smaller than 0.00390625 are lost — truncated to zero in Q8
- float vs fixed difference 0.006563 — expected Q8 precision loss, not a bug
- increase FIXED_SHIFT for finer precision: Q16 floor = 1/65536 = 0.0000153

## m3-ex05-circular-buffer-attempt1 - 2026-04-03
- head = write index (next empty slot), tail = read index (oldest data)
- modulo wraps head/tail within BUFFER_SIZE — prevents going past array end
- power of 2 size enables bitwise AND instead of modulo: (head+1) & (SIZE-1)
- count resolves head==tail ambiguity — could be full or empty without it
- data never cleared — old bytes stay in array, head/tail define valid region
- push rejected when full (count==SIZE), pop rejected when empty (count==0)
- FIFO order preserved — first pushed is first popped via tail chasing head

## m3-ex06-state-machine-attempt1 - 2026-04-06
- enum values start at 0, auto-increment — STATE_COUNT/EVENT_COUNT always equals total count
- enum value IS the array index — STATE_IDLE=0 maps to transitions[0], state_names[0]
- transition table transitions[STATE_COUNT][EVENT_COUNT] — 2D array, O(1) lookup
- ctx holds runtime state only — current state + application data, not the table
- table is static const — shared, never changes, one instance for all ctx
- action_fn is function pointer typedef — one consistent interface for all actions
- action_ignore safer than NULL — NULL dereference = hard fault on MCU
- state_names/event_names — debug utility only, strip with #ifdef DEBUG in production
- switch vs table: switch explicit+compiler checks, table scalable+visible in one place
- dispatch: input → lookup → execute — same pattern in Redux, Linux VFS, HTTP routing
- states come from system design not MCU — describe behavior in English first
- ignored events stay in current state — never leave transition cell undefined

## m4-ex01-stack-attempt1 - 2026-04-07
- top starts at -1 — 0 is valid index, -1 is unambiguous empty sentinel
- push uses ++top (pre-increment) — increment first, then write to new slot
- pop uses top-- (post-decrement) — read current top first, then decrement
- stack_peek reads top without removing — same bounds check as pop
- LIFO: last pushed = first popped — CPU call stack works identically
- stdlib.h not needed here — no malloc/free, fixed array only, can be removed
- interrupt nesting is LIFO — last interrupted context = first restored
- return = function finishes completely, resume = continues from paused point

## m4-ex02-queue-attempt1 - 2026-04-09
- queue is FIFO: first enqueued = first dequeued — opposite of stack LIFO
- enqueue writes to tail, dequeue reads from head — opposite naming from circ buffer
- circular buffer: push→head, pop→tail / queue: enqueue→tail, dequeue→head — same mechanism, different names
- queue_print uses (head+i)%SIZE not data[i] — valid data starts at head not index 0
- count resolves head==tail ambiguity — same as circular buffer
- modulo wrap: (tail+1)%SIZE — same pattern as circular buffer
- FIFO embedded uses: UART RX, CAN messages, task scheduler, command parser, DMA list
- mechanically identical to circular buffer — element size and use case differ

## m4-ex03-circular-buffer-production-attempt1 - 2026-04-12
- ever-increasing head/tail — never reset, overflow is intentional and safe
- array access uses head & CBUF_MASK (bitwise AND) — faster than modulo, no division
- CBUF_MASK = CBUF_SIZE - 1 — only valid when size is power of 2
- count = (head - tail) & CBUF_MASK — correct through uint32_t overflow
- volatile on head and tail — shared between ISR and main, must re-read each time
- data[] not volatile — each byte has one writer, one reader, no simultaneous access
- flush resets head/tail to 0 only — data stays, stale until overwritten
- memset data only when security requires zeroing sensitive data
- uint8_t for data — matches byte-oriented hardware: UART, SPI, I2C
- uint32_t for indices — no integer promotion, no truncation concern, supports large buffers
- uint8_t/uint16_t indices work mathematically BUT get promoted to signed int in expressions
- integer promotion is not the only reason — buffer size capacity is equally important

## m4-ex04-bst-attempt1 - 2026-04-13
- BST property: all left descendants < node < all right descendants
- insert returns node* — each level assigns returned pointer to connect tree
- inorder traversal (left→node→right) produces sorted output — BST property guarantees it
- bst_free frees left and right subtrees before root — reverse of allocation
- every node gets freed via recursion — not just root
- only root = NULL required in main — only accessible pointer after bst_free returns
- internal node pointers become dangling after free — in freed memory, nobody can reach them
- bst_height: 1 + max(left_height, right_height) — recursive depth calculation
- bst_find_min: always go left until left == NULL — leftmost node is minimum
- BST search O(log n) average, O(n) worst case (unbalanced tree)
- struct needs name (bst_node) for self-reference — same reason as linked list node
- free() returns memory to heap pool — fully reusable, not lost
- NULL after free prevents misuse of stale address — loud crash beats silent corruption
- stack variables including pointer variables released automatically when scope exits
- only NULL pointers still in scope after free — internal node pointers die with freed heap

## m4-ex05-state-machine-advanced-attempt1 - 2026-04-14
- action signature adds uint8_t data — same event carries different byte values each call
- XOR checksum: self-inverse (A^A=0), detects any single-bit error, lightweight
- XOR limitation: misses even number of flipped bits in same position — use CRC for stronger check
- context richer than m3-ex06: frame_t buffer + statistics tracked across events
- frame accumulates across multiple EVENT_BYTE_RECEIVED — state persists between events
- transition table pattern identical to m3-ex06 — same engine, richer context
- action_ignore still used — (void)ctx; (void)data to silence unused warnings
- checksum flow: start_frame initializes, store_byte XORs each byte, validate reports
- real protocol behavior: idle→receiving→validating→processing→idle
- timeout in RECEIVING → ERROR — hardware timeout maps to real UART timeout ISR
