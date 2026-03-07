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
        printf("ERROR: could not open ROM image file '%s'\n", path);
        return NULL;
    }

    cartridge_t* cart = malloc(sizeof(cartridge_t));
    cart_header_t header;

    fseek(cart_file, 0x100, SEEK_SET);
    if (fread(&header, sizeof(header), 1, cart_file) < 1) {
        // File too short
        fclose(cart_file);
        printf("ERROR: '%s' is not a valid GameBoy ROM image - file way too short", path);
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

void cart_destroy(cartridge_t* cart) {
    // TODO: Save battery-backed SRAM to disk

    free(cart->rom_data);
    //free(cart->sram_data);
    free(cart);
}
