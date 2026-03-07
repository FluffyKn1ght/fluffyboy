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
    cpu->last_imm8 = value;
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
    cpu->last_imm16 = value;
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

void _cpu_load8(uint8_t* dest, uint8_t source) {
    *dest = source;
    _cpu_synchronize(4);
}

void _cpu_load16(uint16_t* dest, uint16_t source) {
    *dest = source;
    _cpu_synchronize(4);
}

void _cpu_stack_push(cpu_state_t* cpu, memory_t* mem, uint16_t value) {
    mem_write(mem, cpu->sp, value & 0xFF);
    _cpu_synchronize(4);
    cpu->sp--;
    mem_write(mem, cpu->sp, value & 0xFF00);
    _cpu_synchronize(8);
    cpu->sp--;
}

uint16_t _cpu_stack_pop(cpu_state_t* cpu, memory_t* mem) {
    uint16_t value = mem_readw(mem, cpu->sp);
    cpu->sp += 2;
    _cpu_synchronize(8);
    return value;
}

void _cpu_add8(cpu_state_t* cpu, uint8_t* operand_a, uint8_t* operand_b, bool with_carry) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;

    uint8_t operand_c = (with_carry && flags->c) ? 1 : 0;

    uint16_t full_result = *operand_a + *operand_b + operand_c;
    uint8_t partial_result = (*operand_a & 0xF) + (*operand_b & 0xF) + operand_c;

    if (full_result == 0) {
        flags->z = 1;
    }

    if (full_result > 0xFF) {
        flags->c = 0;
    }
    if (partial_result > 0xF) {
        flags->h = 0;
    }

    *operand_a = (uint8_t)(full_result & 0xFF);
    _cpu_synchronize(4);
}

void _cpu_sub8(cpu_state_t* cpu, uint8_t* operand_a, uint8_t* operand_b, bool with_carry) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;
    flags->n = 1;

    int8_t operand_c = (with_carry && flags->c) ? 1 : 0;

    uint16_t full_result = *operand_a - *operand_b - operand_c;
    int16_t full_signed_result = *operand_a - *operand_b - operand_c;
    int8_t partial_result = (*operand_a & 0xF) - (*operand_b & 0xF) - operand_c;

    if (!(full_signed_result < 0)) {
        flags->c = 1;
    }
    if (!(partial_result < 0)) {
        flags->h = 1;
    }

    if (full_result == 0) {
        flags->z = 1;
    }

    *operand_a = (uint8_t)(full_result & 0xFF);
    _cpu_synchronize(4);
}

void _cpu_and8(cpu_state_t* cpu, uint8_t* operand_a, uint8_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;
    flags->h = 1;

    uint8_t result = *operand_a & *operand_b;
    if (result == 0) {
        flags->z = 1;
    }

    *operand_a = result;
    _cpu_synchronize(4);
}

void _cpu_or8(cpu_state_t* cpu, uint8_t* operand_a, uint8_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;

    uint8_t result = *operand_a | *operand_b;
    if (result == 0) {
        flags->z = 1;
    }

    *operand_a = result;
    _cpu_synchronize(4);
}

void _cpu_xor8(cpu_state_t* cpu, uint8_t* operand_a, uint8_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;

    uint8_t result = *operand_a ^ *operand_b;
    if (result == 0) {
        flags->z = 1;
    }

    *operand_a = result;
    _cpu_synchronize(4);
}

void _cpu_cp8(cpu_state_t* cpu, uint8_t* operand_a, uint8_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;
    flags->n = 1;

    flags->h = ((*operand_a & 0xF) < (*operand_b & 0xF)) ? 0 : 1;
    flags->c = (*operand_a < *operand_b) ? 0 : 1;
    flags->z = (*operand_a == *operand_b) ? 1 : 0;

    _cpu_synchronize(4);
}

void _cpu_inc8(cpu_state_t* cpu, uint8_t* what) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte &= 0x10;

    uint16_t full_result = *what + 1;
    uint16_t partial_result = (*what & 0xF) + 1;

    if (partial_result > 0xF) {
        flags->h = 1;
    }
    if ((full_result & 0xFF) == 0) {
        flags->z = 1;
    }

    *what = (full_result & 0xFF);
    _cpu_synchronize(4);
}

void _cpu_dec8(cpu_state_t* cpu, uint8_t* what) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte &= 0x10;
    flags->n = 1;

    int16_t full_result = *what - 1;
    int8_t partial_result = (*what & 0xF) - 1;

    if (!(partial_result < 0)) {
        flags->h = 1;
    }
    if ((full_result & 0xFF) == 0) {
        flags->z = 1;
    }

    *what = (full_result & 0xFF);
    _cpu_synchronize(4);
}

