#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "cpu.h"
#include "ppu.h"

typedef struct {
    memory_t* memory;
    cpu_state_t* cpu;
    ppu_state_t* ppu;
} fluffy_emu_state_t;

typedef enum: uint8_t {
    FLUFFYEMU_ERR_OK = 0,
    FLUFFYEMU_ERR_NO_MEMORY = 1,
    FLUFFYEMU_ERR_UNKNOWN_MAPPER = 2,
    FLUFFYEMU_ERR_CPU_JAMMED = 255,
} fluffy_emu_error_t;

fluffy_emu_state_t* fluffy_emu_create();
void fluffy_emu_destroy(fluffy_emu_state_t* emu);
fluffy_emu_error_t fluffy_emu_step(fluffy_emu_state_t* emu);

#endif
