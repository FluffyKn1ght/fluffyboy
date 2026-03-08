#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t internal_state;
    uint8_t next_internal_state;
    bool scx_extended_states;
    uint16_t sprite_cycle_penatly;
    uint16_t cycles_til_next_state;
    bool lcdc_interrupt;
    uint8_t ly;
} ppu_state_t;

ppu_state_t* ppu_create();
void ppu_destroy(ppu_state_t* ppu);
