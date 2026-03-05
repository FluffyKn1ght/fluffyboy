#include "cpu.h"
#include <stdint.h>
#include <stdio.h>

void cpu_stack_push(cpu_state_t* cpu, memory_t* mem, uint16_t value) {
    cpu->sp--;
    mem_write(mem, cpu->sp, value & 0xFF);
    cpu->sp--;
    mem_write(mem, cpu->sp, value & 0xFF00);
}

uint16_t cpu_stack_pop(cpu_state_t* cpu, memory_t* mem) {
    uint16_t value = mem_readw(mem, cpu->sp);
    cpu->sp += 2;
    return value;
}

void cpu_execute_instruction(cpu_state_t* cpu, memory_t* mem) {
    // Executes an instruction, reading/writing data from/to
    // the provided memory and altering the provided CPU state.

    uint8_t opcode = mem_read(mem, cpu->pc);
    cpu->pc++;

    uint8_t imm8 = mem_read(mem, cpu->pc);
    uint16_t imm16 = mem_readw(mem, cpu->pc);

    cpu->result.cycles = -1;
    cpu->result.size = 1;
    cpu->result.disasm = "????????";

    switch (opcode) {
        case 0x06: {
            cpu->bc.high = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld b, 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x0E: {
            cpu->bc.low = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld c, 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x16: {
            cpu->de.high = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld d, 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x1E: {
            cpu->de.low = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld e, 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x26: {
            cpu->hl.high = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld h, 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x2E: {
            cpu->hl.low = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld l, 0x%X", imm8);
            cpu->result.size = 2;
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
        case 0x70: {
            mem_write(mem, cpu->hl.word, cpu->bc.high);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [hl], b");
            break;
        }
        case 0x71: {
            mem_write(mem, cpu->hl.word, cpu->bc.low);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [hl], c");
            break;
        }
        case 0x72: {
            mem_write(mem, cpu->hl.word, cpu->de.high);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [hl], d");
            break;
        }
        case 0x73: {
            mem_write(mem, cpu->hl.word, cpu->de.low);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [hl], e");
            break;
        }
        case 0x74: {
            mem_write(mem, cpu->hl.word, cpu->hl.high);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [hl], h");
            break;
        }
        case 0x75: {
            mem_write(mem, cpu->hl.word, cpu->hl.low);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [hl], l");
            break;
        }
        case 0x36: {
            mem_write(mem, cpu->hl.word, imm8);
            cpu->pc++;

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ld [hl], 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x0A: {
            cpu->af.high = mem_read(mem, cpu->bc.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld a, [bc]");
            break;
        }
        case 0x1A: {
            cpu->af.high = mem_read(mem, cpu->de.word);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld a, [de]");
            break;
        }
        case 0xFA: {
            cpu->af.high = mem_read(mem, imm16);
            cpu->pc++;
            cpu->pc++;

            cpu->result.cycles = 16;
            sprintf(cpu->result.disasm, "ld a, [0x%X]", imm16);
            cpu->result.size = 3;
            break;
        }
        case 0x3E: {
            cpu->af.high = imm8;
            cpu->pc++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld a, 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x47: {
            cpu->bc.high = cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld b, a");
            break;
        }
        case 0x4F: {
            cpu->bc.low = cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld c, a");
            break;
        }
        case 0x57: {
            cpu->de.high = cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld d, a");
            break;
        }
        case 0x5F: {
            cpu->de.low = cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld e, a");
            break;
        }
        case 0x67: {
            cpu->hl.low = cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld h, a");
            break;
        }
        case 0x6F: {
            cpu->hl.low = cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ld l, a");
            break;
        }
        case 0x02: {
            mem_write(mem, cpu->bc.word, cpu->af.high);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [bc], a");
            break;
        }
        case 0x12: {
            mem_write(mem, cpu->de.word, cpu->af.high);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [de], a");
            break;
        }
        case 0x77: {
            mem_write(mem, cpu->hl.word, cpu->af.high);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [hl], a");
            break;
        }
        case 0xEA: {
            mem_write(mem, imm16, cpu->af.high);
            cpu->pc++;
            cpu->pc++;

            cpu->result.cycles = 16;
            sprintf(cpu->result.disasm, "ld [0x%X], a", imm16);
            cpu->result.size = 3;
            break;
        }
        case 0xF2: {
            cpu->af.high = mem_read(mem, 0xFF00 + cpu->bc.low);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld [0xFF00+c], a");
            break;
        }
        case 0xE2: {
            mem_write(mem, 0xFF00 + cpu->bc.low, cpu->af.high);

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld a, [0xFF00+c]");
            break;
        }
        case 0x3A: {
            cpu->af.high = mem_read(mem, cpu->hl.word);
            cpu->hl.word--;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ldd a, [hl]");
            break;
        }
        case 0x32: {
            mem_write(mem, cpu->hl.word, cpu->af.high);
            cpu->hl.word--;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ldd [hl], a");
            break;
        }
        case 0x2A: {
            cpu->af.high = mem_read(mem, cpu->hl.word);
            cpu->hl.word++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ldi a, [hl]");
            break;
        }
        case 0x22: {
            mem_write(mem, cpu->hl.word, cpu->af.high);
            cpu->hl.word++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ldi [hl], a");
            break;
        }
        case 0xE0: {
            mem_write(mem, 0xFF00 + imm8, cpu->af.high);
            cpu->pc++;

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ldh [0x%X], a", (0xFF00 + imm8));
            cpu->result.size = 2;
            break;
        }
        case 0xF0: {
            cpu->af.high = mem_read(mem, 0xFF00 + imm8);
            cpu->pc++;

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ldh a, [0x%X]", (0xFF00 + imm8));
            cpu->result.size = 2;
            break;
        }
        case 0x01: {
            cpu->bc.word = imm16;
            cpu->pc++;
            cpu->pc++;

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ld bc, 0x%X", imm16);
            cpu->result.size = 3;
            break;
        }
        case 0x11: {
            cpu->de.word = imm16;
            cpu->pc++;
            cpu->pc++;

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ld de, 0x%X", imm16);
            cpu->result.size = 3;
            break;
        }
        case 0x21: {
            cpu->hl.word = imm16;
            cpu->pc++;
            cpu->pc++;

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ld hl, 0x%X", imm16);
            cpu->result.size = 3;
            break;
        }
        case 0x31: {
            cpu->sp = imm16;
            cpu->pc++;
            cpu->pc++;

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ld sp, 0x%X", imm16);
            cpu->result.size = 3;
            break;
        }
        case 0xF9: {
            cpu->sp = cpu->hl.word;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "ld sp, hl");
            break;
        }
        case 0xF8: {
            cpu->af.low = 0;
            cpu->pc++;

            ((cpu_f_register*)&cpu->af.low)->h = ((cpu->sp & 0xF + ((int8_t)imm8)) & 0x10) ? 1 : 0;
            ((cpu_f_register*)&cpu->af.low)->c = ((cpu->sp + ((int8_t)imm8)) & 0x10000) ? 1 : 0;

            cpu->hl.word = cpu->sp + ((int8_t)imm8);

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ld hl, sp+(0x%X)", ((int8_t)imm8));
            cpu->result.size = 2;
            break;
        }
        case 0x08: {
            cpu->sp = imm16;
            cpu->pc++;
            cpu->pc++;

            cpu->result.cycles = 20;
            sprintf(cpu->result.disasm, "ld 0x%X, sp", imm16);
            cpu->result.size = 3;
            break;
        }
        case 0xF5: {
            cpu_stack_push(cpu, mem, cpu->af.word);

            cpu->result.cycles = 16;
            sprintf(cpu->result.disasm, "push af");
            break;
        }
        case 0xC5: {
            cpu_stack_push(cpu, mem, cpu->bc.word);

            cpu->result.cycles = 16;
            sprintf(cpu->result.disasm, "push bc");
            break;
        }
        case 0xD5: {
            cpu_stack_push(cpu, mem, cpu->de.word);

            cpu->result.cycles = 16;
            sprintf(cpu->result.disasm, "push de");
            break;
        }
        case 0xE5: {
            cpu_stack_push(cpu, mem, cpu->hl.word);

            cpu->result.cycles = 16;
            sprintf(cpu->result.disasm, "push hl");
            break;
        }
        case 0xF1: {
            cpu->af.word = (cpu_stack_pop(cpu, mem) & 0xFFF0);

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "pop af");
            break;
        }
        case 0xC1: {
            cpu->bc.word = cpu_stack_pop(cpu, mem);

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "pop bc");
            break;
        }
        case 0xD1: {
            cpu->de.word = cpu_stack_pop(cpu, mem);

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "pop de");
            break;
        }
        case 0xE1: {
            cpu->hl.word = cpu_stack_pop(cpu, mem);

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "pop hl");
            break;
        }
        default: {
            sprintf(cpu->result.disasm, "unknown opcode 0x%X", opcode);
            break;
        }
    }
}
