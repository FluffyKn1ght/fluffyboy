#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "ppu.h"

enum: uint8_t {
    PPUSTATE_LY00_HBLANK,
    PPUSTATE_LYXX_HBLANK,
    PPUSTATE_LYXX_HBLANK_INC,
    PPUSTATE_LY00_VBLANK,
    PPUSTATE_LY9X_VBLANK,
    PPUSTATE_LY9X_VBLANK_INC,
    PPUSTATE_LYXX_OAM,
    PPUSTATE_LYXX_OAMVRAM,
};

enum: uint8_t {
    LCDCINT_HBLANK = 0x8,
    LCDCINT_VBLANK = 0x10,
    LCDCINT_OAM = 0x20,
    LCDCINT_LYC = 0x40,
};

enum: uint8_t {
    LCDMODE_HBLANK = 0,
    LCDMODE_VBLANK = 1,
    LCDMODE_OAM = 2,
    LCDMODE_RENDER = 3
};

lcdc_register_t _ppu_get_lcdc(memory_t* mem) {
    return (lcdc_register_t)mem_read(mem, 0xFF40);
}

stat_register_t _ppu_get_stat(memory_t* mem) {
    return (stat_register_t)mem_read(mem, 0xFF41);
}

void _ppu_request_lcdc_interrupt(ppu_state_t* ppu, memory_t* mem, uint8_t reason) {
    // TODO: Check if the specified reason can trigger the interrupt

    if (!ppu->lcdc_interrupt) {
        interrupt_register_t int_flag = mem_get_intflag(mem);
        int_flag.lcd_stat = 1;
        mem_write(mem, 0xFF0F, int_flag.byte);
        ppu->lcdc_interrupt = true;
    }
}

void _ppu_rearm_lcdc_interrupt(ppu_state_t* ppu, memory_t* mem) {
    interrupt_register_t int_flag = mem_get_intflag(mem);
    int_flag.lcd_stat = 0;
    mem_write(mem, 0xFF0F, int_flag.byte);
    ppu->lcdc_interrupt = false;
}

void _ppu_raise_vblank_interrupt(ppu_state_t* ppu, memory_t* mem) {
    interrupt_register_t int_flag = mem_get_intflag(mem);
    int_flag.vblank = 1;
    mem_write(mem, 0xFF0F, int_flag.byte);
}

void _ppu_do_ly_lyc_check(ppu_state_t* ppu, memory_t* mem) {
    uint8_t ly = mem_read(mem, 0xFF44);
    uint8_t lyc = mem_read(mem, 0xFF45);

    if (ly == lyc) {
        uint8_t stat = mem_read(mem, 0xFF41);
        if (!(stat & 0x4)) {
            _ppu_request_lcdc_interrupt(ppu, mem, LCDCINT_LYC);
            stat |= 0x4;
        } else {
            stat &= ~(0x4);
        }
        mem_write(mem, 0xFF41, stat);
    }
}

