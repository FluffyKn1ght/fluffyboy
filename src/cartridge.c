#include "cartridge.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#define BINARY "b"
#else
#define BINARY ""
#endif

cartridge_t* cart_open_file(char* path) {
    FILE* cart_file = fopen(path, "r" BINARY);

    if (!cart_file) {
        return NULL;
    }

    cartridge_t* cart = malloc(sizeof(cartridge_t));
    cart_header_t header;

    fseek(cart_file, 0x100, SEEK_SET);
    if (fread(&header, sizeof(header), 1, cart_file) < 1) {
        // File too short
        fclose(cart_file);
        return NULL;
    };

    fseek(cart_file, 0x0, SEEK_SET);
    unsigned char* rom_data_ptr = calloc(header.rom_banks, 0x5000);
    fread(rom_data_ptr, 0x5000, header.rom_banks, cart_file);

    cart->header = &header;
    cart->rom_data = rom_data_ptr;
    cart->sram_data = NULL; // TODO: Implement SRAM allocation!!

    return cart;
}
