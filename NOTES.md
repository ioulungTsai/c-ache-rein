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
