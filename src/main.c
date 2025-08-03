#include <windows.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "gui.h"
#include "input.h"
#include "shell.h"

// Define the global word wrap variable
int wordWrapEnabled = 0; // 0 = false, 1 = true

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Allocate console for debugging (optional - can be removed in release)
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    
    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf library
    if (TTF_Init() == -1)
    {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Create main window
    SDL_Window *window = SDL_CreateWindow("OCTO-Shell Emulator - Word Wrap Edition",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        printf("Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer with hardware acceleration
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font with fallback options
    TTF_Font *font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!font)
    {
        printf("Primary font loading failed: %s\n", TTF_GetError());
        printf("Trying fallback fonts...\n");

        // Try relative path
        font = TTF_OpenFont("assets/FiraCode-Light.ttf", FONT_SIZE);
        if (!font)
        {
            // Try current directory
            font = TTF_OpenFont("FiraCode-Light.ttf", FONT_SIZE);
            if (!font)
            {
                // Try system default monospace font
                font = TTF_OpenFont("C:/Windows/Fonts/consola.ttf", FONT_SIZE);
                if (!font)
                {
                    printf("Could not load any suitable font. Please ensure FiraCode-Light.ttf is available.\n");
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    TTF_Quit();
                    SDL_Quit();
                    return 1;
                }
                else
                {
                    printf("Using system Consolas font as fallback.\n");
                }
            }
            else
            {
                printf("Loaded font from current directory.\n");
            }
        }
        else
        {
            printf("Loaded font from assets directory.\n");
        }
    }
    else
    {
      
    }

    // Initialize GUI subsystem
    gui_init(renderer, font);

    // Initialize application state
    char inputBuffer[INPUT_BUFFER_SIZE] = "";
    char output[MAX_LINES][INPUT_BUFFER_SIZE];
    int lineCount = 0;
    int cursorPos = 0;
    
    // Initialize text selection structure
    TextSelection selection = {0};
    selection.startLine = 0;
    selection.startChar = 0;
    selection.endLine = 0;
    selection.endChar = 0;
    selection.active = 0;

    // Clear output buffer
    for (int i = 0; i < MAX_LINES; i++)
    {
        output[i][0] = '\0';
    }

    // Display welcome messages
    snprintf(output[lineCount], INPUT_BUFFER_SIZE, "~~~ OCTO-SHELL Emulator v2.0 ~~~");
    lineCount++;
    snprintf(output[lineCount], INPUT_BUFFER_SIZE, "");
    lineCount++;
    snprintf(output[lineCount], INPUT_BUFFER_SIZE, "Type 'help' for available commands.");
    lineCount++;
    snprintf(output[lineCount], INPUT_BUFFER_SIZE, "Type 'Shortcuts' for available Shortcuts.");
    lineCount++;
    snprintf(output[lineCount], INPUT_BUFFER_SIZE, "");
    lineCount++;

    const char *prompt = ">> ";
    bool running = true;
    SDL_Event e;

    // Enable text input for keyboard handling
    SDL_StartTextInput();

    printf("OCTO-Shell Emulator started successfully.\n");
    printf("Window size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);

    // Main application loop
    while (running && !shell_should_exit())
    {
        // Process all pending events
        while (SDL_PollEvent(&e))
        {
            // Handle window close event
            if (e.type == SDL_QUIT)
            {
                running = false;
                break;
            }

            // Handle keyboard events
            if (e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT)
            {
                // Check for escape key to exit
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                    break;
                }
                else
                {
                    // Pass keyboard events to input handler
                    input_handle_event(&e, inputBuffer, output, &lineCount, &cursorPos, &selection);
                }
            }
            
            // Handle mouse events for text selection
            if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEMOTION)
            {
                gui_handle_mouse_event(&e, output, inputBuffer, lineCount, &selection);
            }

            // Handle window resize events
            if (e.type == SDL_WINDOWEVENT)
            {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    printf("Window resized to: %dx%d\n", e.window.data1, e.window.data2);
                    // Note: Word wrap will automatically adjust to new window size
                }
            }
        }

        // Render the current frame
        gui_render(prompt, inputBuffer, output, lineCount, cursorPos, &selection);
        
        // Control frame rate (~60 FPS)
        SDL_Delay(16);
    }


    // Cleanup resources in reverse order of creation
    SDL_StopTextInput();
    gui_cleanup();
    
    if (font)
    {
        TTF_CloseFont(font);
    }
    
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    
    if (window)
    {
        SDL_DestroyWindow(window);
        printf("Window destroyed successfully.\n");
    }
    
    TTF_Quit();
    SDL_Quit();
    
    
    // Optional: Keep console window open for debugging
    #ifdef _DEBUG
    printf("Press Enter to close console...\n");
    getchar();
    #endif

    return 0;
}