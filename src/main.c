#include <windows.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "gui.h"
#include "input.h"
#include "shell.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1)
    {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("OCTO-Shell Emulator",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        printf("Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!font)
    {
        printf("Font loading failed: %s\n", TTF_GetError());
        // Try alternative font paths
        font = TTF_OpenFont("assets/FiraCode-Light.ttf", FONT_SIZE);
        if (!font)
        {
            font = TTF_OpenFont("FiraCode-Light.ttf", FONT_SIZE);
        }
        if (!font)
        {
            printf("Could not load any font\n");
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
            return 1;
        }
    }

    gui_init(renderer, font);

    // Initialize buffers
    char inputBuffer[INPUT_BUFFER_SIZE] = "";
    char output[MAX_LINES][INPUT_BUFFER_SIZE];
    int lineCount = 0;

    // Initialize output array
    for (int i = 0; i < MAX_LINES; i++)
    {
        output[i][0] = '\0';
    }

    // Add welcome message
    snprintf(output[lineCount], INPUT_BUFFER_SIZE, "Welcome to OCTO-Shell Emulator!");
    lineCount++;
    snprintf(output[lineCount], INPUT_BUFFER_SIZE, "Type 'help' for available commands.");
    lineCount++;

    const char *prompt = "> ";
    bool running = true;
    SDL_Event e;

    // Enable text input
    SDL_StartTextInput();

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
                // Fall through to handle other key events
            default:
                input_handle_event(&e, inputBuffer, output, &lineCount);
                gui_handle_mouse_event(&e);
                break;
            }
        }

        gui_render(prompt, inputBuffer, output, lineCount);
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup (moved outside the loop)
    SDL_StopTextInput();
    gui_cleanup();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}