#ifndef CHIP8_H
#define CHIP8_H
#include "SDL2/SDL.h"
#include <stdbool.h>

/* Most programs begin here and do not
   access anything lower than this,
   this is where the interpreter itself
   lived. */
#define PROGRAM_START 0x200;

/* Pixels on the screen are off or on, so will just
   be storing 1's and 0's here. */
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

/* Hex pad for keys */
#define NUM_KEYS 16

extern const SDL_Keycode keymap[16];

void set_key(int keynum, bool value);
void emulate_cycle();
int load_rom();

#endif