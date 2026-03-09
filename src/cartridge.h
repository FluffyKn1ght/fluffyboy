#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum: uint8_t {
    CART_CGB_LEGACY = 0,
    CART_CGB_CAPABLE = 0x80,
    CART_CGB_EXCLUSIVE = 0xC0
} cart_cgb_compat_t;

typedef enum: uint8_t {
    CART_SGB_NOT_CAPABLE = 0x00,
    CART_SGB_CAPABLE = 0x03
} cart_sgb_capable_t;

typedef enum: uint8_t {
    CART_TYPE_ROM_ONLY = 0x00,
    CART_TYPE_MBC1 = 0x01,
    CART_TYPE_MBC1_RAM = 0x02,
    CART_TYPE_MBC1_RAM_BATTERY = 0x03,
    CART_TYPE_MBC2 = 0x05,
    CART_TYPE_MBC2_BATTERY = 0x06,
    CART_TYPE_ROM_RAM = 0x08,
    CART_TYPE_ROM_RAM_BATTERY = 0x09,
    CART_TYPE_MMM01 = 0x0B,
    CART_TYPE_MMM01_RAM = 0x0C,
    CART_TYPE_MMM01_RAM_BATTERY = 0x0D,
    CART_TYPE_MBC3_RTC_BATTERY = 0x0F,
    CART_TYPE_MBC3_RTC_RAM_BATTERY = 0x10,
    CART_TYPE_MBC3 = 0x11,
    CART_TYPE_MBC3_RAM = 0x12,
    CART_TYPE_MBC3_RAM_BATTERY = 0x13,
    CART_TYPE_MBC5 = 0x19,
    CART_TYPE_MBC5_RAM = 0x1A,
    CART_TYPE_MBC5_RAM_BATTERY = 0x1B,
    CART_TYPE_MBC5_RUMBLE = 0x1C,
    CART_TYPE_MBC5_RUMBLE_RAM = 0x1D,
    CART_TYPE_MBC5_RUMBLE_RAM_BATTERY = 0x1E,
    CART_TYPE_MBC6 = 0x20,
    CART_TYPE_MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
    CART_TYPE_POCKET_CAMERA = 0xFC,
    CART_TYPE_TAMA5 = 0xFD,
    CART_TYPE_HUC3 = 0xFE,
    CART_TYPE_HUC1_RAM_BATTERY = 0xFF
} cart_type_t;

typedef union {
    struct {
        char _padding[0x100];
        unsigned char _entrypoint[4];
        char nintendo_logo[48];
        union {
            struct {
                char name[11];
                char id[4];
            };
            char title[15];
        };
        cart_cgb_compat_t cgb_compat;
        char licensee[2];
        cart_sgb_capable_t sgb_capable;
        cart_type_t cart_type;
        uint8_t rom_banks;
        uint8_t sram_banks;
        unsigned char is_export;
        unsigned char old_licensee;
        uint8_t mask_rom;
        uint8_t header_sum;
        uint16_t rom_sum;
    };
    uint8_t bytes[0x150];
} cart_header_t;

typedef struct {
    union {
        cart_header_t* header;
        unsigned char* rom_data;
    };
    unsigned char* sram_data;
} cartridge_t;

typedef struct {
    uint8_t ram_banks;
    bool battery;
    bool rtc;
} cart_sram_specs_t;

cartridge_t* cart_open_file(char* path);
void cart_destroy(cartridge_t* cart);
cart_sram_specs_t cart_get_sram_specs(cart_header_t* header);

#endif
