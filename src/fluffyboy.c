#include <stdio.h>
#include "cartridge.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("%s: no ROM file specified\n", argv[0]);
        return 1;
    }
    cartridge_t* cart = cart_open_file(argv[1]);
    if (cart == NULL) {
        printf("%s: could not open ROM file '%s'\n", argv[0], argv[1]);
        return 1;
    }
    printf("%X\n", cart->header->nintendo_logo[0]);
    printf("%s\n", cart->header->title);

    return 0;
}
