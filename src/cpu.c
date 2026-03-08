#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void _cpu_synchronize(memory_t* mem, uint8_t cycles) {
    // TODO: Implement
}

uint8_t _cpu_read_imm8(cpu_state_t* cpu, memory_t* mem) {
    uint8_t value = mem_read_cpu(mem, cpu->pc);
    cpu->pc++;
    _cpu_synchronize(mem, 4);
    cpu->last_imm8 = value;
    return value;
}

uint16_t _cpu_read_imm16(cpu_state_t* cpu, memory_t* mem) {
    uint8_t value_low_byte = mem_read_cpu(mem, cpu->pc);
    cpu->pc++;
    _cpu_synchronize(mem, 4);
    uint8_t value_high_byte = mem_read_cpu(mem, cpu->pc);
    cpu->pc++;
    _cpu_synchronize(mem, 4);
    uint16_t value = (value_low_byte) | (value_high_byte << 8);
    cpu->last_imm16 = value;
    return value;
}

uint8_t _cpu_read_hl_addr(cpu_state_t* cpu, memory_t* mem) {
    uint8_t value = mem_read_cpu(mem, cpu->hl.word);
    _cpu_synchronize(mem, 4);
    return value;
}

void _cpu_write_hl_addr(cpu_state_t* cpu, memory_t* mem, uint8_t value) {
    mem_write_cpu(mem, cpu->hl.word, value);
    _cpu_synchronize(mem, 4);
}

void _cpu_load8(memory_t* mem, uint8_t* dest, uint8_t source) {
    *dest = source;
    _cpu_synchronize(mem, 4);
}

void _cpu_load16(memory_t* mem, uint16_t* dest, uint16_t source) {
    *dest = source;
    _cpu_synchronize(mem, 4);
}

void _cpu_stack_push(cpu_state_t* cpu, memory_t* mem, uint16_t value) {
    mem_write_cpu(mem, cpu->sp, value & 0xFF);
    _cpu_synchronize(mem, 4);
    cpu->sp--;
    mem_write_cpu(mem, cpu->sp, (value & 0xFF00) >> 8);
    _cpu_synchronize(mem, 8);
    cpu->sp--;
}

uint16_t _cpu_stack_pop(cpu_state_t* cpu, memory_t* mem) {
    uint16_t value = mem_readw_cpu(mem, cpu->sp);
    cpu->sp += 2;
    _cpu_synchronize(mem, 8);
    return value;
}

void _cpu_add8(cpu_state_t* cpu, memory_t* mem, uint8_t* operand_a, uint8_t* operand_b, bool with_carry) {
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
    _cpu_synchronize(mem, 4);
}

void _cpu_sub8(cpu_state_t* cpu, memory_t* mem, uint8_t* operand_a, uint8_t* operand_b, bool with_carry) {
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
    _cpu_synchronize(mem, 4);
}

void _cpu_and8(cpu_state_t* cpu, memory_t* mem, uint8_t* operand_a, uint8_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;
    flags->h = 1;

    uint8_t result = *operand_a & *operand_b;
    if (result == 0) {
        flags->z = 1;
    }

    *operand_a = result;
    _cpu_synchronize(mem, 4);
}

void _cpu_or8(cpu_state_t* cpu, memory_t* mem, uint8_t* operand_a, uint8_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;

    uint8_t result = *operand_a | *operand_b;
    if (result == 0) {
        flags->z = 1;
    }

    *operand_a = result;
    _cpu_synchronize(mem, 4);
}

void _cpu_xor8(cpu_state_t* cpu, memory_t* mem, uint8_t* operand_a, uint8_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;

    uint8_t result = *operand_a ^ *operand_b;
    if (result == 0) {
        flags->z = 1;
    }

    *operand_a = result;
    _cpu_synchronize(mem, 4);
}

void _cpu_cp8(cpu_state_t* cpu, memory_t* mem, uint8_t* operand_a, uint8_t* operand_b) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;
    flags->n = 1;

    flags->h = ((*operand_a & 0xF) < (*operand_b & 0xF)) ? 0 : 1;
    flags->c = (*operand_a < *operand_b) ? 0 : 1;
    flags->z = (*operand_a == *operand_b) ? 1 : 0;

    _cpu_synchronize(mem, 4);
}

void _cpu_inc8(cpu_state_t* cpu, memory_t* mem, uint8_t* what) {
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
    _cpu_synchronize(mem, 4);
}

void _cpu_dec8(cpu_state_t* cpu, memory_t* mem, uint8_t* what) {
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
    _cpu_synchronize(mem, 4);
}

void _cpu_add16(cpu_state_t* cpu, memory_t* mem, uint16_t* operand_a, uint16_t* operand_b) {
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
    _cpu_synchronize(mem, 8);
}

void _cpu_stop(cpu_state_t* cpu, memory_t* mem) {
    if ((mem_read_cpu(mem, 0xFF00) & 0xF) != 0xF) {
        if (!(mem_read_cpu(mem, 0xFF0F) & mem_read_cpu(mem, 0xFFFF))) {
            cpu->halted = true;
            cpu->pc++;
        }
    } else {
        cpu->stopped = true;
        mem_write_cpu(mem, 0xFF04, 0x00);
        if (!(mem_read_cpu(mem, 0xFF0F) & mem_read_cpu(mem, 0xFFFF))) {
            cpu->pc++;
        }
    }
}

void _cpu_swap8(cpu_state_t* cpu, memory_t* mem, uint8_t* operand) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;

    if (*operand == 0) {
        flags->z = 1;
    }

    uint8_t lower_nibble = *operand & 0xF;
    uint8_t upper_nibble = (*operand & 0xF0) >> 4;
    *operand = (lower_nibble << 4) | upper_nibble;
    _cpu_synchronize(mem, 4);
}

void _cpu_rotate_left(cpu_state_t* cpu, memory_t* mem, uint8_t* operand) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;

    uint8_t edge_bit = *operand >> 7;
    *operand <<= 1;
    *operand |= edge_bit;

    flags->byte = 0;
    flags->c = edge_bit;
    flags->z = (*operand == 0);

    _cpu_synchronize(mem, 4);
}

void _cpu_rotate_right(cpu_state_t* cpu, memory_t* mem, uint8_t* operand) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);
    flags->byte = 0;

    uint8_t edge_bit = *operand & 0x1;
    *operand >>= 1;
    *operand |= (edge_bit << 7);

    flags->byte = 0;
    flags->c = edge_bit;
    flags->z = (*operand == 0);

    _cpu_synchronize(mem, 4);
}

