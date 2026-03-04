#include "memory.h"
#include <stdint.h>

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

typedef struct {
    int16_t cycles;
    char* disasm;
} cpu_result_t;

typedef struct {
    cpu_word_register_t af, bc, de, hl;
    uint16_t pc;
    uint16_t sp;
    uint8_t ime;
    cpu_arm_state_t halted;
    cpu_arm_state_t ei_state;
    uint8_t stopped;
    cpu_result_t result;
} cpu_state_t;

void cpu_execute_instruction(cpu_state_t* cpu, memory_t* mem);
