#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>
#include "cartridge.h"
#include "interrupts.h"

typedef struct {
    cartridge_t* cart;
} memory_t;

memory_t* mem_create(cartridge_t* cart);
void mem_destroy(memory_t* mem);

uint8_t mem_read_cpu(memory_t* mem, uint16_t address);
uint16_t mem_readw_cpu(memory_t* mem, uint16_t address);
void mem_write_cpu(memory_t* mem, uint16_t address, uint8_t value);

uint8_t mem_read(memory_t* mem, uint16_t address);
uint16_t mem_readw(memory_t* mem, uint16_t address);
void mem_write(memory_t* mem, uint16_t address, uint8_t value);

interrupt_register_t mem_get_intflag(memory_t* mem);
interrupt_register_t mem_get_intenable(memory_t* mem);

#endif
