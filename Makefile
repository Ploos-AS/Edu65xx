CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2

SIM := build/edu65xx
SIM_TEST := build/test_cpu
EMU_TEST := build/test_machine
QUAL_TEST := build/klaus65c02
KLAUS_BIN ?= build/65C02_extended_opcodes_test.bin
SIM_SRC := simulator/cpu.c simulator/via.c simulator/serial.c

.PHONY: all test qualification clean

all: $(SIM) test

build:
	mkdir -p build

$(SIM): $(SIM_SRC) simulator/cpu.h simulator/via.h simulator/serial.h simulator/main.c | build
	$(CC) $(CFLAGS) -Isimulator $(SIM_SRC) simulator/main.c -o $(SIM)

$(SIM_TEST): $(SIM_SRC) simulator/cpu.h simulator/via.h simulator/test_cpu.c | build
	$(CC) $(CFLAGS) -Isimulator $(SIM_SRC) simulator/test_cpu.c -o $(SIM_TEST)

$(EMU_TEST): $(SIM_SRC) emulator/machine.c emulator/machine.h emulator/monitor_rom.c emulator/monitor_rom.h emulator/test_machine.c | build
	$(CC) $(CFLAGS) -Isimulator -Iemulator $(SIM_SRC) emulator/machine.c emulator/monitor_rom.c emulator/test_machine.c -o $(EMU_TEST)

$(QUAL_TEST): $(SIM_SRC) simulator/cpu.h qualification/klaus65c02.c | build
	$(CC) $(CFLAGS) -Isimulator $(SIM_SRC) qualification/klaus65c02.c -o $(QUAL_TEST)

qualification: $(QUAL_TEST)
	test -s "$(KLAUS_BIN)"
	./$(QUAL_TEST) "$(KLAUS_BIN)"

test: $(SIM_TEST) $(EMU_TEST)
	./$(SIM_TEST)
	./$(EMU_TEST)

clean:
	rm -rf build
