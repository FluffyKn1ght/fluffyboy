#include "memory.h"
#include "cartridge.h"
#include <stdint.h>
#include <stdlib.h>

uint8_t _mem_read_io(memory_t* mem, uint16_t address) {
    // TODO: Implement

    return 0;
}

memory_t* mem_create(cartridge_t* cart) {
    memory_t* mem = calloc(1, sizeof(memory_t));

    mem->cart = cart;
    mem->video_ram = malloc(0x2000);
    mem->work_ram = malloc(0x2000);
    mem->high_ram = malloc(0x7F);
    mem->oam = malloc(0xA0);
    mem->mmio = malloc(0x80);

    return mem;
}

void mem_destroy(memory_t* mem) {
    cart_destroy(mem->cart);
    free(mem->video_ram);
    free(mem->work_ram);
    free(mem->high_ram);
    free(mem->oam);
    free(mem->mmio);
    free(mem);
}

uint8_t mem_read(memory_t* mem, uint16_t address) {
    if ((0x0000 <= address) && (address <= 0x7FFF)) {
        // ROM
        return cart_read(mem->cart, address);
    } else if ((0x8000 <= address) && (address <= 0x9FFF)) {
        // VRAM
        return (*((uint8_t*)(mem->video_ram + (address - 0x8000))));
    } else if ((0xA000 <= address) && (address <= 0xBFFF)) {
        // SRAM
        return cart_read(mem->cart, address);
    } else if ((0xC000 <= address) && (address <= 0xDFFF)) {
        // WRAM
        return (*((uint8_t*)(mem->work_ram + (address - 0xC000))));
    } else if ((0xE000 <= address) && (address <= 0xFDFF)) {
        // Echo RAM (mirror of WRAM)
        return (*((uint8_t*)(mem->work_ram + (address - 0xE000))));
    } else if ((0xFE00 <= address) && (address <= 0xFE9F)) {
        // OAM
        return (*((uint8_t*)(mem->oam + (address - 0xFE00))));
    } else if ((0xFEA0 <= address) && (address <= 0xFEFF)) {
        // Unusable memory space
        if (mem->oam_locked) {
            return 0xFF;
        } else {
            // From Pan Docs: "On CGB revision E, AGB, AGS, and GBP, it returns the high
            // nibble of the lower address byte twice, e.g. FFAx returns $AA, FFBx returns
            // $BB, and so forth."
            return ((address & 0xF0) | ((address & 0xF0) >> 4));
        }
    } else if ((0xFF00 <= address) && (address <= 0xFF7F)) {
        return _mem_read_io(mem, address);
    } else if ((0xFF80 <= address) && (address <= 0xFFFE)) {
        return (*((uint8_t*)(mem->high_ram + (address - 0xFF80))));
    } else if (address == 0xFFFF) {
        return mem->int_enable;
    }
    return 0xFF;
}

uint16_t mem_readw(memory_t* mem, uint16_t address) {
    // TODO: Implement
    return 0;
}

void mem_write(memory_t* mem, uint16_t address, uint8_t value) {
    // TODO: Implement
    return;
}

uint8_t mem_read_cpu(memory_t* mem, uint16_t address) {
    if ((0x0000 <= address) && (address <= 0x7FFF)) {
        // ROM
        return cart_read(mem->cart, address);
    } else if ((0x8000 <= address) && (address <= 0x9FFF)) {
        // VRAM
        if (!mem->vram_locked) {
            return (*((uint8_t*)(mem->video_ram + (address - 0x8000))));
        } else {
            return 0xFF;
        }
    } else if ((0xA000 <= address) && (address <= 0xBFFF)) {
        // SRAM
        return cart_read(mem->cart, address);
    } else if ((0xC000 <= address) && (address <= 0xDFFF)) {
        // WRAM
        return (*((uint8_t*)(mem->work_ram + (address - 0xC000))));
    } else if ((0xE000 <= address) && (address <= 0xFDFF)) {
        // Echo RAM (mirror of WRAM)
        return (*((uint8_t*)(mem->work_ram + (address - 0xE000))));
    } else if ((0xFE00 <= address) && (address <= 0xFE9F)) {
        // OAM
        if (!mem->oam_locked) {
            return (*((uint8_t*)(mem->oam + (address - 0xFE00))));
        } else {
            return 0xFF;
        }
    } else if ((0xFEA0 <= address) && (address <= 0xFEFF)) {
        // Unusable memory space
        if (mem->oam_locked) {
            return 0xFF;
        } else {
            // From Pan Docs: "On CGB revision E, AGB, AGS, and GBP, it returns the high
            // nibble of the lower address byte twice, e.g. FFAx returns $AA, FFBx returns
            // $BB, and so forth."
            return ((address & 0xF0) | ((address & 0xF0) >> 4));
        }
    } else if ((0xFF00 <= address) && (address <= 0xFF7F)) {
        return _mem_read_io(mem, address);
    } else if ((0xFF80 <= address) && (address <= 0xFFFE)) {
        return (*((uint8_t*)(mem->high_ram + (address - 0xFF80))));
    } else if (address == 0xFFFF) {
        return mem->int_enable;
    }
    return 0xFF;
}

uint16_t mem_readw_cpu(memory_t* mem, uint16_t address) {
    // TODO: Implement
    return 0;
}

void mem_write_cpu(memory_t* mem, uint16_t address, uint8_t value) {
    // TODO: Implement
    return;
}

interrupt_register_t mem_get_intflag(memory_t* mem) {
    return (interrupt_register_t)mem_read(mem, 0xFF0F);
}

interrupt_register_t mem_get_intenable(memory_t* mem) {
    return (interrupt_register_t)mem_read(mem, 0xFFFF);
}
