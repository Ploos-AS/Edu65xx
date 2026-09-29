CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2

SIM := build/edu65xx
SIM_TEST := build/test_cpu
SIM_SRC := simulator/cpu.c simulator/via.c simulator/serial.c

.PHONY: all test clean

all: $(SIM) test

build:
	mkdir -p build

$(SIM): $(SIM_SRC) simulator/cpu.h simulator/via.h simulator/serial.h simulator/main.c | build
	$(CC) $(CFLAGS) -Isimulator $(SIM_SRC) simulator/main.c -o $(SIM)

$(SIM_TEST): $(SIM_SRC) simulator/cpu.h simulator/via.h simulator/test_cpu.c | build
	$(CC) $(CFLAGS) -Isimulator $(SIM_SRC) simulator/test_cpu.c -o $(SIM_TEST)

test: $(SIM_TEST)
	./$(SIM_TEST)

clean:
	rm -rf build
