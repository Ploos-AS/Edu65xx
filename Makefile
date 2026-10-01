CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2

SIM := build/edu65xx
SIM_TEST := build/test_cpu
EMU_TEST := build/test_machine
QUAL_TEST := build/klaus65c02
HW_DECODE_TEST := build/test_hw_decode
BRINGUP_GEN := build/make_bringup_rom
BRINGUP_ROM := build/via-blink.bin
BRINGUP_TEST := build/test_bringup_rom
IRQ_GEN := build/make_irq_rom
IRQ_ROM := build/via-irq.bin
IRQ_TEST := build/test_irq_rom
RAM_GEN := build/make_ram_smoke_rom
RAM_ROM := build/ram-smoke.bin
RAM_TEST := build/test_ram_smoke_rom
RAM_FULL_GEN := build/make_ram_full_rom
RAM_FULL_ROM := build/ram-full.bin
RAM_FULL_TEST := build/test_ram_full_rom
KLAUS_BIN ?= build/65C02_extended_opcodes_test.bin
SIM_SRC := simulator/cpu.c simulator/via.c simulator/serial.c

.PHONY: all test qualification hardware-test clean

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

$(HW_DECODE_TEST): hardware/test_decode.c | build
	$(CC) $(CFLAGS) hardware/test_decode.c -o $(HW_DECODE_TEST)

$(BRINGUP_GEN): rom/bringup/make_image.c | build
	$(CC) $(CFLAGS) rom/bringup/make_image.c -o $(BRINGUP_GEN)

$(BRINGUP_ROM): $(BRINGUP_GEN)
	./$(BRINGUP_GEN) $(BRINGUP_ROM)

$(BRINGUP_TEST): $(SIM_SRC) emulator/machine.c emulator/machine.h hardware/test_bringup_rom.c | build
	$(CC) $(CFLAGS) -Isimulator -Iemulator $(SIM_SRC) emulator/machine.c hardware/test_bringup_rom.c -o $(BRINGUP_TEST)

hardware-test: $(HW_DECODE_TEST) $(BRINGUP_ROM) $(BRINGUP_TEST)
	./$(HW_DECODE_TEST)
	./$(BRINGUP_TEST) $(BRINGUP_ROM)
	test "$(wc -c < $(BRINGUP_ROM))" -eq 16384

test: $(SIM_TEST) $(EMU_TEST)
	./$(SIM_TEST)
	./$(EMU_TEST)

clean:
	rm -rf build
