#include "SDL2/SDL.h"
#include "chip8.h"

static SDL_Window* window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static uint32_t buffer[64*32];

void initialize_screen()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Could not init SDL, something is wrong.\n");
    }
    window = SDL_CreateWindow(
            "CHIP-8 Emulator",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            640,
            320,
            SDL_WINDOW_SHOWN
    );
    if (!window)
    {
        printf("Window did not get created\n");
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 64, 32);
}

void update(uint8_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
        for (int j = 0; j < SCREEN_WIDTH; j++)
            buffer[i * SCREEN_WIDTH + j] = 0xFF000000 | (0xFFFFFF * pixels[i][j]);
    SDL_UpdateTexture(
        texture,
        NULL,
        buffer,
        SCREEN_WIDTH * sizeof(uint8_t) // TODO: height??
    );
    SDL_RenderCopy(
        renderer,
        texture,
        NULL,
        NULL
    );
    SDL_RenderPresent(renderer);
}


void cleanup_screen()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}