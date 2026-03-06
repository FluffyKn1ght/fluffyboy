#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void cpu_stack_push(cpu_state_t* cpu, memory_t* mem, uint16_t value) {
    // Adds a value onto the top of the stack and decrements SP twice.

    cpu->sp--;
    mem_write(mem, cpu->sp, value & 0xFF);
    cpu->sp--;
    mem_write(mem, cpu->sp, value & 0xFF00);
}

uint16_t cpu_stack_pop(cpu_state_t* cpu, memory_t* mem) {
    // Returns a value from the top of the stack and increments SP twice.

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
    cpu_f_register_t* flags = (cpu_f_register_t*)&cpu->af.low;

    cpu->result.cycles = -1;
    cpu->result.size = 1;

    if (cpu->ei_state == ARMED) {
        cpu->ei_state = ACTIVE;
    } else if (cpu->ei_state == ACTIVE) {
        cpu->ei_state = NOT_ARMED;
        cpu->ime = true;
    }

    if (cpu->halted) {
        // TODO: Implement interrupt checking
        cpu->result.cycles = 1;
        cpu->result.size = 0;
        return;
    }

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
            flags->byte = 0;
            cpu->pc++;

            if ((int8_t)imm8 >= 0) {
                flags->h = ((cpu->sp & 0xF + ((int8_t)imm8) & 0xF) >= 0x10) ? 1 : 0;
                flags->c = ((cpu->sp + ((int8_t)imm8)) >= 0x10000) ? 1 : 0;
            } else {
                flags->h = (((cpu->sp & 0xF) + ((int8_t)imm8) & 0xF) < (cpu->sp & 0xF)) ? 1 : 0;
                flags->c = ((cpu->sp + ((int8_t)imm8)) < (cpu->sp)) ? 1 : 0;
            }

            cpu->hl.word = cpu->sp + ((int8_t)imm8);

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "ld hl, sp+(%d)", ((int8_t)imm8));
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
        case 0x87: {
            flags->byte = 0;
            if ((cpu->af.high + cpu->af.high) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->af.high) & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->af.high) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "add a");
            break;
        }
        case 0x80: {
            flags->byte = 0;
            if ((cpu->af.high + cpu->bc.high) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->bc.high) & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->bc.high) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "add b");
            break;
        }
        case 0x81: {
            flags->byte = 0;
            if ((cpu->af.high + cpu->bc.low) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->bc.low) & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->bc.low) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "add c");
            break;
        }
        case 0x82: {
            flags->byte = 0;
            if ((cpu->af.high + cpu->de.high) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->de.high) & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->de.high) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "add d");
            break;
        }
        case 0x83: {
            flags->byte = 0;
            if ((cpu->af.high + cpu->de.low) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->de.low) & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->de.low) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "add e");
            break;
        }
        case 0x84: {
            flags->byte = 0;
            if ((cpu->af.high + cpu->hl.high) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->hl.high) & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->hl.high) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "add h");
            break;
        }
        case 0x85: {
            flags->byte = 0;
            if ((cpu->af.high + cpu->hl.low) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->hl.low) & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->hl.low) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "add l");
            break;
        }
        case 0x86: {
            flags->byte = 0;
            uint8_t value = mem_read(mem, cpu->hl.word);

            if ((cpu->af.high + value) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + (value & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + value) >= 0x10000) ? 1 : 0;
            cpu->af.high += value;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "add [hl]");
            break;
        }
        case 0xC6: {
            flags->byte = 0;
            cpu->pc++;
            if ((cpu->af.high + imm8) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + (imm8 & 0xF)) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + imm8) >= 0x10000) ? 1 : 0;
            cpu->af.high += imm8;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "add 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x8F: {
            flags->byte = 0;
            bool carry = flags->c;

            if ((cpu->af.high + cpu->af.high + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->af.high) & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->af.high + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->af.high + carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "adc a");
            break;
        }
        case 0x88: {
            flags->byte = 0;
            bool carry = flags->c;

            if ((cpu->af.high + cpu->bc.high + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->bc.high) & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->bc.high + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->bc.high + carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "adc b");
            break;
        }
        case 0x89: {
            flags->byte = 0;
            bool carry = flags->c;

            if ((cpu->af.high + cpu->bc.low + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->bc.low) & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->bc.low + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->bc.low + carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "adc c");
            break;
        }
        case 0x8A: {
            flags->byte = 0;
            bool carry = flags->c;

            if ((cpu->af.high + cpu->de.high + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->de.high) & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->de.high + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->de.high + carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "adc d");
            break;
        }
        case 0x8B: {
            flags->byte = 0;
            bool carry = flags->c;

            if ((cpu->af.high + cpu->de.low + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->de.low) & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->de.low + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->de.low + carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "adc e");
            break;
        }
        case 0x8C: {
            flags->byte = 0;
            bool carry = flags->c;

            if ((cpu->af.high + cpu->hl.high + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->hl.high) & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->hl.high + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->hl.high + carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "adc h");
            break;
        }
        case 0x8D: {
            flags->byte = 0;
            bool carry = flags->c;

            if ((cpu->af.high + cpu->hl.low + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + ((cpu->hl.low) & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + cpu->hl.low + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += cpu->hl.low + carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "adc l");
            break;
        }
        case 0x8E: {
            flags->byte = 0;
            bool carry = flags->c;
            uint8_t value = mem_read(mem, cpu->hl.word);

            if ((cpu->af.high + value + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + (value & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + value + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += value + carry;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "adc [hl]");
            break;
        }
        case 0xCE: {
            flags->byte = 0;
            bool carry = flags->c;
            cpu->pc++;

            if ((cpu->af.high + imm8 + carry) == 0) {
                flags->z = 1;
            }
            flags->h = ((((cpu->af.high) & 0xF) + (imm8 & 0xF) + carry) >= 0x10) ? 1 : 0;
            flags->c = ((cpu->af.high + imm8 + carry) >= 0x10000) ? 1 : 0;
            cpu->af.high += imm8 + carry;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "adc 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x97: {
            flags->byte = 0;
            flags->n = 1;

            if ((cpu->af.high - cpu->af.high) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->af.high) & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->af.high) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sub a");
            break;
        }
        case 0x90: {
            flags->byte = 0;
            flags->n = 1;

            if ((cpu->af.high - cpu->bc.high) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->bc.high) & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->bc.high) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sub b");
            break;
        }
        case 0x91: {
            flags->byte = 0;
            flags->n = 1;

            if ((cpu->af.high - cpu->bc.low) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->bc.low) & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->bc.low) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sub c");
            break;
        }
        case 0x92: {
            flags->byte = 0;
            flags->n = 1;

            if ((cpu->af.high - cpu->de.high) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->de.high) & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->de.high) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sub d");
            break;
        }
        case 0x93: {
            flags->byte = 0;
            flags->n = 1;

            if ((cpu->af.high - cpu->de.low) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->de.low) & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->de.low) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sub e");
            break;
        }
        case 0x94: {
            flags->byte = 0;
            flags->n = 1;

            if ((cpu->af.high - cpu->hl.high) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->hl.high) & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->hl.high) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sub h");
            break;
        }
        case 0x95: {
            flags->byte = 0;
            flags->n = 1;

            if ((cpu->af.high - cpu->hl.low) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->hl.low) & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->hl.low) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sub l");
            break;
        }
        case 0x96: {
            flags->byte = 0;
            flags->n = 1;
            uint8_t value = mem_read(mem, cpu->hl.word);

            if ((cpu->af.high - value) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - (value & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - value) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= value;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "sub [hl]");
            break;
        }
        case 0xD6: {
            flags->byte = 0;
            flags->n = 1;
            cpu->pc++;

            if (cpu->af.high - imm8 == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - (imm8 & 0xF))) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - imm8) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= imm8;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "sub 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x9F: {
            flags->byte = 0;
            flags->n = 1;
            bool carry = flags->c;

            if ((cpu->af.high - cpu->af.high - carry) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->af.high) & 0xF)) - carry) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->af.high - carry) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->af.high - carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sbc a");
            break;
        }
        case 0x98: {
            flags->byte = 0;
            flags->n = 1;
            bool carry = flags->c;

            if ((cpu->af.high - cpu->bc.high - carry) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->bc.high) & 0xF)) - carry) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->bc.high - carry) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->bc.high - carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sbc b");
            break;
        }
        case 0x99: {
            flags->byte = 0;
            flags->n = 1;
            bool carry = flags->c;

            if ((cpu->af.high - cpu->bc.low - carry) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->bc.low) & 0xF)) - carry) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->bc.low - carry) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->bc.low - carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sbc c");
            break;
        }
        case 0x9A: {
            flags->byte = 0;
            flags->n = 1;
            bool carry = flags->c;

            if ((cpu->af.high - cpu->de.high - carry) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->de.high) & 0xF)) - carry) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->de.high - carry) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->de.high - carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sbc d");
            break;
        }
        case 0x9B: {
            flags->byte = 0;
            flags->n = 1;
            bool carry = flags->c;

            if ((cpu->af.high - cpu->de.low - carry) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->de.low) & 0xF)) - carry) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->de.low - carry) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->de.low - carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sbc e");
            break;
        }
        case 0x9C: {
            flags->byte = 0;
            flags->n = 1;
            bool carry = flags->c;

            if ((cpu->af.high - cpu->hl.high - carry) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->hl.high) & 0xF)) - carry) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->hl.high - carry) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->hl.high - carry;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sbc h");
            break;
        }
        case 0x9D: {
            flags->byte = 0;
            flags->n = 1;
            bool carry = flags->c;

            if ((cpu->af.high - cpu->hl.low - carry) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - ((cpu->hl.low) & 0xF)) - carry) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - cpu->hl.low - carry) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "sbc l");
            break;
        }
        case 0x9E: {
            flags->byte = 0;
            flags->n = 1;
            uint8_t value = mem_read(mem, cpu->hl.word);
            bool carry = flags->c;

            if ((cpu->af.high - value - carry) == 0) {
                flags->z = 1;
            }

            flags->h = (((((cpu->af.high) & 0xF) - (value & 0xF)) - carry) < ((cpu->af.high) & 0xF)) ? 1 : 0;
            flags->c = ((cpu->af.high - value - carry) < cpu->af.high) ? 1 : 0;

            cpu->af.high -= value - carry;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "sbc [hl]");
            break;
        }
        case 0xA7: {
            flags->byte = 0;
            flags->h = 1;

            flags->z = (cpu->af.high & cpu->af.high) ? 1 : 0;

            cpu->af.high &= cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "and a");
            break;
        }
        case 0xA0: {
            flags->byte = 0;
            flags->h = 1;

            flags->z = (cpu->af.high & cpu->bc.high) ? 1 : 0;

            cpu->af.high &= cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "and b");
            break;
        }
        case 0xA1: {
            flags->byte = 0;
            flags->h = 1;

            flags->z = (cpu->af.high & cpu->bc.low) ? 1 : 0;

            cpu->af.high &= cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "and c");
            break;
        }
        case 0xA2: {
            flags->byte = 0;
            flags->h = 1;

            flags->z = (cpu->af.high & cpu->de.high) ? 1 : 0;

            cpu->af.high &= cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "and d");
            break;
        }
        case 0xA3: {
            flags->byte = 0;
            flags->h = 1;

            flags->z = (cpu->af.high & cpu->de.low) ? 1 : 0;

            cpu->af.high &= cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "and e");
            break;
        }
        case 0xA4: {
            flags->byte = 0;
            flags->h = 1;

            flags->z = (cpu->af.high & cpu->hl.high) ? 1 : 0;

            cpu->af.high &= cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "and h");
            break;
        }
        case 0xA5: {
            flags->byte = 0;
            flags->h = 1;

            flags->z = (cpu->af.high & cpu->hl.low) ? 1 : 0;

            cpu->af.high &= cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "and l");
            break;
        }
        case 0xA6: {
            flags->byte = 0;
            flags->h = 1;
            uint8_t value = mem_read(mem, cpu->hl.word);

            flags->z = (cpu->af.high & value) ? 1 : 0;

            cpu->af.high &= value;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "and [hl]");
            break;
        }
        case 0xE6: {
            flags->byte = 0;
            flags->h = 1;

            flags->z = (cpu->af.high & imm8) ? 1 : 0;

            cpu->af.high &= imm8;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "and 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0xB7: {
            flags->byte = 0;

            flags->z = (cpu->af.high | cpu->af.high) ? 1 : 0;

            cpu->af.high |= cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "or a");
            break;
        }
        case 0xB0: {
            flags->byte = 0;

            flags->z = (cpu->af.high | cpu->bc.high) ? 1 : 0;

            cpu->af.high |= cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "or b");
            break;
        }
        case 0xB1: {
            flags->byte = 0;

            flags->z = (cpu->af.high | cpu->bc.low) ? 1 : 0;

            cpu->af.high |= cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "or c");
            break;
        }
        case 0xB2: {
            flags->byte = 0;

            flags->z = (cpu->af.high | cpu->de.high) ? 1 : 0;

            cpu->af.high |= cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "or d");
            break;
        }
        case 0xB3: {
            flags->byte = 0;

            flags->z = (cpu->af.high | cpu->de.low) ? 1 : 0;

            cpu->af.high |= cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "or e");
            break;
        }
        case 0xB4: {
            flags->byte = 0;

            flags->z = (cpu->af.high | cpu->hl.high) ? 1 : 0;

            cpu->af.high |= cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "or h");
            break;
        }
        case 0xB5: {
            flags->byte = 0;

            flags->z = (cpu->af.high | cpu->hl.low) ? 1 : 0;

            cpu->af.high |= cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "or l");
            break;
        }
        case 0xB6: {
            flags->byte = 0;
            uint8_t value = mem_read(mem, cpu->hl.word);

            flags->z = (cpu->af.high | value) ? 1 : 0;

            cpu->af.high |= value;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "or [hl]");
            break;
        }
        case 0xF6: {
            flags->byte = 0;

            flags->z = (cpu->af.high | imm8) ? 1 : 0;

            cpu->af.high |= imm8;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "or 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0xAF: {
            flags->byte = 0;

            flags->z = (cpu->af.high ^ cpu->af.high) ? 1 : 0;

            cpu->af.high ^= cpu->af.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "xor a");
            break;
        }
        case 0xA8: {
            flags->byte = 0;

            flags->z = (cpu->af.high ^ cpu->bc.high) ? 1 : 0;

            cpu->af.high ^= cpu->bc.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "xor b");
            break;
        }
        case 0xA9: {
            flags->byte = 0;

            flags->z = (cpu->af.high ^ cpu->bc.low) ? 1 : 0;

            cpu->af.high ^= cpu->bc.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "xor c");
            break;
        }
        case 0xAA: {
            flags->byte = 0;

            flags->z = (cpu->af.high ^ cpu->de.high) ? 1 : 0;

            cpu->af.high ^= cpu->de.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "xor d");
            break;
        }
        case 0xAB: {
            flags->byte = 0;

            flags->z = (cpu->af.high ^ cpu->de.low) ? 1 : 0;

            cpu->af.high ^= cpu->de.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "xor e");
            break;
        }
        case 0xAC: {
            flags->byte = 0;

            flags->z = (cpu->af.high ^ cpu->hl.high) ? 1 : 0;

            cpu->af.high ^= cpu->hl.high;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "xor h");
            break;
        }
        case 0xAD: {
            flags->byte = 0;

            flags->z = (cpu->af.high ^ cpu->hl.low) ? 1 : 0;

            cpu->af.high ^= cpu->hl.low;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "xor l");
            break;
        }
        case 0xAE: {
            flags->byte = 0;
            uint8_t value = mem_read(mem, cpu->hl.word);

            flags->z = (cpu->af.high ^ value) ? 1 : 0;

            cpu->af.high ^= value;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "xor [hl]");
            break;
        }
        case 0xEE: {
            flags->byte = 0;

            flags->z = (cpu->af.high ^ imm8) ? 1 : 0;

            cpu->af.high ^= imm8;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "xor 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0xBF: {
            flags->byte = 0;
            flags->n = 1;

            flags->z = (cpu->af.high == cpu->af.high) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (cpu->af.high & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < cpu->af.high) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "cp a");
            break;
        }
        case 0xB8: {
            flags->byte = 0;
            flags->n = 1;

            flags->z = (cpu->af.high == cpu->bc.high) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (cpu->bc.high & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < cpu->bc.high) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "cp b");
            break;
        }
        case 0xB9: {
            flags->byte = 0;
            flags->n = 1;

            flags->z = (cpu->af.high == cpu->bc.low) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (cpu->bc.low & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < cpu->bc.low) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "cp c");
            break;
        }
        case 0xBA: {
            flags->byte = 0;
            flags->n = 1;

            flags->z = (cpu->af.high == cpu->de.high) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (cpu->de.high & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < cpu->de.high) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "cp d");
            break;
        }
        case 0xBB: {
            flags->byte = 0;
            flags->n = 1;

            flags->z = (cpu->af.high == cpu->de.low) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (cpu->de.low & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < cpu->de.low) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "cp e");
            break;
        }
        case 0xBC: {
            flags->byte = 0;
            flags->n = 1;

            flags->z = (cpu->af.high == cpu->hl.high) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (cpu->hl.high & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < cpu->hl.high) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "cp h");
            break;
        }
        case 0xBD: {
            flags->byte = 0;
            flags->n = 1;

            flags->z = (cpu->af.high == cpu->hl.low) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (cpu->hl.low & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < cpu->hl.low) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "cp l");
            break;
        }
        case 0xBE: {
            flags->byte = 0;
            flags->n = 1;
            uint8_t value = mem_read(mem, cpu->hl.word);

            flags->z = (cpu->af.high == value) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (value & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < value) ? 1 : 0;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "cp [hl]");
            break;
        }
        case 0xFE: {
            flags->byte = 0;
            flags->n = 1;
            cpu->pc++;

            flags->z = (cpu->af.high == imm8) ? 1 : 0;
            flags->h = (((cpu->af.high & 0xF) - (imm8 & 0xF)) < (cpu->af.high & 0xF)) ? 1 : 0;
            flags->c = (cpu->af.high < imm8) ? 1 : 0;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "cp 0x%X", imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x3C: {
            flags->byte &= 0x10;

            flags->h = (((cpu->af.high & 0xF) + 1) >= 0x10) ? 1 : 0;
            cpu->af.high++;
            flags->z = (cpu->af.high == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "inc a");
            break;
        }
        case 0x04: {
            flags->byte &= 0x10;

            flags->h = (((cpu->bc.high & 0xF) + 1) >= 0x10) ? 1 : 0;
            cpu->bc.high++;
            flags->z = (cpu->bc.high == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "inc b");
            break;
        }
        case 0x0C: {
            flags->byte &= 0x10;

            flags->h = (((cpu->bc.low & 0xF) + 1) >= 0x10) ? 1 : 0;
            cpu->bc.low++;
            flags->z = (cpu->bc.low == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "inc c");
            break;
        }
        case 0x14: {
            flags->byte &= 0x10;

            flags->h = (((cpu->de.high & 0xF) + 1) >= 0x10) ? 1 : 0;
            cpu->de.high++;
            flags->z = (cpu->de.high == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "inc d");
            break;
        }
        case 0x1C: {
            flags->byte &= 0x10;

            flags->h = (((cpu->de.low & 0xF) + 1) >= 0x10) ? 1 : 0;
            cpu->de.low++;
            flags->z = (cpu->de.low == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "inc e");
            break;
        }
        case 0x24: {
            flags->byte &= 0x10;

            flags->h = (((cpu->hl.high & 0xF) + 1) >= 0x10) ? 1 : 0;
            cpu->hl.high++;
            flags->z = (cpu->hl.high == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "inc h");
            break;
        }
        case 0x2C: {
            flags->byte &= 0x10;

            flags->h = (((cpu->hl.low & 0xF) + 1) >= 0x10) ? 1 : 0;
            cpu->hl.low++;
            flags->z = (cpu->hl.low == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "inc l");
            break;
        }
        case 0x34: {
            flags->byte &= 0x10;
            uint8_t value = mem_read(mem, cpu->hl.word);

            flags->h = (((value & 0xF) + 1) >= 0x10) ? 1 : 0;
            value++;
            flags->z = (value == 0x00) ? 1 : 0;

            mem_write(mem, cpu->hl.word, value);

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "inc [hl]");
            break;
        }
        case 0x3D: {
            flags->byte &= 0x10;

            flags->h = (((cpu->af.high & 0xF) - 1) < (cpu->af.high & 0xF)) ? 1 : 0;
            cpu->af.high--;
            flags->z = (cpu->af.high == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "dec a");
            break;
        }
        case 0x05: {
            flags->byte &= 0x10;

            flags->h = (((cpu->bc.high & 0xF) - 1) < (cpu->bc.high & 0xF)) ? 1 : 0;
            cpu->bc.high--;
            flags->z = (cpu->bc.high == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "dec b");
            break;
        }
        case 0x0D: {
            flags->byte &= 0x10;

            flags->h = (((cpu->bc.low & 0xF) - 1) < (cpu->bc.low & 0xF)) ? 1 : 0;
            cpu->bc.low--;
            flags->z = (cpu->bc.low == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "dec c");
            break;
        }
        case 0x15: {
            flags->byte &= 0x10;

            flags->h = (((cpu->de.high & 0xF) - 1) < (cpu->de.high & 0xF)) ? 1 : 0;
            cpu->de.high++;
            flags->z = (cpu->de.high == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "dec d");
            break;
        }
        case 0x1D: {
            flags->byte &= 0x10;

            flags->h = (((cpu->de.low & 0xF) - 1) < (cpu->de.low & 0xF)) ? 1 : 0;
            cpu->de.low--;
            flags->z = (cpu->de.low == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "dec e");
            break;
        }
        case 0x25: {
            flags->byte &= 0x10;

            flags->h = (((cpu->hl.high & 0xF) - 1) < (cpu->hl.high & 0xF)) ? 1 : 0;
            cpu->hl.high--;
            flags->z = (cpu->hl.high == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "dec h");
            break;
        }
        case 0x2D: {
            flags->byte &= 0x10;

            flags->h = (((cpu->hl.low & 0xF) - 1) < (cpu->hl.low & 0xF)) ? 1 : 0;
            cpu->hl.low--;
            flags->z = (cpu->hl.low == 0x00) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "dec l");
            break;
        }
        case 0x35: {
            flags->byte &= 0x10;
            uint8_t value = mem_read(mem, cpu->hl.word);

            flags->h = (((value & 0xF) - 1) < (value & 0xF)) ? 1 : 0;
            value++;
            flags->z = (value == 0x00) ? 1 : 0;

            mem_write(mem, cpu->hl.word, value);

            cpu->result.cycles = 12;
            sprintf(cpu->result.disasm, "dec [hl]");
            break;
        }
        case 0x09: {
            flags->byte &= 0x80;

            flags->h = (((cpu->hl.word & 0xFFF) + (cpu->bc.word & 0xFFF)) >= 0x1000) ? 1 : 0;
            flags->c = ((cpu->hl.word + cpu->bc.word) >= 0x10000) ? 1 : 0;

            cpu->hl.word += cpu->bc.word;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "add hl, bc");
            break;
        }
        case 0x19: {
            flags->byte &= 0x80;

            flags->h = (((cpu->hl.word & 0xFFF) + (cpu->de.word & 0xFFF)) >= 0x1000) ? 1 : 0;
            flags->c = ((cpu->hl.word + cpu->de.word) >= 0x10000) ? 1 : 0;

            cpu->hl.word += cpu->de.word;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "add hl, de");
            break;
        }
        case 0x29: {
            flags->byte &= 0x80;

            flags->h = (((cpu->hl.word & 0xFFF) + (cpu->hl.word & 0xFFF)) >= 0x1000) ? 1 : 0;
            flags->c = ((cpu->hl.word + cpu->hl.word) >= 0x10000) ? 1 : 0;

            cpu->hl.word += cpu->hl.word;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "add hl, hl");
            break;
        }
        case 0x39: {
            flags->byte &= 0x80;

            flags->h = (((cpu->hl.word & 0xFFF) + (cpu->sp & 0xFFF)) >= 0x1000) ? 1 : 0;
            flags->c = ((cpu->hl.word + cpu->sp) >= 0x10000) ? 1 : 0;

            cpu->hl.word += cpu->sp;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "add hl, sp");
            break;
        }
        case 0xE8: {
            flags->byte = 0;
            cpu->pc++;

            if ((int8_t)imm8 >= 0) {
                flags->h = ((cpu->sp & 0xF + ((int8_t)imm8) & 0xF) >= 0x10) ? 1 : 0;
                flags->c = ((cpu->sp + ((int8_t)imm8)) >= 0x10000) ? 1 : 0;
            } else {
                flags->h = (((cpu->sp & 0xF) + ((int8_t)imm8) & 0xF) < (cpu->sp & 0xF)) ? 1 : 0;
                flags->c = ((cpu->sp + ((int8_t)imm8)) < (cpu->sp)) ? 1 : 0;
            }

            cpu->sp += (int8_t)imm8;

            cpu->result.cycles = 16;
            sprintf(cpu->result.disasm, "add sp, %d", (int8_t)imm8);
            cpu->result.size = 2;
            break;
        }
        case 0x03: {
            cpu->bc.word++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "inc bc");
            break;
        }
        case 0x13: {
            cpu->de.word++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "inc de");
            break;
        }
        case 0x23: {
            cpu->hl.word++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "inc hl");
            break;
        }
        case 0x33: {
            cpu->sp++;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "inc sp");
            break;
        }
        case 0x0B: {
            cpu->bc.word--;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "dec bc");
            break;
        }
        case 0x1B: {
            cpu->de.word--;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "dec de");
            break;
        }
        case 0x2B: {
            cpu->hl.word--;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "dec hl");
            break;
        }
        case 0x3B: {
            cpu->sp--;

            cpu->result.cycles = 8;
            sprintf(cpu->result.disasm, "dec sp");
            break;
        }
        case 0x27: {
            if (cpu->af.high == 0) {
                flags->byte &= 0x40;
                flags->z = 1;
            } else {
                int8_t adjust = 0;
                bool carry = false;

                if (!flags->n) {
                    // Addition
                    if (((cpu->af.high & 0xF) > 0x9) || flags->h) {
                        adjust += 0x6;
                    }
                    if (((cpu->af.high & 0xF0) > 0x90) || flags->c) {
                        carry = true;
                        adjust += 0x60;
                    }
                } else {
                    // Subtraction
                    if (((cpu->af.high & 0xF) > 0x9) || flags->h) {
                        adjust -= 0x6;
                    }
                    if (((cpu->af.high & 0xF0) > 0x90) || flags->c) {
                        carry = true;
                        adjust -= 0x60;
                    }
                }

                flags->byte &= 0x40;
                flags->c = carry;
                flags->z = (cpu->af.high == 0) ? 1 : 0;
                cpu->af.high += adjust;
            }

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "daa");
            break;
        }
        case 0x2F: {
            flags->n = 1;
            flags->h = 1;
            cpu->af.high = ~(cpu->af.high);

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "cpl");
            break;
        }
        case 0x3F: {
            flags->n = 0;
            flags->h = 0;
            flags->c = !flags->c;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ccf");
            break;
        }
        case 0x37: {
            flags->n = 0;
            flags->h = 0;
            flags->c = 1;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "scf");
            break;
        }
        case 0x00: {
            // *insert careless fox noises*

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "nop");
            break;
        }
        case 0x76: {
            // *insert bored fox noises*

            cpu->halted = true;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "halt");
            break;
        }
        case 0x10: {
            // *insert sleeping fox noises*

            // TODO: Make sure this arse pain of an opcode is implemented properly...

            cpu->stopped = true;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "stop");
            break;
        }
        case 0xF3: {
            // TODO: Figure out if this needs the same "arming" treatment as EI
            cpu->ime = false;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "di");
            break;
        }
        case 0xFB: {
            cpu->ei_state = ARMED;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "ei");
            break;
        }
        case 0x07: {
            flags->byte = 0;

            uint16_t extended = (uint16_t)cpu->af.high;
            extended <<= 1;
            extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
            cpu->af.high = (uint8_t)(extended & 0xFF);

            flags->z = (extended == 0) ? 1 : 0;
            flags->c = (extended & 0x1) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "rlca");
            break;
        }
        case 0x17: {
            flags->byte = 0;

            uint16_t extended = (uint16_t)cpu->af.high;
            extended |= (uint16_t)(flags->c) << 8;
            extended <<= 1;
            extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

            cpu->af.high = (uint8_t)(extended & 0xFF);
            flags->c = (extended & 0x100) ? 1 : 0;
            flags->z = (extended == 0) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "rla");
            break;
        }
        case 0x0F: {
            flags->byte = 0;

            uint8_t old_bit_0 = cpu->af.high & 0x1;
            cpu->af.high >>= 1;
            cpu->af.high |= (old_bit_0 << 7);

            flags->c = (old_bit_0) ? 1 : 0;
            flags->z = (cpu->af.high == 0) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "rrca");
            break;
        }
        case 0x1F: {
            flags->byte = 0;

            uint16_t extended = (uint16_t)cpu->af.high;
            extended <<= 1;
            extended |= (flags->c) ? 1 : 0;

            uint8_t old_bit_0 = extended & 0x1;
            extended >>= 1;
            extended |= (old_bit_0 << 8);

            cpu->af.high = (extended & 0x1FE) >> 1;
            flags->c = (extended & 0x1) ? 1 : 0;
            flags->z = (extended == 0) ? 1 : 0;

            cpu->result.cycles = 4;
            sprintf(cpu->result.disasm, "rra");
            break;
        }
        case 0xCB: {
            cpu->result.size = 2;
            uint8_t opcode2 = mem_read(mem, cpu->pc);
            cpu->pc++;

            switch (opcode2) {
                case 0x37: {
                    flags->byte = 0;
                    flags->z = (cpu->af.high == 0) ? 1 : 0;

                    uint8_t lower_nibble = (cpu->af.high) & 0xF;
                    uint8_t upper_nibble = ((cpu->af.high) & 0xF0) >> 4;

                    cpu->af.high = (lower_nibble << 4) | upper_nibble;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "swap a");
                    break;
                }
                case 0x30: {
                    flags->byte = 0;
                    flags->z = (cpu->bc.high == 0) ? 1 : 0;

                    uint8_t lower_nibble = (cpu->bc.high) & 0xF;
                    uint8_t upper_nibble = ((cpu->bc.high) & 0xF0) >> 4;

                    cpu->bc.high = (lower_nibble << 4) | upper_nibble;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "swap b");
                    break;
                }
                case 0x31: {
                    flags->byte = 0;
                    flags->z = (cpu->bc.low == 0) ? 1 : 0;

                    uint8_t lower_nibble = (cpu->bc.low) & 0xF;
                    uint8_t upper_nibble = ((cpu->bc.low) & 0xF0) >> 4;

                    cpu->bc.low = (lower_nibble << 4) | upper_nibble;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "swap c");
                    break;
                }
                case 0x32: {
                    flags->byte = 0;
                    flags->z = (cpu->de.high == 0) ? 1 : 0;

                    uint8_t lower_nibble = (cpu->de.high) & 0xF;
                    uint8_t upper_nibble = ((cpu->de.high) & 0xF0) >> 4;

                    cpu->de.high = (lower_nibble << 4) | upper_nibble;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "swap d");
                    break;
                }
                case 0x33: {
                    flags->byte = 0;
                    flags->z = (cpu->de.low == 0) ? 1 : 0;

                    uint8_t lower_nibble = (cpu->de.low) & 0xF;
                    uint8_t upper_nibble = ((cpu->de.low) & 0xF0) >> 4;

                    cpu->de.low = (lower_nibble << 4) | upper_nibble;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "swap e");
                    break;
                }
                case 0x34: {
                    flags->byte = 0;
                    flags->z = (cpu->hl.high == 0) ? 1 : 0;

                    uint8_t lower_nibble = (cpu->hl.high) & 0xF;
                    uint8_t upper_nibble = ((cpu->hl.high) & 0xF0) >> 4;

                    cpu->hl.high = (lower_nibble << 4) | upper_nibble;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "swap h");
                    break;
                }
                case 0x35: {
                    flags->byte = 0;
                    flags->z = (cpu->hl.low == 0) ? 1 : 0;

                    uint8_t lower_nibble = (cpu->hl.low) & 0xF;
                    uint8_t upper_nibble = ((cpu->hl.low) & 0xF0) >> 4;

                    cpu->hl.low = (lower_nibble << 4) | upper_nibble;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "swap l");
                    break;
                }
                case 0x36: {
                    uint8_t value = mem_read(mem, value);
                    flags->byte = 0;
                    flags->z = (value == 0) ? 1 : 0;

                    uint8_t lower_nibble = (value) & 0xF;
                    uint8_t upper_nibble = ((value) & 0xF0) >> 4;

                    value = (lower_nibble << 4) | upper_nibble;
                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "swap [hl]");
                    break;
                }
                case 0x07: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->af.high;
                    extended <<= 1;
                    extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
                    cpu->af.high = (uint8_t)(extended & 0xFF);

                    flags->z = (extended == 0) ? 1 : 0;
                    flags->c = (extended & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rlc a");
                    break;
                }
                case 0x00: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->bc.high;
                    extended <<= 1;
                    extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
                    cpu->bc.high = (uint8_t)(extended & 0xFF);

                    flags->z = (extended == 0) ? 1 : 0;
                    flags->c = (extended & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rlc b");
                    break;
                }
                case 0x01: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->bc.low;
                    extended <<= 1;
                    extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
                    cpu->bc.low = (uint8_t)(extended & 0xFF);

                    flags->z = (extended == 0) ? 1 : 0;
                    flags->c = (extended & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rlc c");
                    break;
                }
                case 0x02: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->de.high;
                    extended <<= 1;
                    extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
                    cpu->de.high = (uint8_t)(extended & 0xFF);

                    flags->z = (extended == 0) ? 1 : 0;
                    flags->c = (extended & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rlc d");
                    break;
                }
                case 0x03: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->de.low;
                    extended <<= 1;
                    extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
                    cpu->de.low = (uint8_t)(extended & 0xFF);

                    flags->z = (extended == 0) ? 1 : 0;
                    flags->c = (extended & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rlc e");
                    break;
                }
                case 0x04: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->hl.high;
                    extended <<= 1;
                    extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
                    cpu->hl.high = (uint8_t)(extended & 0xFF);

                    flags->z = (extended == 0) ? 1 : 0;
                    flags->c = (extended & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rlc h");
                    break;
                }
                case 0x05: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->hl.low;
                    extended <<= 1;
                    extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
                    cpu->hl.low = (uint8_t)(extended & 0xFF);

                    flags->z = (extended == 0) ? 1 : 0;
                    flags->c = (extended & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rlc l");
                    break;
                }
                case 0x06: {
                    flags->byte = 0;
                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint16_t extended = (uint16_t)value;
                    extended <<= 1;
                    extended = (extended & 0xFF) | ((extended & 0x100) >> 8);
                    mem_write(mem, cpu->hl.word, (uint8_t)(extended & 0xFF));

                    flags->z = (extended == 0) ? 1 : 0;
                    flags->c = (extended & 0x1) ? 1 : 0;

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "rlc [hl]");
                    break;
                }
                case 0x17: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->af.high;
                    extended |= (uint16_t)(flags->c) << 8;
                    extended <<= 1;
                    extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

                    cpu->af.high = (uint8_t)(extended & 0xFF);
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rl a");
                    break;
                }
                case 0x10: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->bc.high;
                    extended |= (uint16_t)(flags->c) << 8;
                    extended <<= 1;
                    extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

                    cpu->bc.high = (uint8_t)(extended & 0xFF);
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rl b");
                    break;
                }
                case 0x11: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->bc.low;
                    extended |= (uint16_t)(flags->c) << 8;
                    extended <<= 1;
                    extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

                    cpu->bc.low = (uint8_t)(extended & 0xFF);
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rl c");
                    break;
                }
                case 0x12: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->de.high;
                    extended |= (uint16_t)(flags->c) << 8;
                    extended <<= 1;
                    extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

                    cpu->de.high = (uint8_t)(extended & 0xFF);
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rl d");
                    break;
                }
                case 0x13: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->de.low;
                    extended |= (uint16_t)(flags->c) << 8;
                    extended <<= 1;
                    extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

                    cpu->de.low = (uint8_t)(extended & 0xFF);
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rl e");
                    break;
                }
                case 0x14: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->hl.high;
                    extended |= (uint16_t)(flags->c) << 8;
                    extended <<= 1;
                    extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

                    cpu->hl.high = (uint8_t)(extended & 0xFF);
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rl h");
                    break;
                }
                case 0x15: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->hl.low;
                    extended |= (uint16_t)(flags->c) << 8;
                    extended <<= 1;
                    extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

                    cpu->hl.low = (uint8_t)(extended & 0xFF);
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rl l");
                    break;
                }
                case 0x16: {
                    flags->byte = 0;

                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint16_t extended = (uint16_t)value;
                    extended |= (uint16_t)(flags->c) << 8;
                    extended <<= 1;
                    extended = (extended & 0x1FF) | ((extended & 0x200) >> 9);

                    value = (uint8_t)(extended & 0xFF);
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "rl [hl]");
                    break;
                }
                case 0x0F: {
                    flags->byte = 0;

                    uint8_t old_bit_0 = cpu->af.high & 0x1;
                    cpu->af.high >>= 1;
                    cpu->af.high |= (old_bit_0 << 7);

                    flags->c = (old_bit_0) ? 1 : 0;
                    flags->z = (cpu->af.high == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rrc a");
                    break;
                }
                case 0x08: {
                    flags->byte = 0;

                    uint8_t old_bit_0 = cpu->bc.high & 0x1;
                    cpu->bc.high >>= 1;
                    cpu->bc.high |= (old_bit_0 << 7);

                    flags->c = (old_bit_0) ? 1 : 0;
                    flags->z = (cpu->bc.high == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rrc b");
                    break;
                }
                case 0x09: {
                    flags->byte = 0;

                    uint8_t old_bit_0 = cpu->bc.low & 0x1;
                    cpu->bc.low >>= 1;
                    cpu->bc.low |= (old_bit_0 << 7);

                    flags->c = (old_bit_0) ? 1 : 0;
                    flags->z = (cpu->bc.low == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rrc c");
                    break;
                }
                case 0x0A: {
                    flags->byte = 0;

                    uint8_t old_bit_0 = cpu->de.high & 0x1;
                    cpu->de.high >>= 1;
                    cpu->de.high |= (old_bit_0 << 7);

                    flags->c = (old_bit_0) ? 1 : 0;
                    flags->z = (cpu->de.high == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rrc d");
                    break;
                }
                case 0x0B: {
                    flags->byte = 0;

                    uint8_t old_bit_0 = cpu->de.low & 0x1;
                    cpu->de.low >>= 1;
                    cpu->de.low |= (old_bit_0 << 7);

                    flags->c = (old_bit_0) ? 1 : 0;
                    flags->z = (cpu->de.low == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rrc e");
                    break;
                }
                case 0x0C: {
                    flags->byte = 0;

                    uint8_t old_bit_0 = cpu->hl.high & 0x1;
                    cpu->hl.high >>= 1;
                    cpu->hl.high |= (old_bit_0 << 7);

                    flags->c = (old_bit_0) ? 1 : 0;
                    flags->z = (cpu->hl.high == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rrc h");
                    break;
                }
                case 0x0D: {
                    flags->byte = 0;


                    uint8_t old_bit_0 = cpu->hl.low & 0x1;
                    cpu->hl.low >>= 1;
                    cpu->hl.low |= (old_bit_0 << 7);

                    flags->c = (old_bit_0) ? 1 : 0;
                    flags->z = (cpu->hl.low == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rrc l");
                    break;
                }
                case 0x0E: {
                    flags->byte = 0;

                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint8_t old_bit_0 = value & 0x1;
                    value >>= 1;
                    value |= (old_bit_0 << 7);

                    flags->c = (old_bit_0) ? 1 : 0;
                    flags->z = (value == 0) ? 1 : 0;

                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "rrc [hl]");
                    break;
                }
                case 0x1F: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->af.high;
                    extended <<= 1;
                    extended |= (flags->c) ? 1 : 0;

                    uint8_t old_bit_0 = extended & 0x1;
                    extended >>= 1;
                    extended |= (old_bit_0 << 8);

                    cpu->af.high = (extended & 0x1FE) >> 1;
                    flags->c = (extended & 0x1) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rr a");
                    break;
                }
                case 0x18: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->bc.high;
                    extended <<= 1;
                    extended |= (flags->c) ? 1 : 0;

                    uint8_t old_bit_0 = extended & 0x1;
                    extended >>= 1;
                    extended |= (old_bit_0 << 8);

                    cpu->bc.high = (extended & 0x1FE) >> 1;
                    flags->c = (extended & 0x1) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rr b");
                    break;
                }
                case 0x19: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->bc.low;
                    extended <<= 1;
                    extended |= (flags->c) ? 1 : 0;

                    uint8_t old_bit_0 = extended & 0x1;
                    extended >>= 1;
                    extended |= (old_bit_0 << 8);

                    cpu->bc.low = (extended & 0x1FE) >> 1;
                    flags->c = (extended & 0x1) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rr c");
                    break;
                }
                case 0x1A: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->de.high;
                    extended <<= 1;
                    extended |= (flags->c) ? 1 : 0;

                    uint8_t old_bit_0 = extended & 0x1;
                    extended >>= 1;
                    extended |= (old_bit_0 << 8);

                    cpu->de.high = (extended & 0x1FE) >> 1;
                    flags->c = (extended & 0x1) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rr d");
                    break;
                }
                case 0x1B: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->de.low;
                    extended <<= 1;
                    extended |= (flags->c) ? 1 : 0;

                    uint8_t old_bit_0 = extended & 0x1;
                    extended >>= 1;
                    extended |= (old_bit_0 << 8);

                    cpu->de.low = (extended & 0x1FE) >> 1;
                    flags->c = (extended & 0x1) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rr e");
                    break;
                }
                case 0x1C: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->hl.high;
                    extended <<= 1;
                    extended |= (flags->c) ? 1 : 0;

                    uint8_t old_bit_0 = extended & 0x1;
                    extended >>= 1;
                    extended |= (old_bit_0 << 8);

                    cpu->hl.high = (extended & 0x1FE) >> 1;
                    flags->c = (extended & 0x1) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rr h");
                    break;
                }
                case 0x1D: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->hl.low;
                    extended <<= 1;
                    extended |= (flags->c) ? 1 : 0;

                    uint8_t old_bit_0 = extended & 0x1;
                    extended >>= 1;
                    extended |= (old_bit_0 << 8);

                    cpu->hl.low = (extended & 0x1FE) >> 1;
                    flags->c = (extended & 0x1) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "rr l");
                    break;
                }
                case 0x1E: {
                    flags->byte = 0;

                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint16_t extended = (uint16_t)value;
                    extended <<= 1;
                    extended |= (flags->c) ? 1 : 0;

                    uint8_t old_bit_0 = extended & 0x1;
                    extended >>= 1;
                    extended |= (old_bit_0 << 8);

                    value = (extended & 0x1FE) >> 1;
                    flags->c = (extended & 0x1) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "rr [hl]");
                    break;
                }
                case 0x27: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->af.high;
                    extended <<= 1;

                    cpu->af.high = extended & 0xFF;
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sla a");
                    break;
                }
                case 0x20: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->bc.high;
                    extended <<= 1;

                    cpu->bc.high = extended & 0xFF;
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sla b");
                    break;
                }
                case 0x21: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->bc.low;
                    extended <<= 1;

                    cpu->bc.low = extended & 0xFF;
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sla c");
                    break;
                }
                case 0x22: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->de.high;
                    extended <<= 1;

                    cpu->de.high = extended & 0xFF;
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sla d");
                    break;
                }
                case 0x23: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->de.low;
                    extended <<= 1;

                    cpu->de.low = extended & 0xFF;
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sla e");
                    break;
                }
                case 0x24: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->hl.high;
                    extended <<= 1;

                    cpu->hl.high = extended & 0xFF;
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sla h");
                    break;
                }
                case 0x25: {
                    flags->byte = 0;

                    uint16_t extended = (uint16_t)cpu->hl.low;
                    extended <<= 1;

                    cpu->hl.low = extended & 0xFF;
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sla l");
                    break;
                }
                case 0x26: {
                    flags->byte = 0;

                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint16_t extended = (uint16_t)value;
                    extended <<= 1;

                    value = extended & 0xFF;
                    flags->c = (extended & 0x100) ? 1 : 0;
                    flags->z = (extended == 0) ? 1 : 0;

                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sla [hl]");
                    break;
                }
                case 0x2F: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->af.high & 0x80;
                    uint8_t old_bit_0 = cpu->af.high & 0x1;
                    cpu->af.high >>= 1;
                    cpu->af.high |= old_bit_7;

                    flags->z = (cpu->af.high == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sra a");
                    break;
                }
                case 0x28: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->bc.high & 0x80;
                    uint8_t old_bit_0 = cpu->bc.high & 0x1;
                    cpu->bc.high >>= 1;
                    cpu->bc.high |= old_bit_7;

                    flags->z = (cpu->bc.high == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sra b");
                    break;
                }
                case 0x29: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->bc.low & 0x80;
                    uint8_t old_bit_0 = cpu->bc.low & 0x1;
                    cpu->bc.low >>= 1;
                    cpu->bc.low |= old_bit_7;

                    flags->z = (cpu->bc.low == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sra c");
                    break;
                }
                case 0x2A: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->de.high & 0x80;
                    uint8_t old_bit_0 = cpu->de.high & 0x1;
                    cpu->de.high >>= 1;
                    cpu->de.high |= old_bit_7;

                    flags->z = (cpu->de.high == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sra d");
                    break;
                }
                case 0x2B: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->de.low & 0x80;
                    uint8_t old_bit_0 = cpu->de.low & 0x1;
                    cpu->de.low >>= 1;
                    cpu->de.low |= old_bit_7;

                    flags->z = (cpu->de.low == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sra e");
                    break;
                }
                case 0x2C: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->hl.high & 0x80;
                    uint8_t old_bit_0 = cpu->hl.high & 0x1;
                    cpu->hl.high >>= 1;
                    cpu->hl.high |= old_bit_7;

                    flags->z = (cpu->hl.high == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sra h");
                    break;
                }
                case 0x2D: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->hl.low & 0x80;
                    uint8_t old_bit_0 = cpu->hl.low & 0x1;
                    cpu->hl.low >>= 1;
                    cpu->hl.low |= old_bit_7;

                    flags->z = (cpu->hl.low == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "sra l");
                    break;
                }
                case 0x2E: {
                    flags->byte = 0;

                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint8_t old_bit_7 = value & 0x80;
                    uint8_t old_bit_0 = value & 0x1;
                    value >>= 1;
                    value |= old_bit_7;

                    flags->z = (value == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "sra [hl]");
                    break;
                }
                case 0x3F: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->af.high & 0x80;
                    uint8_t old_bit_0 = cpu->af.high & 0x1;
                    cpu->af.high >>= 1;

                    flags->z = (cpu->af.high == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "srl a");
                    break;
                }
                case 0x38: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->bc.high & 0x80;
                    uint8_t old_bit_0 = cpu->bc.high & 0x1;
                    cpu->bc.high >>= 1;

                    flags->z = (cpu->bc.high == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "srl b");
                    break;
                }
                case 0x39: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->bc.low & 0x80;
                    uint8_t old_bit_0 = cpu->bc.low & 0x1;
                    cpu->bc.low >>= 1;

                    flags->z = (cpu->bc.low == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "srl c");
                    break;
                }
                case 0x3A: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->de.high & 0x80;
                    uint8_t old_bit_0 = cpu->de.high & 0x1;
                    cpu->de.high >>= 1;

                    flags->z = (cpu->de.high == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "srl d");
                    break;
                }
                case 0x3B: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->de.low & 0x80;
                    uint8_t old_bit_0 = cpu->de.low & 0x1;
                    cpu->de.low >>= 1;

                    flags->z = (cpu->de.low == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "srl e");
                    break;
                }
                case 0x3C: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->hl.high & 0x80;
                    uint8_t old_bit_0 = cpu->hl.high & 0x1;
                    cpu->hl.high >>= 1;

                    flags->z = (cpu->hl.high == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "srl h");
                    break;
                }
                case 0x3D: {
                    flags->byte = 0;

                    uint8_t old_bit_7 = cpu->hl.low & 0x80;
                    uint8_t old_bit_0 = cpu->hl.low & 0x1;
                    cpu->hl.low >>= 1;

                    flags->z = (cpu->hl.low == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "srl l");
                    break;
                }
                case 0x3E: {
                    flags->byte = 0;

                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint8_t old_bit_7 = value & 0x80;
                    uint8_t old_bit_0 = value & 0x1;
                    value >>= 1;

                    flags->z = (value == 0) ? 1 : 0;
                    flags->c = (old_bit_0) ? 1 : 0;

                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "srl [hl]");
                    break;
                }
                case 0x47: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    flags->byte &= 0x10;
                    flags->h = true;

                    flags->z = ((cpu->af.high >> bit) & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "bit %d, a", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x40: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    flags->byte &= 0x10;
                    flags->h = true;

                    flags->z = ((cpu->bc.high >> bit) & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "bit %d, b", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x41: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    flags->byte &= 0x10;
                    flags->h = true;

                    flags->z = ((cpu->bc.low >> bit) & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "bit %d, c", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x42: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    flags->byte &= 0x10;
                    flags->h = true;

                    flags->z = ((cpu->de.high >> bit) & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "bit %d, d", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x43: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    flags->byte &= 0x10;
                    flags->h = true;

                    flags->z = ((cpu->de.low >> bit) & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "bit %d, e", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x44: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    flags->byte &= 0x10;
                    flags->h = true;

                    flags->z = ((cpu->hl.high >> bit) & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "bit %d, h", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x45: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    flags->byte &= 0x10;
                    flags->h = true;

                    flags->z = ((cpu->hl.low >> bit) & 0x1) ? 1 : 0;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "bit %d, l", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x46: {
                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    flags->byte &= 0x10;
                    flags->h = true;

                    flags->z = ((value >> bit) & 0x1) ? 1 : 0;

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "bit %d, [hl]", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0xC7: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->af.high |= (1 << bit);

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "set %d, a", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0xC0: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->bc.high |= (1 << bit);

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "set %d, b", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0xC1: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->bc.low |= (1 << bit);

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "set %d, c", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0xC2: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->de.high |= (1 << bit);

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "set %d, d", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0xC3: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->de.low |= (1 << bit);

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "set %d, e", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0xC4: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->hl.high |= (1 << bit);

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "set %d, h", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0xC5: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->hl.low |= (1 << bit);

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "set %d, l", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0xC6: {
                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    value |= (1 << bit);
                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "set %d, [hl]", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x87: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->af.high &= (~(1 << bit)) & 0xFF;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "res %d, a", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x80: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->bc.high &= (~(1 << bit)) & 0xFF;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "res %d, b", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x81: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->bc.low &= (~(1 << bit)) & 0xFF;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "res %d, c", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x82: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->de.high &= (~(1 << bit)) & 0xFF;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "res %d, d", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x83: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->de.low &= (~(1 << bit)) & 0xFF;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "res %d, e", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x84: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->hl.high &= (~(1 << bit)) & 0xFF;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "res %d, h", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x85: {
                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    cpu->hl.low &= (~(1 << bit)) & 0xFF;

                    cpu->result.cycles = 8;
                    sprintf(cpu->result.disasm, "res %d, l", bit);
                    cpu->result.size = 3;
                    break;
                }
                case 0x86: {
                    uint8_t value = mem_read(mem, cpu->hl.word);

                    uint8_t bit = mem_read(mem, cpu->pc) & 0x7;
                    cpu->pc++;

                    value &= (~(1 << bit)) & 0xFF;
                    mem_write(mem, cpu->hl.word, value);

                    cpu->result.cycles = 16;
                    sprintf(cpu->result.disasm, "res %d, [hl]", bit);
                    cpu->result.size = 3;
                    break;
                }
                default: {
                    sprintf(cpu->result.disasm, "unknown opcode 0x%X 0x%X", opcode, opcode2);
                    break;
                }
            }
        }
        default: {
            sprintf(cpu->result.disasm, "unknown opcode 0x%X", opcode);
            break;
        }
    }
}
