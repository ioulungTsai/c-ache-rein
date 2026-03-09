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
