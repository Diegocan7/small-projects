## Compiler to use
CC = gcc

## Compiler flags
CFLAGS = -static -Wall -Wextra -Werror -pedantic -O3

## Linking flags
LDFLAGS = $(CFLAGS)

## The name of the binary fike
TARGET ?= goteam

## src folder
SRCS = $(wildcard src/*.c)

## Object files
OBJS = $(SRCS:.c=.o)

## Build the target by default
all: $(TARGET)

## Link
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

## Rules

run: $(TARGET)
	./$(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

