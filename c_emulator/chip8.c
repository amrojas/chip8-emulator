#include "SDL2/SDL.h"
#include <stdio.h>


/* Most programs begin here and do not
   access anything lower than this,
   this is where the interpreter itself
   lived. */
#define PROGRAM_START 0x200;
/* 4 KiB of memory in the system */
unsigned char memory[4096];
/* Registers are named V0, V1, ..., VF,
   and are 8-bit. */
unsigned char registers[16];
/* Address register is 16 bits long. */
unsigned short register_i;
/* 4 KiB would take 12 bits to address. */
unsigned short program_counter;
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
/* Pixels on the screen are off or on, so will just
   be storing 1's and 0's here. */
unsigned char screen[SCREEN_HEIGHT][SCREEN_WIDTH];
/* Original 1802 version had 24 levels of nesting */
unsigned short stack[24];
unsigned char stack_pointer;
/* State of all hex keys must be saved. */
unsigned char keypad[16];
/* Two timers that count down at 60Hz
   Soudn timer causes beep when finished. */
unsigned char delay_timer;
unsigned char sound_timer;


void emulate_cycle()
{
    // One opcode takes 16 bits, so need to read in two spots
    unsigned short opcode = memory[program_counter] << 8 | memory[program_counter + 1];

}


void initialize()
{
    program_counter = PROGRAM_START;
    register_i = 0;
    stack_pointer = 0;
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Could not init SDL, something is wrong.\n");
    }
    
    SDL_Window* window = SDL_CreateWindow(
            "Hello World!",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            640,
            480,
            SDL_WINDOW_SHOWN
    );
    if (!window)
    {
        printf("Wtf why didn't this load dude?");
    }
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);
}


void cleanup()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void emulate()
{
    initialize();
    while(1)
    {
        emulate_cycle();
    }
    cleanup();
}


int main(int argc, char **argv)
{
    //emulate();
    int quit = 0;
    SDL_Event e;
    while (!quit)
    {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }
    return 0;
}