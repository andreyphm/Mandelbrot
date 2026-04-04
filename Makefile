CXX := g++
BUILD_DIR := build
TARGET := $(BUILD_DIR)/mandelbrot.exe
SRC := mandelbrot.c
TXLIB_DIR := D:\Libraries\TXLib
FLAGS := -I$(TXLIB_DIR) -D_DEBUG -ggdb3 -std=c++17 -O3 -Wall -Wextra -Wshadow -mavx
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(FLAGS)

clean:
	rm -rf $(BUILD_DIR)
