CC=gcc
CFLAGS=-Wall -Wextra -std=c99
TARGET=search_index
SRC=main.c file.c trie.c heap.c
OBJ=$(SRC:.c=.o)

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: build
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) indexare.out
.PHONY: build run clean
