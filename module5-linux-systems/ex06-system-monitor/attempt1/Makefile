CC = gcc
CFLAG = -Wall -Wextra -g
LIBS = -lpthread
TARGET = main

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAG) -o $(TARGET) main.c $(LIBS)

clean:
	rm -f $(TARGET)
