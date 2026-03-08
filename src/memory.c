#include "memory.h"
#include <stdint.h>
#include <stdlib.h>

memory_t* mem_create(cartridge_t* cart) {
    memory_t* mem = calloc(1, sizeof(memory_t));

    mem->cart = cart;

    return mem;
}

void mem_destroy(memory_t* mem) {
    cart_destroy(mem->cart);
    free(mem);
}

uint8_t mem_read(memory_t* mem, uint16_t address) {
    // TODO: Implement
    return 0x1F;
}

uint16_t mem_readw(memory_t* mem, uint16_t address) {
    // TODO: Implement
    return 0;
}

void mem_write(memory_t* mem, uint16_t address, uint8_t value) {
    // TODO: Implement
    return;
}

interrupt_register_t mem_get_intflag(memory_t* mem) {
    return (interrupt_register_t)mem_read(mem, 0xFF0F);
}

interrupt_register_t mem_get_intenable(memory_t* mem) {
    return (interrupt_register_t)mem_read(mem, 0xFFFF);
}
