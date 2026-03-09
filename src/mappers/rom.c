#include <stdint.h>
#include "rom.h"
#include "../cartridge.h"

uint8_t mapper_rom_read(void* cart_ptr, uint16_t address) {
    cartridge_t* cart = (cartridge_t*)cart_ptr;
    if ((0x0000 <= address) && (address <= 0x7FFF)) {
        return cart->rom_data[address];
    } else if ((0xA000 <= address) && (address <= 0xBFFF)) {
        cart_sram_specs_t sram_specs = cart_get_sram_specs(cart->header);
        if (sram_specs.ram_banks) {
            return cart->sram_data[address];
        }
    }
    return 0xFF;
}

void mapper_rom_write(void* cart_ptr, uint16_t address, uint8_t value) {
    cartridge_t* cart = (cartridge_t*)cart_ptr;
    if ((0x0000 <= address) && (address <= 0x7FFF)) {
        *((uint8_t*)cart->rom_data + address) = value;
    } else if ((0xA000 <= address) && (address <= 0xBFFF)) {
        cart_sram_specs_t sram_specs = cart_get_sram_specs(cart->header);
        if (sram_specs.ram_banks) {
            *((uint8_t*)cart->sram_data + (address - 0xA000)) = value;
        }
    }
}
