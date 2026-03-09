#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "argparse.h"
#include "emulator.h"
#include "cartridge.h"

static bool run = true;

void on_sigint(int sig) {
    printf("\n");
    run = false;
}

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

    // TODO: Implement player input and audio/video output via SDL

    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        printf("ERROR: could not initialize SDL: %s\n", SDL_GetError());
        fluffy_emu_destroy(emu);
        return 1;
    }

    SDL_Window* window;
    SDL_Renderer* renderer;
    if (!SDL_CreateWindowAndRenderer("fluffyboy", 160, 144, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        printf("ERROR: could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        fluffy_emu_destroy(emu);
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);
    if (!texture) {
        printf("ERROR: could not create texture: %s\n", SDL_GetError());
        SDL_Quit();
        fluffy_emu_destroy(emu);
        return 1;
    }

    SDL_FRect srcrect = {0, 0, 160, 144};
    SDL_FRect dstrect = {0, 0, 160, 144};

    SDL_ShowWindow(window);

    void* pixels;
    int pitch;
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch)) {
        memset(pixels, 0xFF, 160 * 144 * 4);
        SDL_UnlockTexture(texture);
    }

    printf("WARN: _run_emulator() is not fully implemented yet\n");
    while (run) {
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, &srcrect, &dstrect);
        SDL_RenderPresent(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT: {
                    run = false;
                    break;
                }
            }
        }

        /* fluffy_emu_error_t error = fluffy_emu_step(emu);
        if (error != 0) {
            switch (error) {
                case FLUFFYEMU_ERR_UNKNOWN_MAPPER: {
                    printf("ERROR: Unknown mapper chip (incompatible game)\n");
                    break;
                }
                case FLUFFYEMU_ERR_CPU_JAMMED: {
                    printf("ERROR: CPU jammed (illegal instruction/invalid STOP)\n");
                    break;
                }
                default: {
                    printf("ERROR: Unknown error 0x%X\n", error);
                    break;
                }
            }
            fluffy_emu_destroy(emu);
            return 1;
        } */
    }

    printf("WARN: Closing...\n");
    fluffy_emu_destroy(emu);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("INFO: Closed. See ya! :3\n");

    return 0;
}

int main(int argc, char** argv) {
    signal(SIGINT, &on_sigint);

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
