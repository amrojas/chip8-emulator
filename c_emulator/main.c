#include <stdio.h>
#include "screen.c"
#include "chip8.h"

int main(int argc, char **argv)
{
    initialize_screen();
    while (1)
    {
        // Get the next event
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                break;
            
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYDOWN)
            {
                for (int i = 0; i < NUM_KEYS; i++)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        set_key(i, event.type == SDL_KEYDOWN);
                    }
                }
            }

            update(screen);
        }
    }
    cleanup_screen();
    return 0;
}