#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debugger.h"

static const char *stop_name(edu65xx_debug_stop_t stop)
{
    switch (stop) {
    case EDU65XX_DEBUG_STOP_NONE: return "NONE";
    case EDU65XX_DEBUG_STOP_BREAKPOINT: return "BREAKPOINT";
    case EDU65XX_DEBUG_STOP_WATCH_READ: return "WATCH_READ";
    case EDU65XX_DEBUG_STOP_WATCH_WRITE: return "WATCH_WRITE";
    case EDU65XX_DEBUG_STOP_STEP_LIMIT: return "STEP_LIMIT";
    case EDU65XX_DEBUG_STOP_MACHINE_ERROR: return "MACHINE_ERROR";
    }
    return "UNKNOWN";
}

static void regs(const edu65xx_machine_t *m)
{
    printf("PC=%04X A=%02X X=%02X Y=%02X SP=%02X P=%02X steps=%llu\n",
           m->cpu.pc, m->cpu.a, m->cpu.x, m->cpu.y, m->cpu.sp, m->cpu.p,
           (unsigned long long)m->steps);
}

static void help(void)
{
    puts("commands:");
    puts("  s                 step one instruction");
    puts("  r [count]         run, default limit 100000");
    puts("  b ADDRESS         add PC breakpoint");
    puts("  wr ADDRESS        add read watchpoint");
    puts("  ww ADDRESS        add write watchpoint");
    puts("  regs              show registers");
    puts("  m ADDRESS [LEN]   dump memory, default 16 bytes");
    puts("  q                 quit");
    puts("addresses are hexadecimal, with or without 0x");
}

static int parse_hex(const char *s, unsigned long *value)
{
    char *end;
    *value = strtoul(s, &end, 16);
    return s != end && *end == '\0';
}

int main(int argc, char **argv)
{
    edu65xx_machine_t machine;
    edu65xx_debugger_t debugger;
    uint8_t rom[EDU65XX_ROM_SIZE];
    char line[128];
    FILE *file;

    if (argc != 2) {
        fprintf(stderr, "usage: %s rom.bin\n", argv[0]);
        return 2;
    }

    file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("fopen");
        return 2;
    }
    if (fread(rom, 1u, sizeof(rom), file) != sizeof(rom) || fgetc(file) != EOF) {
        fprintf(stderr, "ROM must be exactly %u bytes\n", EDU65XX_ROM_SIZE);
        fclose(file);
        return 2;
    }
    fclose(file);

    edu65xx_machine_init(&machine);
    if (edu65xx_machine_load_rom(&machine, rom, sizeof(rom), 0u) != 0)
        return 2;
    edu65xx_machine_reset(&machine);
    edu65xx_debugger_init(&debugger, &machine);

    puts("Edu65xx debugger. Type help for commands.");
    regs(&machine);

    while (fgets(line, sizeof(line), stdin) != NULL) {
        char *cmd = strtok(line, " \t\r\n");
        char *arg = strtok(NULL, " \t\r\n");
        char *arg2 = strtok(NULL, " \t\r\n");
        unsigned long value, length, i;

        if (cmd == NULL)
            continue;
        if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0)
            break;
        if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
            help();
        } else if (strcmp(cmd, "regs") == 0) {
            regs(&machine);
        } else if (strcmp(cmd, "s") == 0) {
            if (edu65xx_debugger_step(&debugger) != 0)
                puts("stop=MACHINE_ERROR");
            else {
                printf("stop=%s\n", stop_name(debugger.last_stop));
                regs(&machine);
            }
        } else if (strcmp(cmd, "r") == 0) {
            uint64_t limit = 100000u;
            if (arg != NULL) {
                char *end;
                unsigned long long n = strtoull(arg, &end, 0);
                if (arg == end || *end != '\0') { puts("bad count"); continue; }
                limit = (uint64_t)n;
            }
            printf("stop=%s", stop_name(edu65xx_debugger_run(&debugger, limit)));
            if (debugger.last_stop == EDU65XX_DEBUG_STOP_WATCH_READ ||
                debugger.last_stop == EDU65XX_DEBUG_STOP_WATCH_WRITE)
                printf(" address=%04X data=%02X", debugger.stop_address, debugger.stop_data);
            putchar('\n');
            regs(&machine);
        } else if (strcmp(cmd, "b") == 0 || strcmp(cmd, "wr") == 0 || strcmp(cmd, "ww") == 0) {
            if (arg == NULL || !parse_hex(arg, &value) || value > 0xFFFFu) {
                puts("bad address");
                continue;
            }
            if (strcmp(cmd, "b") == 0)
                i = (unsigned long)edu65xx_debugger_add_breakpoint(&debugger, (uint16_t)value);
            else
                i = (unsigned long)edu65xx_debugger_add_watchpoint(
                    &debugger, (uint16_t)value, strcmp(cmd, "wr") == 0, strcmp(cmd, "ww") == 0);
            puts(i == 0u ? "ok" : "table full or invalid");
        } else if (strcmp(cmd, "m") == 0) {
            if (arg == NULL || !parse_hex(arg, &value) || value > 0xFFFFu) {
                puts("bad address");
                continue;
            }
            length = 16u;
            if (arg2 != NULL && (!parse_hex(arg2, &length) || length == 0u)) {
                puts("bad length");
                continue;
            }
            for (i = 0; i < length && value + i <= 0xFFFFu; ++i) {
                if ((i % 16u) == 0u) printf("%04lX:", value + i);
                printf(" %02X", machine.cpu.memory[value + i]);
                if ((i % 16u) == 15u || i + 1u == length || value + i == 0xFFFFu)
                    putchar('\n');
            }
        } else {
            puts("unknown command");
        }
    }
    return 0;
}
