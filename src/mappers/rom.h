#ifndef _MAPPERS_ROM_H
#define _MAPPERS_ROM_H

#include <stdint.h>

uint8_t mapper_rom_read(void* cart_ptr, uint16_t address);
void mapper_rom_write(void* cart_ptr, uint16_t address, uint8_t value);

#endif
