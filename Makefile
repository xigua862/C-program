RAYLIB_DIR = vendor/raylib

CC      = gcc
CFLAGS  = -std=c99 -O2 -Wall -I$(RAYLIB_DIR)/include
LDFLAGS = -L$(RAYLIB_DIR)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = $(wildcard src/*.c)
BIN = game.exe

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) $(LDFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: all run clean
