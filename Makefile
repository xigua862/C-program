RAYLIB_DIR = vendor/raylib

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -I$(RAYLIB_DIR)/include
LDFLAGS = -L$(RAYLIB_DIR)/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = $(wildcard src/*.cpp)
BIN = game.exe

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(SRC) -o $(BIN) $(CXXFLAGS) $(LDFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: all run clean
