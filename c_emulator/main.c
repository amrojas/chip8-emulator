#include <stdio.h>
#include "screen.c"

int main(int argc, char **argv)
{
    initialize_screen();
    // this is a todo
    while (1)
    {
        // Get the next event
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                break;
        }
    }
    cleanup_screen();
    return 0;
}