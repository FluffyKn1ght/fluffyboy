#include <stdint.h>

typedef struct {
    // TODO: Implement
} memory_t;

uint8_t mem_read(memory_t* mem, uint16_t address);
uint16_t mem_readw(memory_t* mem, uint16_t address);
void mem_write(memory_t* mem, uint16_t address, uint8_t value);
