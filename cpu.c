#include "cpu.h"
#include <stdio.h>

void cpu_execute_instruction(cpu_state_t* cpu, memory_t* mem) {
    // Executes an instruction, reading/writing data from/to
    // the provided memory and altering the provided CPU state.

    unsigned char opcode = mem_read(mem, cpu->pc);
    cpu->pc++;

    unsigned char imm8 = mem_read(mem, cpu->pc);
    unsigned short imm16 = mem_readw(mem, cpu->pc);

    cpu->result.cycles = 0;
    cpu->result.disasm = "";

    switch (opcode) {
        case 0x06: {
            cpu->bc.high = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld b, 0x%X", imm8);
            break;
        }
        case 0x0E: {
            cpu->bc.low = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld c, 0x%X", imm8);
            break;
        }
        case 0x16: {
            cpu->de.high = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld d, 0x%X", imm8);
            break;
        }
        case 0x1E: {
            cpu->de.low = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld e, 0x%X", imm8);
            break;
        }
        case 0x26: {
            cpu->hl.high = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld h, 0x%X", imm8);
            break;
        }
        case 0x2E: {
            cpu->hl.low = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld l, 0x%X", imm8);
            break;
        }
        case 0x7F: {
            cpu->af.high = cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld a, a");
            break;
        }
        case 0x78: {
            cpu->af.high = cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld a, b");
            break;
        }
        case 0x79: {
            cpu->af.high = cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld a, c");
            break;
        }
        case 0x7A: {
            cpu->af.high = cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld a, d");
            break;
        }
        case 0x7B: {
            cpu->af.high = cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld a, e");
            break;
        }
        case 0x7C: {
            cpu->af.high = cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld a, h");
            break;
        }
        case 0x7D: {
            cpu->af.high = cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld a, l");
            break;
        }
        case 0x7E: {
            cpu->af.high = mem_read(mem, cpu->hl.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld a, [hl]");
            break;
        }
        case 0x40: {
            cpu->bc.high = cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld b, b");
            break;
        }
        case 0x41: {
            cpu->bc.high = cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld b, c");
            break;
        }
        case 0x42: {
            cpu->bc.high = cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld b, d");
            break;
        }
        case 0x43: {
            cpu->bc.high = cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld b, e");
            break;
        }
        case 0x44: {
            cpu->bc.high = cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld b, h");
            break;
        }
        case 0x45: {
            cpu->bc.high = cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld b, l");
            break;
        }
        case 0x46: {
            cpu->bc.high = mem_read(mem, cpu->hl.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld b, [hl]");
            break;
        }
        case 0x48: {
            cpu->bc.low = cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld c, b");
            break;
        }
        case 0x49: {
            cpu->bc.low = cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld c, c");
            break;
        }
        case 0x4A: {
            cpu->bc.low = cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld c, d");
            break;
        }
        case 0x4B: {
            cpu->bc.low = cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld c, e");
            break;
        }
        case 0x4C: {
            cpu->bc.low = cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld c, h");
            break;
        }
        case 0x4D: {
            cpu->bc.low = cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld c, l");
            break;
        }
        case 0x4E: {
            cpu->bc.low = mem_read(mem, cpu->hl.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld c, [hl]");
            break;
        }
        case 0x50: {
            cpu->de.high = cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld d, b");
            break;
        }
        case 0x51: {
            cpu->de.high = cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld d, c");
            break;
        }
        case 0x52: {
            cpu->de.high = cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld d, d");
            break;
        }
        case 0x53: {
            cpu->de.high = cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld d, e");
            break;
        }
        case 0x54: {
            cpu->de.high = cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld d, h");
            break;
        }
        case 0x55: {
            cpu->de.high = cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld d, l");
            break;
        }
        case 0x56: {
            cpu->de.high = mem_read(mem, cpu->hl.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld d, [hl]");
            break;
        }
        case 0x58: {
            cpu->de.low = cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld e, b");
            break;
        }
        case 0x59: {
            cpu->de.low = cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld e, c");
            break;
        }
        case 0x5A: {
            cpu->de.low = cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld e, d");
            break;
        }
        case 0x5B: {
            cpu->de.low = cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld e, e");
            break;
        }
        case 0x5C: {
            cpu->de.low = cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld e, h");
            break;
        }
        case 0x5D: {
            cpu->de.low = cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld e, l");
            break;
        }
        case 0x5E: {
            cpu->de.low = mem_read(mem, cpu->hl.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld e, [hl]");
            break;
        }
        case 0x60: {
            cpu->hl.high = cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld h, b");
            break;
        }
        case 0x61: {
            cpu->hl.high = cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld h, c");
            break;
        }
        case 0x62: {
            cpu->hl.high = cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld h, d");
            break;
        }
        case 0x63: {
            cpu->hl.high = cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld h, e");
            break;
        }
        case 0x64: {
            cpu->hl.high = cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld h, h");
            break;
        }
        case 0x65: {
            cpu->hl.high = cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld h, l");
            break;
        }
        case 0x66: {
            cpu->hl.high = mem_read(mem, cpu->hl.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld h, [hl]");
            break;
        }
        case 0x68: {
            cpu->hl.low = cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld l, b");
            break;
        }
        case 0x69: {
            cpu->hl.low = cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld l, c");
            break;
        }
        case 0x6A: {
            cpu->hl.low = cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld l, d");
            break;
        }
        case 0x6B: {
            cpu->hl.low = cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld l, e");
            break;
        }
        case 0x6C: {
            cpu->hl.low = cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld l, h");
            break;
        }
        case 0x6D: {
            cpu->hl.low = cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld l, l");
            break;
        }
        case 0x6E: {
            cpu->hl.low = mem_read(mem, cpu->hl.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld l, [hl]");
            break;
        }
        default: {
            cpu->result.cycles = -1;
            sprintf(cpu->result.disasm, "unknown opcode 0x%X", opcode);
            break;
        }
    }
}
