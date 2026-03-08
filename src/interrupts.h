#include <stdint.h>

typedef union {
    struct {
        bool _bit7: 1, _bit6: 1, _bit5: 1, joypad: 1, serial: 1, timer: 1, lcd_stat: 1, vblank: 1;
    };
    uint8_t byte;
} interrupt_register_t;
