#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

typedef enum {
    NOT_ARMED = 0,
    ARMED = 1,
    ACTIVE = 2
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
    int8_t cycles;
    int8_t size;
    char* disasm;
} cpu_result_t;

typedef struct {
    cpu_word_register_t af, bc, de, hl;
    uint16_t pc;
    uint16_t sp;
    bool ime;
    cpu_arm_state_t ei_state;
    bool halted;
    bool stopped;
    cpu_result_t result;
} cpu_state_t;

void cpu_stack_push(cpu_state_t* cpu, memory_t* mem, uint16_t value);
uint16_t cpu_stack_pop(cpu_state_t* cpu, memory_t* mem);
void cpu_execute_instruction(cpu_state_t* cpu, memory_t* mem);
