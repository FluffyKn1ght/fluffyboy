#ifndef _PPU_H
#define _PPU_H

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

typedef struct {
    uint8_t internal_state;
    uint8_t next_internal_state;
    bool scx_extended_states;
    uint16_t sprite_cycle_penatly;
    uint32_t cycles_til_next_state;
    bool lcdc_interrupt;
    uint8_t ly;
} ppu_state_t;

typedef union {
    bool
    lcd_enable: 1,
    window_map: 1,
    window_enable: 1,
    bgwindow_data: 1,
    bg_data: 1,
    obj_size: 1,
    obj_enable: 1,
    bg_enable: 1;
    uint8_t byte;
} lcdc_register_t;

typedef union {
    struct {
        bool
        _bit7: 1,
        lyc_interrupt: 1,
        oam_interrupt: 1,
        vblank_interrupt: 1,
        hblank_interrupt: 1,
        ly_matches_lyc: 1;
        unsigned _BitInt(2) mode: 2;
    };
    uint8_t byte;
} stat_register_t;

ppu_state_t* ppu_create();
void ppu_destroy(ppu_state_t* ppu);
void ppu_step(ppu_state_t* ppu, memory_t* mem, uint8_t clocks);

#endif
