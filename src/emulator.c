#include "emulator.h"
#include "stdlib.h"

void _fluffy_emu_synchronize(void* emu_ptr, uint16_t cycles) {
    fluffy_emu_state_t* emu = (fluffy_emu_state_t*)emu_ptr;
}

fluffy_emu_state_t* fluffy_emu_create() {
    cpu_state_t* cpu = calloc(1, sizeof(cpu_state_t));

    fluffy_emu_state_t* emu = calloc(1, sizeof(fluffy_emu_state_t));
    emu->cpu = cpu;
    emu->cpu->emu_struct_ptr = emu;
    emu->cpu->synchronize = &_fluffy_emu_synchronize;

    return emu;
}

void fluffy_emu_destroy(fluffy_emu_state_t* emu) {
    cpu_destroy(emu->cpu);
    mem_destroy(emu->memory);

    free(emu);
}

fluffy_emu_error_t fluffy_emu_step(fluffy_emu_state_t* emu) {
    if (emu->memory == NULL) {
        return FLUFFYEMU_ERR_NO_MEMORY;
    }
    // TODO: Implement FLUFFYEMU_ERR_UNKNOWN_MAPPER
    if (emu->cpu->jammed) {
        return FLUFFYEMU_ERR_CPU_JAMMED;
    }

    cpu_step(emu->cpu, emu->memory);
    return FLUFFYEMU_ERR_OK;
}
