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

uint8_t _cpu_read_hl_addr(cpu_state_t* cpu, memory_t* mem) {
    uint8_t value = mem_read(mem, cpu->hl.word);
    _cpu_synchronize(4);
    return value;
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
    }
}
