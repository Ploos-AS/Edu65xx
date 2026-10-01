CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -O2

SIM := build/edu65xx
SIM_TEST := build/test_cpu
EMU_TEST := build/test_machine
DEBUG_TEST := build/test_debugger
DEBUG_CLI := build/edu65xx-debug
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
REV_A_DIR := build/rev-a
KLAUS_BIN ?= build/65C02_extended_opcodes_test.bin
SIM_SRC := simulator/cpu.c simulator/via.c simulator/serial.c

.PHONY: all test qualification hardware-test rev-a-package clean

all: $(SIM) $(DEBUG_CLI) test

build:
	mkdir -p build

$(SIM): $(SIM_SRC) simulator/cpu.h simulator/via.h simulator/serial.h simulator/main.c | build
	$(CC) $(CFLAGS) -Isimulator $(SIM_SRC) simulator/main.c -o $(SIM)

$(SIM_TEST): $(SIM_SRC) simulator/cpu.h simulator/via.h simulator/test_cpu.c | build
	$(CC) $(CFLAGS) -Isimulator $(SIM_SRC) simulator/test_cpu.c -o $(SIM_TEST)

$(EMU_TEST): $(SIM_SRC) emulator/machine.c emulator/machine.h emulator/monitor_rom.c emulator/monitor_rom.h emulator/test_machine.c | build
	$(CC) $(CFLAGS) -Isimulator -Iemulator $(SIM_SRC) emulator/machine.c emulator/monitor_rom.c emulator/test_machine.c -o $(EMU_TEST)

$(DEBUG_TEST): $(SIM_SRC) emulator/machine.c emulator/machine.h emulator/debugger.c emulator/debugger.h emulator/test_debugger.c | build
	$(CC) $(CFLAGS) -Isimulator -Iemulator $(SIM_SRC) emulator/machine.c emulator/debugger.c emulator/test_debugger.c -o $(DEBUG_TEST)

$(DEBUG_CLI): $(SIM_SRC) emulator/machine.c emulator/machine.h emulator/debugger.c emulator/debugger.h emulator/debugger_cli.c | build
	$(CC) $(CFLAGS) -Isimulator -Iemulator $(SIM_SRC) emulator/machine.c emulator/debugger.c emulator/debugger_cli.c -o $(DEBUG_CLI)

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

$(IRQ_GEN): rom/bringup/make_irq_image.c | build
	$(CC) $(CFLAGS) rom/bringup/make_irq_image.c -o $(IRQ_GEN)

$(IRQ_ROM): $(IRQ_GEN)
	./$(IRQ_GEN) $(IRQ_ROM)

$(IRQ_TEST): $(SIM_SRC) emulator/machine.c emulator/machine.h hardware/test_irq_rom.c | build
	$(CC) $(CFLAGS) -Isimulator -Iemulator $(SIM_SRC) emulator/machine.c hardware/test_irq_rom.c -o $(IRQ_TEST)

$(RAM_GEN): rom/bringup/make_ram_smoke_image.c | build
	$(CC) $(CFLAGS) rom/bringup/make_ram_smoke_image.c -o $(RAM_GEN)

$(RAM_ROM): $(RAM_GEN)
	./$(RAM_GEN) $(RAM_ROM)

$(RAM_TEST): $(SIM_SRC) emulator/machine.c emulator/machine.h hardware/test_ram_smoke_rom.c | build
	$(CC) $(CFLAGS) -Isimulator -Iemulator $(SIM_SRC) emulator/machine.c hardware/test_ram_smoke_rom.c -o $(RAM_TEST)

$(RAM_FULL_GEN): rom/bringup/make_ram_full_image.c | build
	$(CC) $(CFLAGS) rom/bringup/make_ram_full_image.c -o $(RAM_FULL_GEN)

$(RAM_FULL_ROM): $(RAM_FULL_GEN)
	./$(RAM_FULL_GEN) $(RAM_FULL_ROM)

$(RAM_FULL_TEST): $(SIM_SRC) emulator/machine.c emulator/machine.h hardware/test_ram_full_rom.c | build
	$(CC) $(CFLAGS) -Isimulator -Iemulator $(SIM_SRC) emulator/machine.c hardware/test_ram_full_rom.c -o $(RAM_FULL_TEST)

hardware-test: $(HW_DECODE_TEST) $(BRINGUP_ROM) $(BRINGUP_TEST) $(IRQ_ROM) $(IRQ_TEST) $(RAM_ROM) $(RAM_TEST) $(RAM_FULL_ROM) $(RAM_FULL_TEST)
	python3 hardware/check_connectivity.py
	./$(HW_DECODE_TEST)
	./$(BRINGUP_TEST) $(BRINGUP_ROM)
	./$(RAM_TEST) $(RAM_ROM)
	./$(RAM_FULL_TEST) $(RAM_FULL_ROM)
	./$(IRQ_TEST) $(IRQ_ROM)
	test "$$(wc -c < $(BRINGUP_ROM))" -eq 16384
	test "$$(wc -c < $(RAM_ROM))" -eq 16384
	test "$$(wc -c < $(RAM_FULL_ROM))" -eq 16384
	test "$$(wc -c < $(IRQ_ROM))" -eq 16384

rev-a-package: hardware-test
	rm -rf $(REV_A_DIR)
	mkdir -p $(REV_A_DIR)
	cp $(BRINGUP_ROM) $(RAM_ROM) $(RAM_FULL_ROM) $(IRQ_ROM) $(REV_A_DIR)/
	cp hardware/BOM.md hardware/assembly-checklist.md hardware/bringup-sequence.md hardware/evidence-template.md $(REV_A_DIR)/
	cd $(REV_A_DIR) && sha256sum *.bin > SHA256SUMS
	printf '%s\n' 'Edu65xx Rev A physical build package' > $(REV_A_DIR)/README.txt
	printf '%s\n' 'ROM order: via-blink -> ram-smoke -> ram-full -> via-irq' >> $(REV_A_DIR)/README.txt

test: $(SIM_TEST) $(EMU_TEST) $(DEBUG_TEST)
	./$(SIM_TEST)
	./$(EMU_TEST)
	./$(DEBUG_TEST)

clean:
	rm -rf build
