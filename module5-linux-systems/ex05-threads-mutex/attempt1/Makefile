CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lpthread
TARGET = main

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c $(LIBS)

clean:
	rm -f $(TARGET)
