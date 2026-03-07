#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void _cpu_synchronize(uint8_t cycles) {
    // TODO: Implement
}

uint8_t _cpu_read_imm8(cpu_state_t* cpu, memory_t* mem) {
    uint8_t value = mem_read(mem, cpu->pc);
    cpu->pc++;
    _cpu_synchronize(4);
    return value;
}

uint16_t _cpu_read_imm16(cpu_state_t* cpu, memory_t* mem) {
    uint8_t value_low_byte = mem_read(mem, cpu->pc);
    cpu->pc++;
    _cpu_synchronize(4);
    uint8_t value_high_byte = mem_read(mem, cpu->pc);
    cpu->pc++;
    _cpu_synchronize(4);
    uint16_t value = (value_low_byte) | (value_high_byte << 8);
    return value;
}


uint8_t _cpu_read_hl_addr(cpu_state_t* cpu, memory_t* mem) {
    uint8_t value = mem_read(mem, cpu->hl.word);
    _cpu_synchronize(4);
    return value;
}

void _cpu_write_hl_addr(cpu_state_t* cpu, memory_t* mem, uint8_t value) {
    mem_write(mem, cpu->hl.word, value);
    _cpu_synchronize(4);
}

void _cpu_instr_ld8(uint8_t* dest, uint8_t source) {
    *dest = source;
    _cpu_synchronize(4);
}

