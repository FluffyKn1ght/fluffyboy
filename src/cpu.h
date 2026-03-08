#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

typedef enum {
    ARMSTATE_NOT_ARMED = 0,
    ARMSTATE_ARMED = 1,
    ARMSTATE_ACTIVE = 2
} cpu_arm_state_t;

typedef union {
    struct {
        uint8_t high;
        uint8_t low;
    };
    uint16_t word;
} cpu_word_register_t;

typedef union {
    struct {
        bool z: 1, n: 1, h: 1, c: 1;
    };
    uint8_t byte;
} cpu_f_register_t;

typedef struct {
    int8_t size;
    char disasm[32];
} cpu_disassembly_t;

typedef struct {
    cpu_word_register_t af, bc, de, hl;
    uint16_t pc;
    uint16_t sp;
    bool ime;
    cpu_arm_state_t ei_state;
    bool halted;
    bool halt_bug;
    bool stopped;
    bool jammed;
    cpu_disassembly_t inst_disasm;
    uint8_t last_imm8;
    uint16_t last_imm16;
    void(*synchronize)(void*, uint16_t);
    void* emu_struct_ptr;
} cpu_state_t;

cpu_state_t* cpu_create();
void cpu_destroy(cpu_state_t* cpu);
void cpu_step(cpu_state_t* cpu, memory_t* mem);

#endif
