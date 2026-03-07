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
    bool stopped;
    cpu_disassembly_t inst_disasm;
} cpu_state_t;

void cpu_step(cpu_state_t* cpu, memory_t* mem);
void cpu_disassemble(cpu_state_t* cpu, memory_t* mem, uint16_t pc_override);
