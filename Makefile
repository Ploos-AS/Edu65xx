CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2

SIM := build/edu65xx
SIM_TEST := build/test_cpu

.PHONY: all test clean

all: $(SIM) test

build:
	mkdir -p build

$(SIM): simulator/cpu.c simulator/cpu.h simulator/main.c | build
	$(CC) $(CFLAGS) -Isimulator simulator/cpu.c simulator/main.c -o $(SIM)

$(SIM_TEST): simulator/cpu.c simulator/cpu.h simulator/test_cpu.c | build
	$(CC) $(CFLAGS) -Isimulator simulator/cpu.c simulator/test_cpu.c -o $(SIM_TEST)

test: $(SIM_TEST)
	./$(SIM_TEST)

clean:
	rm -rf build
