#include <stdio.h>
#include <stdlib.h>
#include "argparse.h"
#include "emulator.h"
#include "cartridge.h"

int _run_emulator(fluffy_emu_state_t* emu) {
    printf("INFO: ROM name: '%s'\n", emu->memory->cart->header->title);
    printf("INFO: ROM size: %d banks (%d KiB)\n", 1 << (1 + emu->memory->cart->header->rom_banks), 32 * (1 << (emu->memory->cart->header->rom_banks)));

    cart_sram_specs_t sram_specs = cart_get_sram_specs(emu->memory->cart->header);
    if (sram_specs.ram_banks) {
        printf("INFO: SRAM size: %d KiB", sram_specs.ram_banks * 8);

        if (sram_specs.battery) {
            printf("; battery-backed");
        }
        if (sram_specs.rtc) {
            printf("; uses real-time clock");
        }
        printf("\n");
    }

    // TODO: Implement player input and SDL audio/video output

    printf("ERROR: _run_emulator() is not fully implemented yet\n");

    bool run = true;

    while (run) {
        fluffy_emu_error_t error = fluffy_emu_step(emu);
        if (error != 0) {
            switch (error) {
                case FLUFFYEMU_ERR_UNKNOWN_MAPPER: {
                    printf("ERROR: Unknown mapper chip (incompatible game)\n");
                    return 1;
                }
                case FLUFFYEMU_ERR_CPU_JAMMED: {
                    printf("ERROR: CPU jammed (illegal instruction/invalid STOP)\n");
                    return 1;
                }
                default: {
                    printf("ERROR: Unknown error 0x%X\n", error);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    argument_parser_t parser;
    argparse_init(&parser, argc, argv, "A simple GameBoy/GameBoy Color emulator", NULL);

    const char* romfile;
    argparse_arg_t arg_romfile = ARGPARSE_POSITIONAL(STRING, "ROMFILE", &romfile, "The ROM image file to load");

    argparse_add_argument(&parser, &arg_romfile);

    argparse_error_t arg_error = argparse_parse_args(&parser);
    if (arg_error.error_val) {
        argparse_print_help(&parser);
        return 1;
    }

    printf("INFO: FluffyBoy emulator, version 0.1.0\n");
    printf("INFO: 2026 (c) FluffyKn1ght / Licensed under GPLv3\n\n");

    cartridge_t* cart = cart_open_file(romfile);
    if (cart == NULL) {
        printf("ERROR: could not open/parse ROM image file\n");
        return 1;
    }

    memory_t* mem = mem_create(cart);

    fluffy_emu_state_t* emu = fluffy_emu_create();
    emu->memory = mem;

    return _run_emulator(emu);
}
