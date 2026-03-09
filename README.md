# fluffyboy

A simple GameBoy/GameBoy Color emulator written in C.

> [!IMPORTANT]
> This emulator is currently a work-in-progress project. **It CANNOT play GB/GBC ROMs yet**, and may be buggy and/or unstable.

# Features

> [!NOTE]
> Keep in mind that this is a list of *planned* features, not of all of them are currently implemented and not all of them ever will get implemented!

- DMG (classic GameBoy) and CGB (GameBoy Color) ROM support
- Mapper chip support (MBC1-MBC7, MMM01, NT/GBX)
- Proper PPU emulation (both B/W and color)
- Accuratee APU (audio) emulation, including Wave RAM access timings
- Accurate system timing emulation
- Battery-backed SRAM and MBC3 real-time clock support (w/ saving to disk)
- Embedding support with an intuitive enough API
- CPU instruction dissassembly
- User-friendly SDL3 frontend

# Usage

```
fluffyboy ROMFILE
```

- `ROMFILE` - The ROM image file to load (.GB/.GBC/.GBX)

# Building

> [!IMPORTANT]
> It is highly recommended to use Linux to build the emulator, Windows support is currently **NOT guranteed**.

## Prerequisites

You will need the following tools:
- `gcc` to compile the code
- `make` to run the build script

You will also need the following libraries:
- `SDL3` (if using built-in frontend (`src/fluffyboy.c`))

## Building the binary

1. Clone the repository:

```
git clone https://github.com/fluffykn1ght/fluffyboy.git
```

2. Run `make`
3. The binary will be saved as `fluffyboy`, while the individual object files will be saved into `bin/`.

> [!TIP]
> For your convinience, a "clean" recipe is available: `make clean`

# Acknowledgements

This project uses code from the following works:

- [**argparse** by pollyren](https://github.com/pollyren/argparse) (MIT license) - CLI argument parsing

Also, while not including their code directly, this project relies (partially/fully) on the following works:

- [**SDL3** by libsdl-org](https://libsdl.org/) (zlib license) - Frontend abstractions

# License

**fluffyboy** is distributed under the **GNU General Public License, version 3**.

A verbatim copy of the license text is available in the `LICENSE` file.
