#include <iostream>
#include <SDL3/SDL.h>

void cleanupSDL(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

int main(int argc, char** argv) {
    /**
     * Initializing SDL for visual window
     */

    // Width and height of window
    int width = 1024;
    int height = 512;

    if (!SDL_Init(SDL_INIT_AUDIO && SDL_INIT_VIDEO)) {
        SDL_Log("Could not initialize SDL subsystem!", SDL_GetError());
        SDL_Quit();
    }

    SDL_Window* window = SDL_CreateWindow("ChipWindow", width, height, 0);

    if (window == nullptr) {
        SDL_Log("Could not initialize window!", SDL_GetError());
        SDL_Quit();
    }

    SDL_Renderer* render = SDL_CreateRenderer(window, "ChipRender");

    if (render == nullptr) {
        SDL_Log("Could not initialize renderer!", SDL_GetError());
        SDL_Quit();
    }

    SDL_Texture* texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

    if (texture == nullptr) {
        SDL_Log("Could not initialize texture!", SDL_GetError());
        SDL_Quit();
    }

    /**
     * Stating emulation
     */



    return 0;
}