void _cpu_rotate_left_thru_carry(cpu_state_t* cpu, memory_t* mem, uint8_t* operand) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);

    uint8_t edge_bit = *operand >> 7;
    *operand <<= 1;
    *operand |= (edge_bit);

    flags->byte = 0;
    flags->c = edge_bit;
    flags->z = (*operand == 0);

    _cpu_synchronize(mem, 4);
}

void _cpu_rotate_right_thru_carry(cpu_state_t* cpu, memory_t* mem, uint8_t* operand) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);

    uint8_t edge_bit = *operand & 0x1;

    *operand >>= 1;
    *operand |= (edge_bit << 7);

    flags->byte = 0;
    flags->c = edge_bit;
    flags->z = (*operand == 0);

    _cpu_synchronize(mem, 4);
}

void _cpu_shift_left(cpu_state_t* cpu, memory_t* mem, uint8_t* operand) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);

    uint8_t edge_bit = *operand >> 7;
    *operand <<= 1;

    flags->byte = 0;
    flags->c = edge_bit;
    flags->z = (*operand == 0);

    _cpu_synchronize(mem, 4);
}

void _cpu_shift_right(cpu_state_t* cpu, memory_t* mem, uint8_t* operand, bool keep_msb) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);

    uint8_t msb = *operand & 0x80;
    uint8_t edge_bit = *operand & 0x1;
    *operand >>= 1;
    if (keep_msb) {
        *operand |= (msb);
    }

    flags->byte = 0;
    flags->c = edge_bit;
    flags->z = (*operand == 0);

    _cpu_synchronize(mem, 4);
}

void _cpu_bit_test(cpu_state_t* cpu, memory_t* mem, uint8_t* operand, _BitInt(3) bit) {
    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);

    flags->n = 0;
    flags->h = 1;

    flags->z = (*operand & (0x1 << bit)) ? 1 : 0;
}

void _cpu_bit_change(cpu_state_t* cpu, memory_t* mem, uint8_t* operand, _BitInt(3) bit, bool set) {
    if (set) {
        *operand |= (0x1 << bit);
    } else {
        *operand &= ~(0x1 << bit);
    }
}

void _cpu_jump(cpu_state_t* cpu, memory_t* mem, uint16_t where, void* condition, bool cond_target, bool push_pc) {
    bool do_we_jump = (condition && (*(bool*)condition == cond_target)) || !condition;
    _cpu_synchronize(mem, 4);

    // jp not taken - +4 cycles
    // jp taken - +8 cycles
    // call not taken - +4 cycles
    // call taken - +16 (+12 +4) cycles

    if (do_we_jump) {
        if (push_pc) {
            _cpu_stack_push(cpu, mem, cpu->pc);
        }
        cpu->pc = where;
        _cpu_synchronize(mem, 4);
    }
}

