CXX := g++
BUILD_DIR := build
TARGET := $(BUILD_DIR)/mandelbrot_1.exe
SRC := mandelbrot_1.c
TXLIB_DIR := D:\Libraries\TXLib
FLAGS := -I$(TXLIB_DIR) -D_DEBUG -ggdb3 -std=c++17 -O3 -Wall -Wextra -Wshadow
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(FLAGS)

clean:
	rm -rf $(BUILD_DIR)
