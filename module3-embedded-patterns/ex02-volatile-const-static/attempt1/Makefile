CC = gcc
CFLAG = -Wall -Wextra -g
TARGET = main

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAG) -o $(TARGET) main.c

asm: main.c
	$(CC) -O2 -S -o main.s main.c
	@echo ""
	@echo "=== poll_without_volatile ==="
	@sed -n '/^poll_without_volatile:/,/\.size.*poll_without_volatile/p' main.s
	@echo ""
	@echo "=== poll_with_volatile ==="
	@sed -n '/^poll_with_volatile:/,/\.size.*poll_with_volatile/p' main.s

clean:
	rm -f $(TARGET) main.s