void _cpu_exec_prefixed(cpu_state_t* cpu, memory_t* mem) {
    _cpu_synchronize(mem, 4);
    uint8_t opcode = mem_read_cpu(mem, cpu->pc);
    cpu->pc++;

    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);

    switch (opcode) {
        case 0x30: {
            _cpu_swap8(cpu, mem, &(cpu->bc.high));
            break;
        }
        case 0x31: {
            _cpu_swap8(cpu, mem, &(cpu->bc.low));
            break;
        }
        case 0x32: {
            _cpu_swap8(cpu, mem, &(cpu->de.high));
            break;
        }
        case 0x33: {
            _cpu_swap8(cpu, mem, &(cpu->de.low));
            break;
        }
        case 0x34: {
            _cpu_swap8(cpu, mem, &(cpu->hl.high));
            break;
        }
        case 0x35: {
            _cpu_swap8(cpu, mem, &(cpu->hl.low));
            break;
        }
        case 0x36: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_swap8(cpu, mem, &thing_at_hl);
            _cpu_write_hl_addr(cpu, mem, thing_at_hl);
            break;
        }
        case 0x37: {
            _cpu_swap8(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x00: {
            _cpu_rotate_left(cpu, mem, &(cpu->bc.high));
            break;
        }
        case 0x01: {
            _cpu_rotate_left(cpu, mem, &(cpu->bc.low));
            break;
        }
        case 0x02: {
            _cpu_rotate_left(cpu, mem, &(cpu->de.high));
            break;
        }
        case 0x03: {
            _cpu_rotate_left(cpu, mem, &(cpu->de.low));
            break;
        }
        case 0x04: {
            _cpu_rotate_left(cpu, mem, &(cpu->hl.high));
            break;
        }
        case 0x05: {
            _cpu_rotate_left(cpu, mem, &(cpu->hl.low));
            break;
        }
        case 0x06: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_rotate_left(cpu, mem, &value);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0x07: {
            _cpu_rotate_left(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x10: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->bc.high));
            break;
        }
        case 0x11: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->bc.low));
            break;
        }
        case 0x12: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->de.high));
            break;
        }
        case 0x13: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->de.low));
            break;
        }
        case 0x14: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->hl.high));
            break;
        }
        case 0x15: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->hl.low));
            break;
        }
        case 0x16: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_rotate_left_thru_carry(cpu, mem, &value);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0x17: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x08: {
            _cpu_rotate_right(cpu, mem, &(cpu->bc.high));
            break;
        }
        case 0x09: {
            _cpu_rotate_right(cpu, mem, &(cpu->bc.low));
            break;
        }
        case 0x0A: {
            _cpu_rotate_right(cpu, mem, &(cpu->de.high));
            break;
        }
        case 0x0B: {
            _cpu_rotate_right(cpu, mem, &(cpu->de.low));
            break;
        }
        case 0x0C: {
            _cpu_rotate_right(cpu, mem, &(cpu->hl.high));
            break;
        }
        case 0x0D: {
            _cpu_rotate_right(cpu, mem, &(cpu->hl.low));
            break;
        }
        case 0x0E: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_rotate_right(cpu, mem, &value);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0x0F: {
            _cpu_rotate_right(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x18: {
            _cpu_rotate_right_thru_carry(cpu, mem, &(cpu->bc.high));
            break;
        }
        case 0x19: {
            _cpu_rotate_right_thru_carry(cpu, mem, &(cpu->bc.low));
            break;
        }
        case 0x1A: {
            _cpu_rotate_right_thru_carry(cpu, mem, &(cpu->de.high));
            break;
        }
        case 0x1B: {
            _cpu_rotate_right_thru_carry(cpu, mem, &(cpu->de.low));
            break;
        }
        case 0x1C: {
            _cpu_rotate_right_thru_carry(cpu, mem, &(cpu->hl.high));
            break;
        }
        case 0x1D: {
            _cpu_rotate_right_thru_carry(cpu, mem, &(cpu->hl.low));
            break;
        }
        case 0x1E: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_rotate_right_thru_carry(cpu, mem, &value);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0x1F: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x20: {
            _cpu_shift_left(cpu, mem, &(cpu->bc.high));
            break;
        }
        case 0x21: {
            _cpu_shift_left(cpu, mem, &(cpu->bc.low));
            break;
        }
        case 0x22: {
            _cpu_shift_left(cpu, mem, &(cpu->de.high));
            break;
        }
        case 0x23: {
            _cpu_shift_left(cpu, mem, &(cpu->de.low));
            break;
        }
        case 0x24: {
            _cpu_shift_left(cpu, mem, &(cpu->hl.high));
            break;
        }
        case 0x25: {
            _cpu_shift_left(cpu, mem, &(cpu->hl.low));
            break;
        }
        case 0x26: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_shift_left(cpu, mem, &value);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0x27: {
            _cpu_shift_left(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x28: {
            _cpu_shift_right(cpu, mem, &(cpu->bc.high), true);
            break;
        }
        case 0x29: {
            _cpu_shift_right(cpu, mem, &(cpu->bc.low), true);
            break;
        }
        case 0x2A: {
            _cpu_shift_right(cpu, mem, &(cpu->de.high), true);
            break;
        }
        case 0x2B: {
            _cpu_shift_right(cpu, mem, &(cpu->de.low), true);
            break;
        }
        case 0x2C: {
            _cpu_shift_right(cpu, mem, &(cpu->hl.high), true);
            break;
        }
        case 0x2D: {
            _cpu_shift_right(cpu, mem, &(cpu->hl.low), true);
            break;
        }
        case 0x2E: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_shift_right(cpu, mem, &value, true);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0x2F: {
            _cpu_shift_right(cpu, mem, &(cpu->af.high), true);
            break;
        }
        case 0x38: {
            _cpu_shift_right(cpu, mem, &(cpu->bc.high), false);
            break;
        }
        case 0x39: {
            _cpu_shift_right(cpu, mem, &(cpu->bc.low), false);
            break;
        }
        case 0x3A: {
            _cpu_shift_right(cpu, mem, &(cpu->de.high), false);
            break;
        }
        case 0x3B: {
            _cpu_shift_right(cpu, mem, &(cpu->de.low), false);
            break;
        }
        case 0x3C: {
            _cpu_shift_right(cpu, mem, &(cpu->hl.high), false);
            break;
        }
        case 0x3D: {
            _cpu_shift_right(cpu, mem, &(cpu->hl.low), false);
            break;
        }
        case 0x3E: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_shift_right(cpu, mem, &value, false);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0x3F: {
            _cpu_shift_right(cpu, mem, &(cpu->af.high), false);
            break;
        }
        case 0x40: {
            _cpu_bit_test(cpu, mem,  &(cpu->bc.high), (_cpu_read_imm8(cpu, mem) & 0x7));
            break;
        }
        case 0x41: {
            _cpu_bit_test(cpu, mem,  &(cpu->bc.low), (_cpu_read_imm8(cpu, mem) & 0x7));
            break;
        }
        case 0x42: {
            _cpu_bit_test(cpu, mem,  &(cpu->de.high), (_cpu_read_imm8(cpu, mem) & 0x7));
            break;
        }
        case 0x43: {
            _cpu_bit_test(cpu, mem,  &(cpu->de.low), (_cpu_read_imm8(cpu, mem) & 0x7));
            break;
        }
        case 0x44: {
            _cpu_bit_test(cpu, mem,  &(cpu->hl.high), (_cpu_read_imm8(cpu, mem) & 0x7));
            break;
        }
        case 0x45: {
            _cpu_bit_test(cpu, mem,  &(cpu->hl.low), (_cpu_read_imm8(cpu, mem) & 0x7));
            break;
        }
        case 0x46: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_bit_test(cpu, mem,  &value, (_cpu_read_imm8(cpu, mem) & 0x7));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x47: {
            _cpu_bit_test(cpu, mem,  &(cpu->af.high), (_cpu_read_imm8(cpu, mem) & 0x7));
            break;
        }
        case 0xC0: {
            _cpu_bit_change(cpu, mem,  &(cpu->bc.high), (_cpu_read_imm8(cpu, mem) & 0x7), false);
            break;
        }
        case 0xC1: {
            _cpu_bit_change(cpu, mem,  &(cpu->bc.low), (_cpu_read_imm8(cpu, mem) & 0x7), false);
            break;
        }
        case 0xC2: {
            _cpu_bit_change(cpu, mem,  &(cpu->de.high), (_cpu_read_imm8(cpu, mem) & 0x7), false);
            break;
        }
        case 0xC3: {
            _cpu_bit_change(cpu, mem,  &(cpu->de.low), (_cpu_read_imm8(cpu, mem) & 0x7), false);
            break;
        }
        case 0xC4: {
            _cpu_bit_change(cpu, mem,  &(cpu->hl.high), (_cpu_read_imm8(cpu, mem) & 0x7), false);
            break;
        }
        case 0xC5: {
            _cpu_bit_change(cpu, mem,  &(cpu->hl.low), (_cpu_read_imm8(cpu, mem) & 0x7), false);
            break;
        }
        case 0xC6: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_bit_change(cpu, mem,  &value, (_cpu_read_imm8(cpu, mem) & 0x7), false);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0xC7: {
            _cpu_bit_change(cpu, mem,  &(cpu->af.high), (_cpu_read_imm8(cpu, mem) & 0x7), false);
            break;
        }
        case 0x80: {
            _cpu_bit_change(cpu, mem,  &(cpu->bc.high), (_cpu_read_imm8(cpu, mem) & 0x7), true);
            break;
        }
        case 0x81: {
            _cpu_bit_change(cpu, mem,  &(cpu->bc.low), (_cpu_read_imm8(cpu, mem) & 0x7), true);
            break;
        }
        case 0x82: {
            _cpu_bit_change(cpu, mem,  &(cpu->de.high), (_cpu_read_imm8(cpu, mem) & 0x7), true);
            break;
        }
        case 0x83: {
            _cpu_bit_change(cpu, mem,  &(cpu->de.low), (_cpu_read_imm8(cpu, mem) & 0x7), true);
            break;
        }
        case 0x84: {
            _cpu_bit_change(cpu, mem,  &(cpu->hl.high), (_cpu_read_imm8(cpu, mem) & 0x7), true);
            break;
        }
        case 0x85: {
            _cpu_bit_change(cpu, mem,  &(cpu->hl.low), (_cpu_read_imm8(cpu, mem) & 0x7), true);
            break;
        }
        case 0x86: {
            uint8_t value = _cpu_read_hl_addr(cpu, mem);
            _cpu_bit_change(cpu, mem,  &value, (_cpu_read_imm8(cpu, mem) & 0x7), true);
            _cpu_write_hl_addr(cpu, mem, value);
            break;
        }
        case 0x87: {
            _cpu_bit_change(cpu, mem,  &(cpu->af.high), (_cpu_read_imm8(cpu, mem) & 0x7), true);
            break;
        }
        default: {
            cpu->jammed = true;
            break;
        }
    }
}

void cpu_step(cpu_state_t* cpu, memory_t* mem) {
    if (cpu->jammed) {
        return;
    }

    if (cpu->stopped) {
        if ((mem_read_cpu(mem, 0xFF00) & 0xF) != 0xF) {
            cpu->stopped = false;
        } else {
            return;
        }
    }

    if (cpu->halted) {
        if (mem_read_cpu(mem, 0xFF0F) & mem_read_cpu(mem, 0xFFFF)) {
            cpu->halted = false;
            goto interrupts;
            if (cpu->halt_bug) {
                cpu->halt_bug = false;
                cpu->pc--;
            }
        } else {
            _cpu_synchronize(mem, 4);
            return;
        }
    }

    if (cpu->ei_state == ARMSTATE_ARMED) {
        cpu->ei_state = ARMSTATE_ACTIVE;
    } else if (cpu->ei_state == ARMSTATE_ACTIVE) {
        cpu->ime = true;
        cpu->ei_state = ARMSTATE_NOT_ARMED;
    }

    uint8_t opcode = mem_read_cpu(mem, cpu->pc);
    cpu->pc++;

    cpu_f_register_t* flags = (cpu_f_register_t*)&(cpu->af.low);

    switch (opcode) {
        case 0x06: {
            _cpu_load8(mem, &(cpu->bc.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x0E: {
            _cpu_load8(mem, &(cpu->bc.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x16: {
            _cpu_load8(mem, &(cpu->de.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x1E: {
            _cpu_load8(mem, &(cpu->de.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x26: {
            _cpu_load8(mem, &(cpu->hl.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x2E: {
            _cpu_load8(mem, &(cpu->hl.low), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x7F: {
            _cpu_load8(mem, &(cpu->af.high), cpu->af.high);
            break;
        }
        case 0x78: {
            _cpu_load8(mem, &(cpu->af.high), cpu->bc.high);
            break;
        }
        case 0x79: {
            _cpu_load8(mem, &(cpu->af.high), cpu->bc.low);
            break;
        }
        case 0x7A: {
            _cpu_load8(mem, &(cpu->af.high), cpu->de.high);
            break;
        }
        case 0x7B: {
            _cpu_load8(mem, &(cpu->af.high), cpu->de.low);
            break;
        }
        case 0x7C: {
            _cpu_load8(mem, &(cpu->af.high), cpu->hl.high);
            break;
        }
        case 0x7D: {
            _cpu_load8(mem, &(cpu->af.high), cpu->hl.low);
            break;
        }
        case 0x7E: {
            _cpu_load8(mem, &(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x40: {
            _cpu_load8(mem, &(cpu->bc.high), cpu->bc.high);
            break;
        }
        case 0x41: {
            _cpu_load8(mem, &(cpu->bc.high), cpu->bc.low);
            break;
        }
        case 0x42: {
            _cpu_load8(mem, &(cpu->bc.high), cpu->de.high);
            break;
        }
        case 0x43: {
            _cpu_load8(mem, &(cpu->bc.high), cpu->de.low);
            break;
        }
        case 0x44: {
            _cpu_load8(mem, &(cpu->bc.high), cpu->hl.high);
            break;
        }
        case 0x45: {
            _cpu_load8(mem, &(cpu->bc.high), cpu->hl.low);
            break;
        }
        case 0x46: {
            _cpu_load8(mem, &(cpu->bc.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x47: {
            _cpu_load8(mem, &(cpu->bc.high), cpu->af.high);
            break;
        }
        case 0x48: {
            _cpu_load8(mem, &(cpu->bc.low), cpu->bc.high);
            break;
        }
        case 0x49: {
            _cpu_load8(mem, &(cpu->bc.low), cpu->bc.low);
            break;
        }
        case 0x4A: {
            _cpu_load8(mem, &(cpu->bc.low), cpu->de.high);
            break;
        }
        case 0x4B: {
            _cpu_load8(mem, &(cpu->bc.low), cpu->de.low);
            break;
        }
        case 0x4C: {
            _cpu_load8(mem, &(cpu->bc.low), cpu->hl.high);
            break;
        }
        case 0x4D: {
            _cpu_load8(mem, &(cpu->bc.low), cpu->hl.low);
            break;
        }
        case 0x4E: {
            _cpu_load8(mem, &(cpu->bc.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x4F: {
            _cpu_load8(mem, &(cpu->bc.low), cpu->af.high);
            break;
        }
        case 0x50: {
            _cpu_load8(mem, &(cpu->de.high), cpu->bc.high);
            break;
        }
        case 0x51: {
            _cpu_load8(mem, &(cpu->de.high), cpu->bc.low);
            break;
        }
        case 0x52: {
            _cpu_load8(mem, &(cpu->de.high), cpu->de.high);
            break;
        }
        case 0x53: {
            _cpu_load8(mem, &(cpu->de.high), cpu->de.low);
            break;
        }
        case 0x54: {
            _cpu_load8(mem, &(cpu->de.high), cpu->hl.high);
            break;
        }
        case 0x55: {
            _cpu_load8(mem, &(cpu->de.high), cpu->hl.low);
            break;
        }
        case 0x56: {
            _cpu_load8(mem, &(cpu->de.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x57: {
            _cpu_load8(mem, &(cpu->de.high), cpu->af.high);
            break;
        }
        case 0x58: {
            _cpu_load8(mem, &(cpu->de.low), cpu->bc.high);
            break;
        }
        case 0x59: {
            _cpu_load8(mem, &(cpu->de.low), cpu->bc.low);
            break;
        }
        case 0x5A: {
            _cpu_load8(mem, &(cpu->de.low), cpu->de.high);
            break;
        }
        case 0x5B: {
            _cpu_load8(mem, &(cpu->de.low), cpu->de.low);
            break;
        }
        case 0x5C: {
            _cpu_load8(mem, &(cpu->de.low), cpu->hl.high);
            break;
        }
        case 0x5D: {
            _cpu_load8(mem, &(cpu->de.low), cpu->hl.low);
            break;
        }
        case 0x5E: {
            _cpu_load8(mem, &(cpu->de.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x5F: {
            _cpu_load8(mem, &(cpu->de.low), cpu->af.high);
            break;
        }
        case 0x60: {
            _cpu_load8(mem, &(cpu->hl.high), cpu->bc.high);
            break;
        }
        case 0x61: {
            _cpu_load8(mem, &(cpu->hl.high), cpu->bc.low);
            break;
        }
        case 0x62: {
            _cpu_load8(mem, &(cpu->hl.high), cpu->de.high);
            break;
        }
        case 0x63: {
            _cpu_load8(mem, &(cpu->hl.high), cpu->de.low);
            break;
        }
        case 0x64: {
            _cpu_load8(mem, &(cpu->hl.high), cpu->hl.high);
            break;
        }
        case 0x65: {
            _cpu_load8(mem, &(cpu->hl.high), cpu->hl.low);
            break;
        }
        case 0x66: {
            _cpu_load8(mem, &(cpu->hl.high), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x67: {
            _cpu_load8(mem, &(cpu->hl.high), cpu->af.high);
            break;
        }
        case 0x68: {
            _cpu_load8(mem, &(cpu->hl.low), cpu->bc.high);
            break;
        }
        case 0x69: {
            _cpu_load8(mem, &(cpu->hl.low), cpu->bc.low);
            break;
        }
        case 0x6A: {
            _cpu_load8(mem, &(cpu->hl.low), cpu->de.high);
            break;
        }
        case 0x6B: {
            _cpu_load8(mem, &(cpu->hl.low), cpu->de.low);
            break;
        }
        case 0x6C: {
            _cpu_load8(mem, &(cpu->hl.low), cpu->hl.high);
            break;
        }
        case 0x6D: {
            _cpu_load8(mem, &(cpu->hl.low), cpu->hl.low);
            break;
        }
        case 0x6E: {
            _cpu_load8(mem, &(cpu->hl.low), _cpu_read_hl_addr(cpu, mem));
            break;
        }
        case 0x6F: {
            _cpu_load8(mem, &(cpu->hl.low), cpu->af.high);
            break;
        }
        case 0x70: {
            _cpu_write_hl_addr(cpu, mem, cpu->bc.high);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x71: {
            _cpu_write_hl_addr(cpu, mem, cpu->bc.low);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x72: {
            _cpu_write_hl_addr(cpu, mem, cpu->de.high);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x73: {
            _cpu_write_hl_addr(cpu, mem, cpu->de.low);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x74: {
            _cpu_write_hl_addr(cpu, mem, cpu->hl.high);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x75: {
            _cpu_write_hl_addr(cpu, mem, cpu->hl.low);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x77: {
            _cpu_write_hl_addr(cpu, mem, cpu->af.high);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x36: {
            _cpu_write_hl_addr(cpu, mem, _cpu_read_imm8(cpu, mem));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x0A: {
            _cpu_load8(mem, &(cpu->af.high), mem_read_cpu(mem, cpu->bc.word));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x1A: {
            _cpu_load8(mem, &(cpu->af.high), mem_read_cpu(mem, cpu->de.word));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xFA: {
            _cpu_load8(mem, &(cpu->af.high), mem_read_cpu(mem, _cpu_read_imm16(cpu, mem)));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x3E: {
            _cpu_load8(mem, &(cpu->af.high), _cpu_read_imm8(cpu, mem));
            break;
        }
        case 0x02: {
            mem_write_cpu(mem, cpu->bc.word, cpu->af.high);
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0x12: {
            mem_write_cpu(mem, cpu->de.word, cpu->af.high);
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0xEA: {
            mem_write_cpu(mem, _cpu_read_imm16(cpu, mem), cpu->af.high);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xF2: {
            _cpu_load8(mem, &(cpu->af.high), mem_read_cpu(mem, 0xFF00 + cpu->bc.low));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xE2: {
            mem_write_cpu(mem, 0xFF00 + cpu->bc.low, cpu->af.high);
            _cpu_synchronize(mem, 8);
        }
        case 0x3A: {
            _cpu_load8(mem, &(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
            cpu->hl.word--;
            break;
        }
        case 0x32: {
            _cpu_write_hl_addr(cpu, mem, cpu->af.high);
            _cpu_synchronize(mem, 4);
            cpu->hl.word--;
            break;
        }
        case 0x2A: {
            _cpu_load8(mem, &(cpu->af.high), _cpu_read_hl_addr(cpu, mem));
            cpu->hl.word++;
            break;
        }
        case 0x22: {
            _cpu_write_hl_addr(cpu, mem, cpu->af.high);
            _cpu_synchronize(mem, 4);
            cpu->hl.word++;
            break;
        }
        case 0xE0: {
            mem_write_cpu(mem, 0xFF00 + _cpu_read_imm8(cpu, mem), cpu->af.high);
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0xF0: {
            _cpu_load8(mem, &(cpu->af.high), mem_read_cpu(mem, 0xFF00 + _cpu_read_imm8(cpu, mem)));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x01: {
            _cpu_load16(mem, &(cpu->bc.word), _cpu_read_imm16(cpu, mem));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x11: {
            _cpu_load16(mem, &(cpu->de.word), _cpu_read_imm16(cpu, mem));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x21: {
            _cpu_load16(mem, &(cpu->hl.word), _cpu_read_imm16(cpu, mem));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x31: {
            _cpu_load16(mem, &(cpu->sp), _cpu_read_imm16(cpu, mem));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xF9: {
            _cpu_load16(mem, &(cpu->sp), cpu->hl.word);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xF8: {
            _cpu_load16(mem, &(cpu->hl.word), cpu->sp + (int8_t)(_cpu_read_imm8(cpu, mem)));

            flags->byte = 0;
            flags->c = (((cpu->sp + (int8_t)(cpu->last_imm8)) & 0xFF) < (cpu->sp & 0xFF));
            flags->h = (((cpu->sp + (int8_t)(cpu->last_imm8)) & 0xF) < (cpu->sp & 0xF));

            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x08: {
            mem_write_cpu(mem, _cpu_read_imm16(cpu, mem), cpu->sp & 0xFF);
            _cpu_synchronize(mem, 4);
            cpu->last_imm16++;
            mem_write_cpu(mem, cpu->last_imm16, (cpu->sp & 0xFF00) >> 8);
            _cpu_synchronize(mem, 12);
            break;
        }
        case 0xF5: {
            _cpu_stack_push(cpu, mem, cpu->af.word);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xC5: {
            _cpu_stack_push(cpu, mem, cpu->bc.word);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xD5: {
            _cpu_stack_push(cpu, mem, cpu->de.word);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xE5: {
            _cpu_stack_push(cpu, mem, cpu->hl.word);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xF1: {
            _cpu_load16(mem, &(cpu->af.word), _cpu_stack_pop(cpu, mem));
            cpu->af.low &= 0xF0;
            break;
        }
        case 0xC1: {
            _cpu_load16(mem, &(cpu->bc.word), _cpu_stack_pop(cpu, mem));
            break;
        }
        case 0xD1: {
            _cpu_load16(mem, &(cpu->de.word), _cpu_stack_pop(cpu, mem));
            break;
        }
        case 0xE1: {
            _cpu_load16(mem, &(cpu->hl.word), _cpu_stack_pop(cpu, mem));
            break;
        }
        case 0x80: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->bc.high), false);
            break;
        }
        case 0x81: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->bc.low), false);
            break;
        }
        case 0x82: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->de.high), false);
            break;
        }
        case 0x83: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->de.low), false);
            break;
        }
        case 0x84: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->hl.high), false);
            break;
        }
        case 0x85: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->hl.low), false);
            break;
        }
        case 0x86: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_add8(cpu, mem, &(cpu->af.high), &thing_at_hl, false);
            break;
        }
        case 0x87: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->af.high), false);
            break;
        }
        case 0xC6: {
            uint8_t value = _cpu_read_imm8(cpu, mem);
            _cpu_add8(cpu, mem, &(cpu->af.high), &value, false);
            break;
        }
        case 0x88: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->bc.high), true);
            break;
        }
        case 0x89: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->bc.low), true);
            break;
        }
        case 0x8A: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->de.high), true);
            break;
        }
        case 0x8B: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->de.low), true);
            break;
        }
        case 0x8C: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->hl.high), true);
            break;
        }
        case 0x8D: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->hl.low), true);
            break;
        }
        case 0x8E: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_add8(cpu, mem, &(cpu->af.high), &thing_at_hl, true);
            break;
        }
        case 0x8F: {
            _cpu_add8(cpu, mem, &(cpu->af.high), &(cpu->af.high), true);
            break;
        }
        case 0xCE: {
            uint8_t value = _cpu_read_imm8(cpu, mem);
            _cpu_add8(cpu, mem, &(cpu->af.high), &value, true);
            break;
        }
        case 0x90: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->bc.high), false);
            break;
        }
        case 0x91: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->bc.low), false);
            break;
        }
        case 0x92: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->de.high), false);
            break;
        }
        case 0x93: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->de.low), false);
            break;
        }
        case 0x94: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->hl.high), false);
            break;
        }
        case 0x95: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->hl.low), false);
            break;
        }
        case 0x96: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_sub8(cpu, mem, &(cpu->af.high), &thing_at_hl, false);
            break;
        }
        case 0x97: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->af.high), false);
            break;
        }
        case 0xD6: {
            uint8_t value = _cpu_read_imm8(cpu, mem);
            _cpu_sub8(cpu, mem, &(cpu->af.high), &value, false);
            break;
        }
        case 0x98: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->bc.high), true);
            break;
        }
        case 0x99: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->bc.low), true);
            break;
        }
        case 0x9A: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->de.high), true);
            break;
        }
        case 0x9B: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->de.low), true);
            break;
        }
        case 0x9C: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->hl.high), true);
            break;
        }
        case 0x9D: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->hl.low), true);
            break;
        }
        case 0x9E: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_sub8(cpu, mem, &(cpu->af.high), &thing_at_hl, true);
            break;
        }
        case 0x9F: {
            _cpu_sub8(cpu, mem, &(cpu->af.high), &(cpu->af.high), true);
            break;
        }
        case 0xA0: {
            _cpu_and8(cpu, mem, &(cpu->af.high), &(cpu->bc.high));
            break;
        }
        case 0xA1: {
            _cpu_and8(cpu, mem, &(cpu->af.high), &(cpu->bc.low));
            break;
        }
        case 0xA2: {
            _cpu_and8(cpu, mem, &(cpu->af.high), &(cpu->de.high));
            break;
        }
        case 0xA3: {
            _cpu_and8(cpu, mem, &(cpu->af.high), &(cpu->de.low));
            break;
        }
        case 0xA4: {
            _cpu_and8(cpu, mem, &(cpu->af.high), &(cpu->hl.high));
            break;
        }
        case 0xA5: {
            _cpu_and8(cpu, mem, &(cpu->af.high), &(cpu->hl.low));
            break;
        }
        case 0xA6: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_and8(cpu, mem, &(cpu->af.high), &thing_at_hl);
            break;
        }
        case 0xE6: {
            uint8_t imm8 = _cpu_read_imm8(cpu, mem);
            _cpu_and8(cpu, mem, &(cpu->af.high), &imm8);
            break;
        }
        case 0xA7: {
            _cpu_and8(cpu, mem, &(cpu->af.high), &(cpu->af.high));
            break;
        }
        case 0xB0: {
            _cpu_or8(cpu, mem, &(cpu->af.high), &(cpu->bc.high));
            break;
        }
        case 0xB1: {
            _cpu_or8(cpu, mem, &(cpu->af.high), &(cpu->bc.low));
            break;
        }
        case 0xB2: {
            _cpu_or8(cpu, mem, &(cpu->af.high), &(cpu->de.high));
            break;
        }
        case 0xB3: {
            _cpu_or8(cpu, mem, &(cpu->af.high), &(cpu->de.low));
            break;
        }
        case 0xB4: {
            _cpu_or8(cpu, mem, &(cpu->af.high), &(cpu->hl.high));
            break;
        }
        case 0xB5: {
            _cpu_or8(cpu, mem, &(cpu->af.high), &(cpu->hl.low));
            break;
        }
        case 0xB6: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_or8(cpu, mem, &(cpu->af.high), &thing_at_hl);
            break;
        }
        case 0xF6: {
            uint8_t imm8 = _cpu_read_imm8(cpu, mem);
            _cpu_or8(cpu, mem, &(cpu->af.high), &imm8);
            break;
        }
        case 0xB7: {
            _cpu_or8(cpu, mem, &(cpu->af.high), &(cpu->af.high));
            break;
        }
        case 0xA8: {
            // note to future fluffy: please shove mem as the 1st/2nd arg EVERYWHERE
            _cpu_xor8(cpu, mem, &(cpu->af.high), &(cpu->bc.high));
            break;
        }
        case 0xA9: {
            _cpu_xor8(cpu, mem, &(cpu->af.high), &(cpu->bc.low));
            break;
        }
        case 0xAA: {
            _cpu_xor8(cpu, mem, &(cpu->af.high), &(cpu->de.high));
            break;
        }
        case 0xAB: {
            _cpu_xor8(cpu, mem, &(cpu->af.high), &(cpu->de.low));
            break;
        }
        case 0xAC: {
            _cpu_xor8(cpu, mem, &(cpu->af.high), &(cpu->hl.high));
            break;
        }
        case 0xAD: {
            _cpu_xor8(cpu, mem, &(cpu->af.high), &(cpu->hl.low));
            break;
        }
        case 0xAE: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_xor8(cpu, mem, &(cpu->af.high), &thing_at_hl);
            break;
        }
        case 0xEE: {
            uint8_t imm8 = _cpu_read_imm8(cpu, mem);
            _cpu_xor8(cpu, mem, &(cpu->af.high), &imm8);
            break;
        }
        case 0xAF: {
            _cpu_xor8(cpu, mem, &(cpu->af.high), &(cpu->af.high));
            break;
        }
        case 0xB8: {
            _cpu_cp8(cpu, mem, &(cpu->af.high), &(cpu->bc.high));
            break;
        }
        case 0xB9: {
            _cpu_cp8(cpu, mem, &(cpu->af.high), &(cpu->bc.low));
            break;
        }
        case 0xBA: {
            _cpu_cp8(cpu, mem, &(cpu->af.high), &(cpu->de.high));
            break;
        }
        case 0xBB: {
            _cpu_cp8(cpu, mem, &(cpu->af.high), &(cpu->de.low));
            break;
        }
        case 0xBC: {
            _cpu_cp8(cpu, mem,  &(cpu->af.high), &(cpu->hl.high));
            break;
        }
        case 0xBD: {
            _cpu_cp8(cpu, mem, &(cpu->af.high), &(cpu->hl.low));
            break;
        }
        case 0xBE: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_cp8(cpu, mem, &(cpu->af.high), &thing_at_hl);
            break;
        }
        case 0xFE: {
            uint8_t imm8 = _cpu_read_imm8(cpu, mem);
            _cpu_cp8(cpu, mem, &(cpu->af.high), &imm8);
            break;
        }
        case 0xBF: {
            _cpu_cp8(cpu, mem, &(cpu->af.high), &(cpu->af.high));
            break;
        }
        case 0x04: {
            _cpu_inc8(cpu, mem, &(cpu->bc.high));
            break;
        }
        case 0x0C: {
            _cpu_inc8(cpu, mem, &(cpu->bc.low));
            break;
        }
        case 0x14: {
            _cpu_inc8(cpu, mem, &(cpu->de.high));
            break;
        }
        case 0x1C: {
            _cpu_inc8(cpu, mem, &(cpu->de.low));
            break;
        }
        case 0x24: {
            _cpu_inc8(cpu, mem, &(cpu->hl.high));
            break;
        }
        case 0x2C: {
            _cpu_inc8(cpu, mem, &(cpu->hl.low));
            break;
        }
        case 0x34: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_inc8(cpu, mem, &thing_at_hl);
            _cpu_write_hl_addr(cpu, mem, thing_at_hl);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x3C: {
            _cpu_inc8(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x05: {
            _cpu_dec8(cpu, mem, &(cpu->bc.high));
            break;
        }
        case 0x0D: {
            _cpu_dec8(cpu, mem, &(cpu->bc.low));
            break;
        }
        case 0x15: {
            _cpu_dec8(cpu, mem, &(cpu->de.high));
            break;
        }
        case 0x1D: {
            _cpu_dec8(cpu, mem, &(cpu->de.low));
            break;
        }
        case 0x25: {
            _cpu_dec8(cpu, mem, &(cpu->hl.high));
            break;
        }
        case 0x2D: {
            _cpu_dec8(cpu, mem, &(cpu->hl.low));
            break;
        }
        case 0x35: {
            uint8_t thing_at_hl = _cpu_read_hl_addr(cpu, mem);
            _cpu_dec8(cpu, mem, &thing_at_hl);
            _cpu_write_hl_addr(cpu, mem, thing_at_hl);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x3D: {
            _cpu_dec8(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0xE8: {
            cpu->sp += (int8_t)(_cpu_read_imm8(cpu, mem));

            flags->c = (((cpu->sp + (int8_t)(cpu->last_imm8)) & 0xFF) < (cpu->sp & 0xFF));
            flags->h = (((cpu->sp + (int8_t)(cpu->last_imm8)) & 0xF) < (cpu->sp & 0xF));

            _cpu_synchronize(mem, 12);
            break;
        }
        case 0x03: {
            cpu->bc.word++;
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0x13: {
            cpu->de.word++;
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0x23: {
            cpu->hl.word++;
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0x33: {
            cpu->sp++;
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0x0B: {
            cpu->bc.word--;
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0x1B: {
            cpu->de.word--;
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0x2B: {
            cpu->hl.word--;
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0x3B: {
            cpu->sp--;
            _cpu_synchronize(mem, 8);
            break;
        }
        case 0xCB: {
            _cpu_exec_prefixed(cpu, mem);
            break;
        }
        case 0x27: {
            uint16_t extended = (uint16_t)cpu->af.high;

            if (!flags->n) {
                if (flags->h || (extended & 0xF) > 9) {
                    extended += 0x06;
                }
                if (flags->c || extended > 0x9F) {
                    extended += 0x60;
                }
            } else {
                if (flags->h) {
                    extended = (extended - 6) & 0xFF;
                }
                if (flags->c) {
                    extended -= 0x60;
                }
            }

            flags->h = 0;
            flags->c = (extended >= 0x0100) ? 1 : 0;
            cpu->af.high = extended & 0xFF;
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x2F: {
            flags->n = 1;
            flags->h = 1;

            cpu->af.high = ~(cpu->af.high);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x3F: {
            flags->n = 0;
            flags->h = 0;
            flags->c = !(flags->c);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x37: {
            flags->n = 0;
            flags->h = 0;
            flags->c = 1;
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x00: {
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x76: {
            cpu->halted = true;
            cpu->halt_bug = (!cpu->ime && (mem_read_cpu(mem, 0xFF0F) & mem_read_cpu(mem, 0xFFFF)));
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x10: {
            _cpu_stop(cpu, mem);
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xF3: {
            cpu->ime = false;
            cpu->ei_state = ARMSTATE_NOT_ARMED;
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0xFB: {
            cpu->ei_state = ARMSTATE_ARMED;
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x07: {
            _cpu_rotate_left(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x17: {
            _cpu_rotate_left_thru_carry(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x0F: {
            _cpu_rotate_right(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0x1F: {
            _cpu_rotate_right_thru_carry(cpu, mem, &(cpu->af.high));
            break;
        }
        case 0xC3: {
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), NULL, false, false);
            break;
        }
        case 0xC2: {
            bool zero = flags->z;
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), &zero, false, false);
            break;
        }
        case 0xCA: {
            bool zero = flags->z;
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), &zero, true, false);
            break;
        }
        case 0xD2: {
            bool carry = flags->c;
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), &carry, false, false);
            break;
        }
        case 0xDA: {
            bool carry = flags->c;
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), &carry, true, false);
            break;
        }
        case 0xE9: {
            cpu->pc = cpu->hl.word;
            _cpu_synchronize(mem, 4);
            break;
        }
        case 0x18: {
            int8_t offset = (int8_t)_cpu_read_imm8(cpu, mem);
            _cpu_jump(cpu, mem, cpu->pc + offset, NULL, false, false);
            break;
        }
        case 0x20: {
            bool zero = flags->z;
            int8_t offset = (int8_t)_cpu_read_imm8(cpu, mem);
            _cpu_jump(cpu, mem, cpu->pc + offset, &zero, false, false);
            break;
        }
        case 0x28: {
            bool zero = flags->z;
            int8_t offset = (int8_t)_cpu_read_imm8(cpu, mem);
            _cpu_jump(cpu, mem, cpu->pc + offset, &zero, true, false);
            break;
        }
        case 0x30: {
            bool carry = flags->c;
            int8_t offset = (int8_t)_cpu_read_imm8(cpu, mem);
            _cpu_jump(cpu, mem, cpu->pc + offset, &carry, false, false);
            break;
        }
        case 0x38: {
            bool carry = flags->c;
            int8_t offset = (int8_t)_cpu_read_imm8(cpu, mem);
            _cpu_jump(cpu, mem, cpu->pc + offset, &carry, true, false);
            break;
        }
        case 0xCD: {
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), NULL, false, true);
            break;
        }
        case 0xC4: {
            bool zero = flags->z;
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), &zero, false, true);
            break;
        }
        case 0xCC: {
            bool zero = flags->z;
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), &zero, true, true);
            break;
        }
        case 0xD4: {
            bool carry = flags->c;
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), &carry, false, true);
            break;
        }
        case 0xDC: {
            bool carry = flags->c;
            _cpu_jump(cpu, mem, _cpu_read_imm16(cpu, mem), &carry, true, true);
            break;
        }
        case 0xC7: {
            _cpu_jump(cpu, mem, 0x0000, NULL, false, true);
            break;
        }
        case 0xCF: {
            _cpu_jump(cpu, mem, 0x0008, NULL, false, true);
            break;
        }
        case 0xD7: {
            _cpu_jump(cpu, mem, 0x0010, NULL, false, true);
            break;
        }
        case 0xDF: {
            _cpu_jump(cpu, mem, 0x0018, NULL, false, true);
            break;
        }
        case 0xE7: {
            _cpu_jump(cpu, mem, 0x0020, NULL, false, true);
            break;
        }
        case 0xEF: {
            _cpu_jump(cpu, mem, 0x0028, NULL, false, true);
            break;
        }
        case 0xF7: {
            _cpu_jump(cpu, mem, 0x0030, NULL, false, true);
            break;
        }
        case 0xFF: {
            _cpu_jump(cpu, mem, 0x0038, NULL, false, true);
            break;
        }
        case 0xC9: {
            _cpu_jump(cpu, mem, _cpu_stack_pop(cpu, mem), NULL, false, false);
            break;
        }
        case 0xC0: {
            bool zero = flags->z;
            _cpu_jump(cpu, mem, _cpu_stack_pop(cpu, mem), &zero, false, false);
            break;
        }
        case 0xC8: {
            bool zero = flags->z;
            _cpu_jump(cpu, mem, _cpu_stack_pop(cpu, mem), &zero, true, false);
            break;
        }
        case 0xD0: {
            bool carry = flags->c;
            _cpu_jump(cpu, mem, _cpu_stack_pop(cpu, mem), &carry, false, false);
            break;
        }
        case 0xD8: {
            bool carry = flags->c;
            _cpu_jump(cpu, mem, _cpu_stack_pop(cpu, mem), &carry, true, false);
            break;
        }
        case 0xD9: {
            _cpu_jump(cpu, mem, _cpu_stack_pop(cpu, mem), NULL, false, false);
            cpu->ei_state = ARMSTATE_ARMED;
            break;
        }
        default: {
            cpu->jammed = true;
            break;
        }
    }

    interrupts:
    if (cpu->ime) {
        cpu->ime = false;
        cpu->ei_state = ARMSTATE_NOT_ARMED; // TODO: Test if this is correct

        interrupt_register_t int_flag = mem_get_intflag(mem);
        interrupt_register_t int_enable = mem_get_intenable(mem);

        if (int_flag.byte & int_enable.byte) {
            _cpu_stack_push(cpu, mem, cpu->pc);
            _cpu_synchronize(mem, 12);

            if (int_flag.vblank && int_enable.vblank) {
                int_flag.vblank = false;
                cpu->pc = 0x0040;
            } else if (int_flag.lcd_stat && int_enable.lcd_stat) {
                int_flag.lcd_stat = false;
                cpu->pc = 0x0048;
            } else if (int_flag.timer && int_enable.timer) {
                int_flag.timer = false;
                cpu->pc = 0x0050;
            } else if (int_flag.serial && int_enable.serial) {
                int_flag.serial = false;
                cpu->pc = 0x0058;
            } else if (int_flag.joypad && int_enable.joypad) {
                int_flag.joypad = false;
                cpu->pc = 0x0060;
            }
        }
    }
}

cpu_state_t* cpu_create() {
    return calloc(1, sizeof(cpu_state_t));
}

void cpu_destroy(cpu_state_t* cpu) {
    cpu->jammed = true;
    free(cpu);
}
