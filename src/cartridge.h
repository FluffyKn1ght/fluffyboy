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
    ROM_ONLY = 0x00,
} cart_type_t;

typedef struct {
    char _padding[0x100];
    unsigned char _entrypoint[4];
    unsigned char nintendo_logo[48];
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
} cart_header_t;

typedef struct {
    union {
        cart_header_t* header;
        unsigned char* rom_data;
    };
    unsigned char* sram_data;
} cartridge_t;

cartridge_t* cart_open_file(char* path);
void cart_destroy(cartridge_t* cart);