void cpu_step(cpu_state_t* cpu, memory_t* mem) {
    uint8_t opcode = mem_read(mem, cpu->pc);
    cpu->pc++;

    switch (opcode) {
        case 0x06: {
            _cpu_instr_ld8(&(cpu->bc.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x0E: {
            _cpu_instr_ld8(&(cpu->bc.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x16: {
            _cpu_instr_ld8(&(cpu->de.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x1E: {
            _cpu_instr_ld8(&(cpu->de.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x26: {
            _cpu_instr_ld8(&(cpu->hl.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x2E: {
            _cpu_instr_ld8(&(cpu->hl.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x7F: {
            _cpu_instr_ld8(&(cpu->af.high), cpu->af.high);
            break;
        }
        case 0x78: {
            _cpu_instr_ld8(&(cpu->af.high), cpu->bc.high);
            break;
        }
        case 0x79: {
            _cpu_instr_ld8(&(cpu->af.high), cpu->bc.low);
            break;
        }
        case 0x7A: {
            _cpu_instr_ld8(&(cpu->af.high), cpu->de.high);
            break;
        }
        case 0x7B: {
            _cpu_instr_ld8(&(cpu->af.high), cpu->de.low);
            break;
        }
        case 0x7C: {
            _cpu_instr_ld8(&(cpu->af.high), cpu->hl.high);
            break;
        }
        case 0x7D: {
            _cpu_instr_ld8(&(cpu->af.high), cpu->hl.low);
            break;
        }
        case 0x7E: {
            _cpu_instr_ld8(&(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x40: {
            _cpu_instr_ld8(&(cpu->bc.high), cpu->bc.high);
            break;
        }
        case 0x41: {
            _cpu_instr_ld8(&(cpu->bc.high), cpu->bc.low);
            break;
        }
        case 0x42: {
            _cpu_instr_ld8(&(cpu->bc.high), cpu->de.high);
            break;
        }
        case 0x43: {
            _cpu_instr_ld8(&(cpu->bc.high), cpu->de.low);
            break;
        }
        case 0x44: {
            _cpu_instr_ld8(&(cpu->bc.high), cpu->hl.high);
            break;
        }
        case 0x45: {
            _cpu_instr_ld8(&(cpu->bc.high), cpu->hl.low);
            break;
        }
        case 0x46: {
            _cpu_instr_ld8(&(cpu->bc.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x47: {
            _cpu_instr_ld8(&(cpu->bc.high), cpu->af.high);
            break;
        }
        case 0x48: {
            _cpu_instr_ld8(&(cpu->bc.low), cpu->bc.high);
            break;
        }
        case 0x49: {
            _cpu_instr_ld8(&(cpu->bc.low), cpu->bc.low);
            break;
        }
        case 0x4A: {
            _cpu_instr_ld8(&(cpu->bc.low), cpu->de.high);
            break;
        }
        case 0x4B: {
            _cpu_instr_ld8(&(cpu->bc.low), cpu->de.low);
            break;
        }
        case 0x4C: {
            _cpu_instr_ld8(&(cpu->bc.low), cpu->hl.high);
            break;
        }
        case 0x4D: {
            _cpu_instr_ld8(&(cpu->bc.low), cpu->hl.low);
            break;
        }
        case 0x4E: {
            _cpu_instr_ld8(&(cpu->bc.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x4F: {
            _cpu_instr_ld8(&(cpu->bc.low), cpu->af.high);
            break;
        }
        case 0x50: {
            _cpu_instr_ld8(&(cpu->de.high), cpu->bc.high);
            break;
        }
        case 0x51: {
            _cpu_instr_ld8(&(cpu->de.high), cpu->bc.low);
            break;
        }
        case 0x52: {
            _cpu_instr_ld8(&(cpu->de.high), cpu->de.high);
            break;
        }
        case 0x53: {
            _cpu_instr_ld8(&(cpu->de.high), cpu->de.low);
            break;
        }
        case 0x54: {
            _cpu_instr_ld8(&(cpu->de.high), cpu->hl.high);
            break;
        }
        case 0x55: {
            _cpu_instr_ld8(&(cpu->de.high), cpu->hl.low);
            break;
        }
        case 0x56: {
            _cpu_instr_ld8(&(cpu->de.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x57: {
            _cpu_instr_ld8(&(cpu->de.high), cpu->af.high);
            break;
        }
        case 0x58: {
            _cpu_instr_ld8(&(cpu->de.low), cpu->bc.high);
            break;
        }
        case 0x59: {
            _cpu_instr_ld8(&(cpu->de.low), cpu->bc.low);
            break;
        }
        case 0x5A: {
            _cpu_instr_ld8(&(cpu->de.low), cpu->de.high);
            break;
        }
        case 0x5B: {
            _cpu_instr_ld8(&(cpu->de.low), cpu->de.low);
            break;
        }
        case 0x5C: {
            _cpu_instr_ld8(&(cpu->de.low), cpu->hl.high);
            break;
        }
        case 0x5D: {
            _cpu_instr_ld8(&(cpu->de.low), cpu->hl.low);
            break;
        }
        case 0x5E: {
            _cpu_instr_ld8(&(cpu->de.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x5F: {
            _cpu_instr_ld8(&(cpu->de.low), cpu->af.high);
            break;
        }
        case 0x60: {
            _cpu_instr_ld8(&(cpu->hl.high), cpu->bc.high);
            break;
        }
        case 0x61: {
            _cpu_instr_ld8(&(cpu->hl.high), cpu->bc.low);
            break;
        }
        case 0x62: {
            _cpu_instr_ld8(&(cpu->hl.high), cpu->de.high);
            break;
        }
        case 0x63: {
            _cpu_instr_ld8(&(cpu->hl.high), cpu->de.low);
            break;
        }
        case 0x64: {
            _cpu_instr_ld8(&(cpu->hl.high), cpu->hl.high);
            break;
        }
        case 0x65: {
            _cpu_instr_ld8(&(cpu->hl.high), cpu->hl.low);
            break;
        }
        case 0x66: {
            _cpu_instr_ld8(&(cpu->hl.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x67: {
            _cpu_instr_ld8(&(cpu->hl.high), cpu->af.high);
            break;
        }
        case 0x68: {
            _cpu_instr_ld8(&(cpu->hl.low), cpu->bc.high);
            break;
        }
        case 0x69: {
            _cpu_instr_ld8(&(cpu->hl.low), cpu->bc.low);
            break;
        }
        case 0x6A: {
            _cpu_instr_ld8(&(cpu->hl.low), cpu->de.high);
            break;
        }
        case 0x6B: {
            _cpu_instr_ld8(&(cpu->hl.low), cpu->de.low);
            break;
        }
        case 0x6C: {
            _cpu_instr_ld8(&(cpu->hl.low), cpu->hl.high);
            break;
        }
        case 0x6D: {
            _cpu_instr_ld8(&(cpu->hl.low), cpu->hl.low);
            break;
        }
        case 0x6E: {
            _cpu_instr_ld8(&(cpu->hl.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x6F: {
            _cpu_instr_ld8(&(cpu->hl.low), cpu->af.high);
            break;
        }
        case 0x70: {
            _cpu_write_hl_addr(cpu, mem, cpu->bc.high);
            _cpu_synchronize(4);
            break;
        }
        case 0x71: {
            _cpu_write_hl_addr(cpu, mem, cpu->bc.low);
            _cpu_synchronize(4);
            break;
        }
        case 0x72: {
            _cpu_write_hl_addr(cpu, mem, cpu->de.high);
            _cpu_synchronize(4);
            break;
        }
        case 0x73: {
            _cpu_write_hl_addr(cpu, mem, cpu->de.low);
            _cpu_synchronize(4);
            break;
        }
        case 0x74: {
            _cpu_write_hl_addr(cpu, mem, cpu->hl.high);
            _cpu_synchronize(4);
            break;
        }
        case 0x75: {
            _cpu_write_hl_addr(cpu, mem, cpu->hl.low);
            _cpu_synchronize(4);
            break;
        }
        case 0x77: {
            _cpu_write_hl_addr(cpu, mem, cpu->af.high);
            _cpu_synchronize(4);
            break;
        }
        case 0x36: {
            _cpu_write_hl_addr(cpu, mem, _cpu_read_imm8(cpu, mem));
            _cpu_synchronize(4);
            break;
        }
        case 0x0A: {
            _cpu_instr_ld8(&(cpu->af.high), mem_read(mem, cpu->bc.word));
            _cpu_synchronize(4);
            break;
        }
        case 0x1A: {
            _cpu_instr_ld8(&(cpu->af.high), mem_read(mem, cpu->de.word));
            _cpu_synchronize(4);
            break;
        }
        case 0xFA: {
            _cpu_instr_ld8(&(cpu->af.high), mem_read(mem, _cpu_read_imm16(cpu, mem)));
            _cpu_synchronize(4);
            break;
        }
        case 0x3E: {
            _cpu_instr_ld8(&(cpu->af.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x02: {
            mem_write(mem, cpu->bc.word, cpu->af.high);
            _cpu_synchronize(8);
            break;
        }
        case 0x12: {
            mem_write(mem, cpu->de.word, cpu->af.high);
            _cpu_synchronize(8);
            break;
        }
        case 0xEA: {
            mem_write(mem, _cpu_read_imm16(cpu, mem), cpu->af.high);
            _cpu_synchronize(4);
            break;
        }
        case 0xF2: {
            _cpu_instr_ld8(&(cpu->af.high), mem_read(mem, 0xFF00 + cpu->bc.low));
            _cpu_synchronize(4);
            break;
        }
        case 0xE2: {
            mem_write(mem, 0xFF00 + cpu->bc.low, cpu->af.high);
            _cpu_synchronize(8);
        }
        case 0x3A: {
            _cpu_instr_ld8(&(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
            cpu->hl.word--;
            break;
        }
        case 0x32: {
            _cpu_write_hl_addr(cpu, mem, cpu->af.high);
            _cpu_synchronize(4);
            cpu->hl.word--;
            break;
        }
        case 0x2A: {
            _cpu_instr_ld8(&(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
            cpu->hl.word++;
            break;
        }
        case 0x22: {
            _cpu_write_hl_addr(cpu, mem, cpu->af.high);
            _cpu_synchronize(4);
            cpu->hl.word++;
            break;
        }
        case 0xE0: {
            mem_write(mem, 0xFF00 + _cpu_read_imm8(cpu, mem), cpu->af.high);
            _cpu_synchronize(8);
            break;
        }
        case 0xF0: {
            _cpu_instr_ld8(&(cpu->af.high), mem_read(mem, 0xFF00 + _cpu_read_imm8(cpu, mem)));
            _cpu_synchronize(4);
            break;
        }
    }
}