void _ppu_switch_to_next_state(ppu_state_t* ppu, memory_t* mem) {
    ppu->internal_state = ppu->next_internal_state;
    switch (ppu->internal_state) {
        case PPUSTATE_LYXX_OAM: {
            _ppu_rearm_lcdc_interrupt(ppu, mem);
            _ppu_request_lcdc_interrupt(ppu, mem, LCDCINT_OAM);
            ppu->scx_extended_states = (mem_read(mem, 0xFF43) & 0x4) ? true : false;

            ppu->cycles_til_next_state = 80;
            ppu->next_internal_state = PPUSTATE_LYXX_OAMVRAM;
            break;
        }
        case PPUSTATE_LYXX_OAMVRAM: {
            stat_register_t stat = _ppu_get_stat(mem);
            stat.mode = LCDMODE_RENDER;
            mem_write(mem, 0xFF41, stat.byte);

            // TODO: Sprites

            ppu->cycles_til_next_state = 172 + ((ppu->scx_extended_states) ? 4 : 0);
            ppu->next_internal_state = PPUSTATE_LYXX_HBLANK;
            break;
        }
        case PPUSTATE_LYXX_HBLANK: {
            // TODO: Draw current scanline

            stat_register_t stat = _ppu_get_stat(mem);
            stat.mode = LCDMODE_HBLANK;
            mem_write(mem, 0xFF41, stat.byte);

            _ppu_request_lcdc_interrupt(ppu, mem, LCDCINT_HBLANK);

            ppu->cycles_til_next_state = 200 - ((ppu->scx_extended_states) ? 4 : 0) - ppu->sprite_cycle_penatly;
            ppu->next_internal_state = PPUSTATE_LYXX_HBLANK_INC;
            break;
        }
        case PPUSTATE_LYXX_HBLANK_INC: {
            ppu->ly++;

            uint8_t ly = mem_read(mem, 0xFF44);
            ly++;
            mem_write(mem, 0xFF44, ly);

            _ppu_do_ly_lyc_check(ppu, mem);

            ppu->cycles_til_next_state = 4;

            if (ppu->ly >= 144) {
                ppu->next_internal_state = PPUSTATE_LY9X_VBLANK;
            } else {
                ppu->next_internal_state = PPUSTATE_LYXX_OAM;
            }
            break;
        }
        case PPUSTATE_LY9X_VBLANK: {
            if (ppu->ly == 144) {
                stat_register_t stat = _ppu_get_stat(mem);
                stat.mode = LCDMODE_VBLANK;
                mem_write(mem, 0xFF41, stat.byte);

                _ppu_raise_vblank_interrupt(ppu, mem);
                _ppu_request_lcdc_interrupt(ppu, mem, LCDCINT_VBLANK);

                ppu->next_internal_state = PPUSTATE_LY9X_VBLANK_INC;
                ppu->cycles_til_next_state = 452;
            }
            break;
        }
        case PPUSTATE_LY9X_VBLANK_INC: {
            ppu->ly++;
            uint8_t ly = mem_read(mem, 0xFF44);
            ly++;
            mem_write(mem, 0xFF44, ly);

            _ppu_do_ly_lyc_check(ppu, mem);

            if (ppu->ly == 153) {
                ppu->next_internal_state = PPUSTATE_LY00_VBLANK;
            } else {
                ppu->next_internal_state = PPUSTATE_LY9X_VBLANK;
            }
            ppu->cycles_til_next_state = 4;
            break;
        }
        case PPUSTATE_LY00_VBLANK: {
            ppu->ly = 0;
            mem_write(mem, 0xFF44, 0);

            ppu->next_internal_state = PPUSTATE_LY00_HBLANK;
            ppu->cycles_til_next_state = 452;
            break;
        }
        case PPUSTATE_LY00_HBLANK: {
            stat_register_t stat = _ppu_get_stat(mem);
            stat.mode = LCDMODE_HBLANK;
            mem_write(mem, 0xFF41, stat.byte);

            ppu->next_internal_state = PPUSTATE_LYXX_OAM;
            ppu->cycles_til_next_state = 4;
            break;
        }
    }
}

ppu_state_t* ppu_create() {
    ppu_state_t* ppu = calloc(1, sizeof(ppu_state_t));

    return ppu;
}

void ppu_destroy(ppu_state_t* ppu) {
    free(ppu);
}

void ppu_step(ppu_state_t* ppu, memory_t* mem, uint8_t clocks) {
    uint8_t clock_counter = clocks;

    while (clock_counter >= ppu->cycles_til_next_state) {
        clock_counter -= ppu->cycles_til_next_state;

        lcdc_register_t lcdc = _ppu_get_lcdc(mem);

        if (!lcdc.lcd_enable) {
            ppu->ly = 0;
            ppu->cycles_til_next_state = 70224;
            return;
        }
    }
    _ppu_switch_to_next_state(ppu, mem);
}
