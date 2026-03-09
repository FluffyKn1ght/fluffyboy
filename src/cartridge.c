#include "cartridge.h"
#include "mappers/rom.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#define BINARY "b"
#else
#define BINARY ""
#endif

const char NINTENDO_LOGO[48] = "\xce\xed\x66\x66\xcc\x0d\x00\x0b\x03\x73\x00\x83\x00\x0c\x00\x0d\x00\x08\x11\x1f\x88\x89\x00\x0e\xdc\xcc\x6e\xe6\xdd\xdd\xd9\x99\xbb\xbb\x67\x63\x6e\x0e\xec\xcc\xdd\xdc\x99\x9f\xbb\xb9\x33\x3e";

uint8_t _cart_sram_id_to_banks(uint8_t sram_id) {
    switch (sram_id) {
        case 0x02: return 1;
        case 0x03: return 4;
        case 0x04: return 16;
        case 0x05: return 8;
        default: return 0;
    }
}

bool _cart_setup_mapper(cartridge_t* cart, cart_header_t* header) {
    mapper_state_t* mapper = calloc(1, sizeof(mapper_state_t));
    cart->mapper = mapper;

    switch (header->cart_type) {
        case CART_TYPE_ROM_ONLY:
        case CART_TYPE_ROM_RAM:
        case CART_TYPE_ROM_RAM_BATTERY: {
            cart->mapper_read = &mapper_rom_read;
            cart->mapper_write = &mapper_rom_write;
            return true;
        }
        default: {
            free(cart->mapper);
            cart->mapper = NULL;
            return false;
        }
    }
}

cartridge_t* cart_open_file(char* path) {
    FILE* cart_file = fopen(path, "r" BINARY);

    if (!cart_file) {
        printf("ERROR: could not open ROM image file '%s'\n", path);
        return NULL;
    }

    cartridge_t* cart = malloc(sizeof(cartridge_t));
    cart_header_t header;

    fseek(cart_file, 0x0, SEEK_SET);
    if (fread(&header, sizeof(header), 1, cart_file) < 1) {
        // File too short
        fclose(cart_file);
        printf("ERROR: '%s' is not a valid GameBoy ROM image - file way too short", path);
        free(cart);
        return NULL;
    };

    int logo_cmp = strcmp(header.nintendo_logo, NINTENDO_LOGO);
    if (logo_cmp != 0) {
        for (int i = 0; i < 48; i++) {
            printf("0x%X 0x%X\n", header.nintendo_logo[i], NINTENDO_LOGO[i]);
        }

        fclose(cart_file);
        printf("ERROR: '%s' is not a valid GameBoy ROM image - Nintendo logo does not match\n", path);
        free(cart);
        return NULL;
    }

    uint8_t header_sum = 0;
    for (uint16_t addr = 0x134; addr <= 0x14C; addr++) {
        header_sum -= header.bytes[addr] + 1;
    }

    if (header_sum != header.header_sum) {
        fclose(cart_file);
        printf("ERROR: '%s' is not a valid GameBoy ROM image - header checksum does not match (expected 0x%X, got 0x%X instead)\n", path, header_sum, header.header_sum);
        free(cart);
        return NULL;
    }

    // TODO: Warn on global checksum mismatch?

    bool mapper_ok = _cart_setup_mapper(cart, &header);
    if (!mapper_ok) {{
        fclose(cart_file);
        printf("ERROR: ROM image '%s' uses an unsupported mapper chip with ID 0x%X\n", path, header.cart_type);
        free(cart->mapper);
        free(cart);
        return NULL;
    }}

    fseek(cart_file, 0x0, SEEK_SET);
    unsigned char* rom_data_ptr = calloc(1 + (1 << header.rom_banks), 0x4000);
    fread(rom_data_ptr, 0x4000, 1 + (1 << header.rom_banks), cart_file);

    cart_sram_specs_t sram_specs = cart_get_sram_specs(&header);
    if (sram_specs.ram_banks > 0) {
        cart->sram_fname = malloc(strlen(path) + 5);

        unsigned char* sram_data_ptr = calloc(sram_specs.ram_banks, 0x2000);

        if (sram_specs.battery) {
            sprintf(cart->sram_fname, "%s.sav", path);

            FILE* sram_file = fopen(cart->sram_fname, "r" BINARY);
            if (sram_file) {
                fread(sram_data_ptr, 0x2000, sram_specs.ram_banks, sram_file);
                fclose(sram_file);
            }

            // TODO: Handle RTC
        }

        cart->sram_data = sram_data_ptr;
    }

    cart->rom_data = rom_data_ptr;

    return cart;
}

void cart_destroy(cartridge_t* cart) {
    cart_sram_specs_t sram_specs = cart_get_sram_specs(cart->header);

    if (sram_specs.battery) {
        // TODO: Save RTC state

        FILE* sram_file = fopen(cart->sram_fname, "w" BINARY);
        if (sram_file) {
            fwrite(cart->sram_data, sram_specs.ram_banks, 0x2000, sram_file);
            fclose(sram_file);
        } else {
            printf("WARN: could not save SRAM/RTC state to disk\n");
        }
    }

    free(cart->rom_data);
    free(cart->sram_data);
    free(cart);
}

cart_sram_specs_t cart_get_sram_specs(cart_header_t* header) {
    cart_sram_specs_t specs = {};

    switch (header->cart_type) {
        case CART_TYPE_MBC1_RAM:
        case CART_TYPE_MBC3_RAM:
        case CART_TYPE_MBC5_RAM:
        case CART_TYPE_ROM_RAM: {
            specs.ram_banks = _cart_sram_id_to_banks(header->sram_banks);
            break;
        }
        case CART_TYPE_MBC1_RAM_BATTERY:
        case CART_TYPE_MBC3_RAM_BATTERY:
        case CART_TYPE_MBC5_RAM_BATTERY:
        case CART_TYPE_ROM_RAM_BATTERY:
        case CART_TYPE_HUC1_RAM_BATTERY:
        case CART_TYPE_MBC7_SENSOR_RUMBLE_RAM_BATTERY: {
            specs.ram_banks = _cart_sram_id_to_banks(header->sram_banks);
            specs.battery = true;
            break;
        }
        case CART_TYPE_MBC3_RTC_RAM_BATTERY: {
            specs.ram_banks = _cart_sram_id_to_banks(header->sram_banks);
            specs.battery = true;
            specs.rtc = true;
            break;
        }
    }

    return specs;
}

uint8_t cart_read(cartridge_t* cart, uint16_t address) {
    return cart->mapper_read(cart, address);
}

void cart_write(cartridge_t* cart, uint16_t address, uint8_t value) {
    cart->mapper_write(cart, address, value);
}