void _cpu_add16(cpu_state_t* cpu, uint16_t* operand_a, uint16_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte &= 0x80;

    uint32_t partial_result = ((*operand_a & 0xFFF) + (*operand_b & 0xFFF));
    uint32_t full_result = *operand_a + *operand_b;

    if (partial_result > 0xFFF) {
        flags->h = 1;
    }
    if (full_result > 0xFFFF) {
        flags->c = 1;
    }

    *operand_a = (uint16_t)(*operand_a & 0xFFFF);
    _cpu_synchronize(8);
}

void cpu_step(cpu_state_t* cpu, memory_t* mem) {
    uint8_t opcode = mem_read(mem, cpu->pc);
    cpu->pc++;

    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);

    switch (opcode) {
        case 0x06: {
            _cpu_load8(&(cpu->bc.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x0E: {
            _cpu_load8(&(cpu->bc.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x16: {
            _cpu_load8(&(cpu->de.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x1E: {
            _cpu_load8(&(cpu->de.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x26: {
            _cpu_load8(&(cpu->hl.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x2E: {
            _cpu_load8(&(cpu->hl.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x7F: {
            _cpu_load8(&(cpu->af.high), cpu->af.high);
            break;
        }
        case 0x78: {
            _cpu_load8(&(cpu->af.high), cpu->bc.high);
            break;
        }
        case 0x79: {
            _cpu_load8(&(cpu->af.high), cpu->bc.low);
            break;
        }
        case 0x7A: {
            _cpu_load8(&(cpu->af.high), cpu->de.high);
            break;
        }
        case 0x7B: {
            _cpu_load8(&(cpu->af.high), cpu->de.low);
            break;
        }
        case 0x7C: {
            _cpu_load8(&(cpu->af.high), cpu->hl.high);
            break;
        }
        case 0x7D: {
            _cpu_load8(&(cpu->af.high), cpu->hl.low);
            break;
        }
        case 0x7E: {
            _cpu_load8(&(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x40: {
            _cpu_load8(&(cpu->bc.high), cpu->bc.high);
            break;
        }
        case 0x41: {
            _cpu_load8(&(cpu->bc.high), cpu->bc.low);
            break;
        }
        case 0x42: {
            _cpu_load8(&(cpu->bc.high), cpu->de.high);
            break;
        }
        case 0x43: {
            _cpu_load8(&(cpu->bc.high), cpu->de.low);
            break;
        }
        case 0x44: {
            _cpu_load8(&(cpu->bc.high), cpu->hl.high);
            break;
        }
        case 0x45: {
            _cpu_load8(&(cpu->bc.high), cpu->hl.low);
            break;
        }
        case 0x46: {
            _cpu_load8(&(cpu->bc.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x47: {
            _cpu_load8(&(cpu->bc.high), cpu->af.high);
            break;
        }
        case 0x48: {
            _cpu_load8(&(cpu->bc.low), cpu->bc.high);
            break;
        }
        case 0x49: {
            _cpu_load8(&(cpu->bc.low), cpu->bc.low);
            break;
        }
        case 0x4A: {
            _cpu_load8(&(cpu->bc.low), cpu->de.high);
            break;
        }
        case 0x4B: {
            _cpu_load8(&(cpu->bc.low), cpu->de.low);
            break;
        }
        case 0x4C: {
            _cpu_load8(&(cpu->bc.low), cpu->hl.high);
            break;
        }
        case 0x4D: {
            _cpu_load8(&(cpu->bc.low), cpu->hl.low);
            break;
        }
        case 0x4E: {
            _cpu_load8(&(cpu->bc.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x4F: {
            _cpu_load8(&(cpu->bc.low), cpu->af.high);
            break;
        }
        case 0x50: {
            _cpu_load8(&(cpu->de.high), cpu->bc.high);
            break;
        }
        case 0x51: {
            _cpu_load8(&(cpu->de.high), cpu->bc.low);
            break;
        }
        case 0x52: {
            _cpu_load8(&(cpu->de.high), cpu->de.high);
            break;
        }
        case 0x53: {
            _cpu_load8(&(cpu->de.high), cpu->de.low);
            break;
        }
        case 0x54: {
            _cpu_load8(&(cpu->de.high), cpu->hl.high);
            break;
        }
        case 0x55: {
            _cpu_load8(&(cpu->de.high), cpu->hl.low);
            break;
        }
        case 0x56: {
            _cpu_load8(&(cpu->de.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x57: {
            _cpu_load8(&(cpu->de.high), cpu->af.high);
            break;
        }
        case 0x58: {
            _cpu_load8(&(cpu->de.low), cpu->bc.high);
            break;
        }
        case 0x59: {
            _cpu_load8(&(cpu->de.low), cpu->bc.low);
            break;
        }
        case 0x5A: {
            _cpu_load8(&(cpu->de.low), cpu->de.high);
            break;
        }
        case 0x5B: {
            _cpu_load8(&(cpu->de.low), cpu->de.low);
            break;
        }
        case 0x5C: {
            _cpu_load8(&(cpu->de.low), cpu->hl.high);
            break;
        }
        case 0x5D: {
            _cpu_load8(&(cpu->de.low), cpu->hl.low);
            break;
        }
        case 0x5E: {
            _cpu_load8(&(cpu->de.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x5F: {
            _cpu_load8(&(cpu->de.low), cpu->af.high);
            break;
        }
        case 0x60: {
            _cpu_load8(&(cpu->hl.high), cpu->bc.high);
            break;
        }
        case 0x61: {
            _cpu_load8(&(cpu->hl.high), cpu->bc.low);
            break;
        }
        case 0x62: {
            _cpu_load8(&(cpu->hl.high), cpu->de.high);
            break;
        }
        case 0x63: {
            _cpu_load8(&(cpu->hl.high), cpu->de.low);
            break;
        }
        case 0x64: {
            _cpu_load8(&(cpu->hl.high), cpu->hl.high);
            break;
        }
        case 0x65: {
            _cpu_load8(&(cpu->hl.high), cpu->hl.low);
            break;
        }
        case 0x66: {
            _cpu_load8(&(cpu->hl.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x67: {
            _cpu_load8(&(cpu->hl.high), cpu->af.high);
            break;
        }
        case 0x68: {
            _cpu_load8(&(cpu->hl.low), cpu->bc.high);
            break;
        }
        case 0x69: {
            _cpu_load8(&(cpu->hl.low), cpu->bc.low);
            break;
        }
        case 0x6A: {
            _cpu_load8(&(cpu->hl.low), cpu->de.high);
            break;
        }
        case 0x6B: {
            _cpu_load8(&(cpu->hl.low), cpu->de.low);
            break;
        }
        case 0x6C: {
            _cpu_load8(&(cpu->hl.low), cpu->hl.high);
            break;
        }
        case 0x6D: {
            _cpu_load8(&(cpu->hl.low), cpu->hl.low);
            break;
        }
        case 0x6E: {
            _cpu_load8(&(cpu->hl.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x6F: {
            _cpu_load8(&(cpu->hl.low), cpu->af.high);
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
            _cpu_load8(&(cpu->af.high), mem_read(mem, cpu->bc.word));
            _cpu_synchronize(4);
            break;
        }
        case 0x1A: {
            _cpu_load8(&(cpu->af.high), mem_read(mem, cpu->de.word));
            _cpu_synchronize(4);
            break;
        }
        case 0xFA: {
            _cpu_load8(&(cpu->af.high), mem_read(mem, _cpu_read_imm16(cpu, mem)));
            _cpu_synchronize(4);
            break;
        }
        case 0x3E: {
            _cpu_load8(&(cpu->af.high), _cpu_read_imm8(cpu, mem));
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
            _cpu_load8(&(cpu->af.high), mem_read(mem, 0xFF00 + cpu->bc.low));
            _cpu_synchronize(4);
            break;
        }
        case 0xE2: {
            mem_write(mem, 0xFF00 + cpu->bc.low, cpu->af.high);
            _cpu_synchronize(8);
        }
        case 0x3A: {
            _cpu_load8(&(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
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
            _cpu_load8(&(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
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
            _cpu_load8(&(cpu->af.high), mem_read(mem, 0xFF00 + _cpu_read_imm8(cpu, mem)));
            _cpu_synchronize(4);
            break;
        }
        case 0x01: {
            _cpu_load16(&(cpu->bc.word), _cpu_read_imm16(cpu, mem));
            _cpu_synchronize(4);
            break;
        }
        case 0x11: {
            _cpu_load16(&(cpu->de.word), _cpu_read_imm16(cpu, mem));
            _cpu_synchronize(4);
            break;
        }
        case 0x21: {
            _cpu_load16(&(cpu->hl.word), _cpu_read_imm16(cpu, mem));
            _cpu_synchronize(4);
            break;
        }
        case 0x31: {
            _cpu_load16(&(cpu->sp), _cpu_read_imm16(cpu, mem));
            _cpu_synchronize(4);
            break;
        }
        case 0xF9: {
            _cpu_load16(&(cpu->sp), cpu->hl.word);
            _cpu_synchronize(4);
            break;
        }
        case 0xF8: {
            _cpu_load16(&(cpu->hl.word), cpu->sp + (int8_t)(_cpu_read_imm8(cpu, mem)));

            flags->byte = 0;
            flags->c = (((cpu->sp + (int8_t)(cpu->last_imm8)) & 0xFF) < (cpu->sp & 0xFF));
            flags->h = (((cpu->sp + (int8_t)(cpu->last_imm8)) & 0xF) < (cpu->sp & 0xF));

            _cpu_synchronize(4);
            break;
        }
        case 0x08: {
            mem_write(mem, _cpu_read_imm16(cpu, mem), cpu->sp & 0xFF);
            _cpu_synchronize(4);
            cpu->last_imm16++;
            mem_write(mem, cpu->last_imm16, cpu->sp & 0xFF00);
            _cpu_synchronize(12);
            break;
        }
        case 0xF5: {
            _cpu_stack_push(cpu, mem, cpu->af.word);
            _cpu_synchronize(4);
            break;
        }
        case 0xC5: {
            _cpu_stack_push(cpu, mem, cpu->bc.word);
            _cpu_synchronize(4);
            break;
        }
        case 0xD5: {
            _cpu_stack_push(cpu, mem, cpu->de.word);
            _cpu_synchronize(4);
            break;
        }
        case 0xE5: {
            _cpu_stack_push(cpu, mem, cpu->hl.word);
            _cpu_synchronize(4);
            break;
        }
        case 0xF1: {
            _cpu_load16(&(cpu->af.word), _cpu_stack_pop(cpu, mem));
            cpu->af.low &= 0xF0;
            break;
        }
        case 0xC1: {
            _cpu_load16(&(cpu->bc.word), _cpu_stack_pop(cpu, mem));
            break;
        }
        case 0xD1: {
            _cpu_load16(&(cpu->de.word), _cpu_stack_pop(cpu, mem));
            break;
        }
        case 0xE1: {
            _cpu_load16(&(cpu->hl.word), _cpu_stack_pop(cpu, mem));
            break;
        }
        case 0x80: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->bc.high), false);
            break;
        }
        case 0x81: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->bc.low), false);
            break;
        }
        case 0x82: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->de.high), false);
            break;
        }
        case 0x83: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->de.low), false);
            break;
        }
        case 0x84: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->hl.high), false);
            break;
        }
        case 0x85: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->hl.low), false);
            break;
        }
        case 0x86: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_add8(cpu, &(cpu->af.high), &thing_at_hl, false);
            break;
        }
        case 0x87: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->af.high), false);
            break;
        }
        case 0xC6: {
            uint8_t value = _cpu_read_imm8(cpu, mem);
            _cpu_add8(cpu, &(cpu->af.high), &value, false);
            break;
        }
        case 0x88: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->bc.high), true);
            break;
        }
        case 0x89: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->bc.low), true);
            break;
        }
        case 0x8A: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->de.high), true);
            break;
        }
        case 0x8B: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->de.low), true);
            break;
        }
        case 0x8C: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->hl.high), true);
            break;
        }
        case 0x8D: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->hl.low), true);
            break;
        }
        case 0x8E: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_add8(cpu, &(cpu->af.high), &thing_at_hl, true);
            break;
        }
        case 0x8F: {
            _cpu_add8(cpu, &(cpu->af.high), &(cpu->af.high), true);
            break;
        }
        case 0xCE: {
            uint8_t value = _cpu_read_imm8(cpu, mem);
            _cpu_add8(cpu, &(cpu->af.high), &value, true);
            break;
        }
        case 0x90: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->bc.high), false);
            break;
        }
        case 0x91: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->bc.low), false);
            break;
        }
        case 0x92: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->de.high), false);
            break;
        }
        case 0x93: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->de.low), false);
            break;
        }
        case 0x94: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->hl.high), false);
            break;
        }
        case 0x95: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->hl.low), false);
            break;
        }
        case 0x96: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_sub8(cpu, &(cpu->af.high), &thing_at_hl, false);
            break;
        }
        case 0x97: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->af.high), false);
            break;
        }
        case 0xD6: {
            uint8_t value = _cpu_read_imm8(cpu, mem);
            _cpu_sub8(cpu, &(cpu->af.high), &value, false);
            break;
        }
        case 0x98: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->bc.high), true);
            break;
        }
        case 0x99: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->bc.low), true);
            break;
        }
        case 0x9A: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->de.high), true);
            break;
        }
        case 0x9B: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->de.low), true);
            break;
        }
        case 0x9C: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->hl.high), true);
            break;
        }
        case 0x9D: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->hl.low), true);
            break;
        }
        case 0x9E: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_sub8(cpu, &(cpu->af.high), &thing_at_hl, true);
            break;
        }
        case 0x9F: {
            _cpu_sub8(cpu, &(cpu->af.high), &(cpu->af.high), true);
            break;
        }
        case 0xA0: {
            _cpu_and8(cpu, &(cpu->af.high), &(cpu->bc.high));
            break;
        }
        case 0xA1: {
            _cpu_and8(cpu, &(cpu->af.high), &(cpu->bc.low));
            break;
        }
        case 0xA2: {
            _cpu_and8(cpu, &(cpu->af.high), &(cpu->de.high));
            break;
        }
        case 0xA3: {
            _cpu_and8(cpu, &(cpu->af.high), &(cpu->de.low));
            break;
        }
        case 0xA4: {
            _cpu_and8(cpu, &(cpu->af.high), &(cpu->hl.high));
            break;
        }
        case 0xA5: {
            _cpu_and8(cpu, &(cpu->af.high), &(cpu->hl.low));
            break;
        }
        case 0xA6: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_and8(cpu, &(cpu->af.high), &thing_at_hl);
            break;
        }
        case 0xE6: {
            uint8_t imm8 = _cpu_read_imm8(cpu, mem);
            _cpu_and8(cpu, &(cpu->af.high), &imm8);
            break;
        }
        case 0xA7: {
            _cpu_and8(cpu, &(cpu->af.high), &(cpu->af.high));
            break;
        }
        case 0xB0: {
            _cpu_or8(cpu, &(cpu->af.high), &(cpu->bc.high));
            break;
        }
        case 0xB1: {
            _cpu_or8(cpu, &(cpu->af.high), &(cpu->bc.low));
            break;
        }
        case 0xB2: {
            _cpu_or8(cpu, &(cpu->af.high), &(cpu->de.high));
            break;
        }
        case 0xB3: {
            _cpu_or8(cpu, &(cpu->af.high), &(cpu->de.low));
            break;
        }
        case 0xB4: {
            _cpu_or8(cpu, &(cpu->af.high), &(cpu->hl.high));
            break;
        }
        case 0xB5: {
            _cpu_or8(cpu, &(cpu->af.high), &(cpu->hl.low));
            break;
        }
        case 0xB6: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_or8(cpu, &(cpu->af.high), &thing_at_hl);
            break;
        }
        case 0xF6: {
            uint8_t imm8 = _cpu_read_imm8(cpu, mem);
            _cpu_or8(cpu, &(cpu->af.high), &imm8);
            break;
        }
        case 0xB7: {
            _cpu_or8(cpu, &(cpu->af.high), &(cpu->af.high));
            break;
        }
        case 0xA8: {
            _cpu_xor8(cpu, &(cpu->af.high), &(cpu->bc.high));
            break;
        }
        case 0xA9: {
            _cpu_xor8(cpu, &(cpu->af.high), &(cpu->bc.low));
            break;
        }
        case 0xAA: {
            _cpu_xor8(cpu, &(cpu->af.high), &(cpu->de.high));
            break;
        }
        case 0xAB: {
            _cpu_xor8(cpu, &(cpu->af.high), &(cpu->de.low));
            break;
        }
        case 0xAC: {
            _cpu_xor8(cpu, &(cpu->af.high), &(cpu->hl.high));
            break;
        }
        case 0xAD: {
            _cpu_xor8(cpu, &(cpu->af.high), &(cpu->hl.low));
            break;
        }
        case 0xAE: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_xor8(cpu, &(cpu->af.high), &thing_at_hl);
            break;
        }
        case 0xEE: {
            uint8_t imm8 = _cpu_read_imm8(cpu, mem);
            _cpu_xor8(cpu, &(cpu->af.high), &imm8);
            break;
        }
        case 0xAF: {
            _cpu_xor8(cpu, &(cpu->af.high), &(cpu->af.high));
            break;
        }
        case 0xB8: {
            _cpu_cp8(cpu, &(cpu->af.high), &(cpu->bc.high));
            break;
        }
        case 0xB9: {
            _cpu_cp8(cpu, &(cpu->af.high), &(cpu->bc.low));
            break;
        }
        case 0xBA: {
            _cpu_cp8(cpu, &(cpu->af.high), &(cpu->de.high));
            break;
        }
        case 0xBB: {
            _cpu_cp8(cpu, &(cpu->af.high), &(cpu->de.low));
            break;
        }
        case 0xBC: {
            _cpu_cp8(cpu, &(cpu->af.high), &(cpu->hl.high));
            break;
        }
        case 0xBD: {
            _cpu_cp8(cpu, &(cpu->af.high), &(cpu->hl.low));
            break;
        }
        case 0xBE: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_cp8(cpu, &(cpu->af.high), &thing_at_hl);
            break;
        }
        case 0xFE: {
            uint8_t imm8 = _cpu_read_imm8(cpu, mem);
            _cpu_cp8(cpu, &(cpu->af.high), &imm8);
            break;
        }
        case 0xBF: {
            _cpu_cp8(cpu, &(cpu->af.high), &(cpu->af.high));
            break;
        }
        case 0x04: {
            _cpu_inc8(cpu, &(cpu->bc.high));
            break;
        }
        case 0x0C: {
            _cpu_inc8(cpu, &(cpu->bc.low));
            break;
        }
        case 0x14: {
            _cpu_inc8(cpu, &(cpu->de.high));
            break;
        }
        case 0x1C: {
            _cpu_inc8(cpu, &(cpu->de.low));
            break;
        }
        case 0x24: {
            _cpu_inc8(cpu, &(cpu->hl.high));
            break;
        }
        case 0x2C: {
            _cpu_inc8(cpu, &(cpu->hl.low));
            break;
        }
        case 0x34: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_inc8(cpu, &thing_at_hl);
            _cpu_write_hl_addr(cpu, mem, thing_at_hl);
            _cpu_synchronize(4);
            break;
        }
        case 0x3C: {
            _cpu_inc8(cpu, &(cpu->af.high));
            break;
        }
        case 0x05: {
            _cpu_dec8(cpu, &(cpu->bc.high));
            break;
        }
        case 0x0D: {
            _cpu_dec8(cpu, &(cpu->bc.low));
            break;
        }
        case 0x15: {
            _cpu_dec8(cpu, &(cpu->de.high));
            break;
        }
        case 0x1D: {
            _cpu_dec8(cpu, &(cpu->de.low));
            break;
        }
        case 0x25: {
            _cpu_dec8(cpu, &(cpu->hl.high));
            break;
        }
        case 0x2D: {
            _cpu_dec8(cpu, &(cpu->hl.low));
            break;
        }
        case 0x35: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_dec8(cpu, &thing_at_hl);
            _cpu_write_hl_addr(cpu, mem, thing_at_hl);
            _cpu_synchronize(4);
            break;
        }
        case 0x3D: {
            _cpu_dec8(cpu, &(cpu->af.high));
            break;
        }
        case 0xE8: {
            cpu->sp += (int8_t)(_cpu_read_imm8(cpu, mem));

            flags->c = (((cpu->sp + (int8_t)(cpu->last_imm8)) & 0xFF) < (cpu->sp & 0xFF));
            flags->h = (((cpu->sp + (int8_t)(cpu->last_imm8)) & 0xF) < (cpu->sp & 0xF));

            _cpu_synchronize(12);
            break;
        }
        case 0x03: {
            cpu->bc.word++;
            _cpu_synchronize(8);
            break;
        }
        case 0x13: {
            cpu->de.word++;
            _cpu_synchronize(8);
            break;
        }
        case 0x23: {
            cpu->hl.word++;
            _cpu_synchronize(8);
            break;
        }
        case 0x33: {
            cpu->sp++;
            _cpu_synchronize(8);
            break;
        }
        case 0x0B: {
            cpu->bc.word--;
            _cpu_synchronize(8);
            break;
        }
        case 0x1B: {
            cpu->de.word--;
            _cpu_synchronize(8);
            break;
        }
        case 0x2B: {
            cpu->hl.word--;
            _cpu_synchronize(8);
            break;
        }
        case 0x3B: {
            cpu->sp--;
            _cpu_synchronize(8);
            break;
        }
    }
}
