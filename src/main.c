#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#include "config.h"
#include "gui.h"
#include "input.h"
#include "shell.h"

// Define the global word wrap variable
int wordWrapEnabled = 0; // 0 = false, 1 = true

// Define the global background configuration
BackgroundConfig backgroundConfig = {
    BACKGROUND_IMAGE_PATH,      // imagePath
    BACKGROUND_OPACITY,         // opacity
    BACKGROUND_SCALE_MODE,      // scaleMode
    BACKGROUND_ENABLED,         // enabled
    // BACKGROUND_BLUR_ENABLED,    // blurEnabled
    // BACKGROUND_TINT_R,          // tintR
    // BACKGROUND_TINT_G,          // tintG
    // BACKGROUND_TINT_B,          // tintB
    // BACKGROUND_OVERLAY_COLOR_R, // overlayR
    // BACKGROUND_OVERLAY_COLOR_G, // overlayG
    // BACKGROUND_OVERLAY_COLOR_B, // overlayB
    // BACKGROUND_OVERLAY_OPACITY  // overlayOpacity
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Allocate console for debugging
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    
    printf("Starting OCTO-Shell Emulator...\n");
    
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

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image init failed: %s\n", IMG_GetError());
        // Continue without full image support
    } else {
        printf("SDL_image initialized successfully with PNG and JPG support.\n");
    }

    // Create main window
    SDL_Window *window = SDL_CreateWindow("OCTO-SHELL Emulator",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        printf("Window creation failed: %s\n", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load and set window icon
    SDL_Surface* icon = IMG_Load("assets/icon.png");
    if (!icon) {
        printf("Failed to load window icon: %s\n", IMG_GetError());
        // Try alternative path
        icon = IMG_Load("icon.png");
        if (!icon) {
            printf("Window icon not found, continuing without icon.\n");
        }
    }
    if (icon) {
        SDL_SetWindowIcon(window, icon);
        SDL_FreeSurface(icon);
        printf("Window icon loaded successfully.\n");
    }

    // Create renderer with hardware acceleration
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load main font with comprehensive fallback system
    TTF_Font *font = NULL;
    const char* fontPaths[] = {
        FONT_PATH,
        "assets/Typewriter.ttf",
        "Typewriter.ttf",
        "assets/FiraCode-Light.ttf",
        "FiraCode-Light.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/cour.ttf",
        NULL
    };
    
    for (int i = 0; fontPaths[i] != NULL; i++) {
        font = TTF_OpenFont(fontPaths[i], FONT_SIZE);
        if (font) {
            printf("Successfully loaded font: %s\n", fontPaths[i]);
            break;
        } else {
            printf("Failed to load font %s: %s\n", fontPaths[i], TTF_GetError());
        }
    }
    
    if (!font) {
        printf("Critical error: Could not load any suitable font!\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load title font with fallbacks
    TTF_Font *titleFont = NULL;
    const char* titleFontPaths[] = {
        TITLE_FONT_PATH,
        "assets/FiraCode-Bold.ttf",
        "FiraCode-Bold.ttf",
        "C:/Windows/Fonts/consolab.ttf",
        "C:/Windows/Fonts/courbd.ttf",
        NULL
    };
    
    for (int i = 0; titleFontPaths[i] != NULL; i++) {
        titleFont = TTF_OpenFont(titleFontPaths[i], TITLE_FONT_SIZE);
        if (titleFont) {
            printf("Successfully loaded title font: %s\n", titleFontPaths[i]);
            break;
        }
    }
    
    if (!titleFont) {
        // Use main font as fallback for title
        titleFont = font;
        printf("Using main font for title.\n");
    }

    // Initialize GUI subsystem
    gui_init(renderer, font);
    gui_set_title_font(titleFont);

    // Load background image if enabled
    if (backgroundConfig.enabled) {
        printf("Attempting to load background image: %s\n", backgroundConfig.imagePath);
        
        // Try multiple possible paths for the background image
        const char* backgroundPaths[] = {
            backgroundConfig.imagePath,
            "assets/background.png",
            "background.png",
            "assets/background.jpg",
            "background.jpg",
            "assets/shell_bg.png",
            "shell_bg.png",
            NULL
        };
        
        int backgroundLoaded = 0;
        for (int i = 0; backgroundPaths[i] != NULL && !backgroundLoaded; i++) {
            if (gui_set_background_image(backgroundPaths[i]) == 1) {
                printf("Background image loaded from: %s\n", backgroundPaths[i]);
                // Update the config with the successful path
                strncpy(backgroundConfig.imagePath, backgroundPaths[i], sizeof(backgroundConfig.imagePath) - 1);
                backgroundConfig.imagePath[sizeof(backgroundConfig.imagePath) - 1] = '\0';
                backgroundLoaded = 1;
            }
        }
        
        if (!backgroundLoaded) {
            printf("Warning: Could not load any background image. Running without background.\n");
            backgroundConfig.enabled = 0;
        }
    } else {
        printf("Background image disabled in configuration.\n");
    }

    // Initialize application state
    char inputBuffer[INPUT_BUFFER_SIZE];
    char output[MAX_LINES][INPUT_BUFFER_SIZE];
    int lineCount = 0;
    int cursorPos = 0;
    
    // Initialize buffers
    memset(inputBuffer, 0, INPUT_BUFFER_SIZE);
    for (int i = 0; i < MAX_LINES; i++) {
        memset(output[i], 0, INPUT_BUFFER_SIZE);
    }
    
    // Initialize text selection structure
    TextSelection selection;
    memset(&selection, 0, sizeof(TextSelection));

    // Display welcome messages
    if (lineCount < MAX_LINES) {
        snprintf(output[lineCount], INPUT_BUFFER_SIZE, "");
        lineCount++;
    }
    if (lineCount < MAX_LINES) {
        snprintf(output[lineCount], INPUT_BUFFER_SIZE, "Welcome to OCTO-SHELL Emulator!");
        lineCount++;
    }
        if (lineCount < MAX_LINES) {
        snprintf(output[lineCount], INPUT_BUFFER_SIZE, "");
        lineCount++;
    }
    if (lineCount < MAX_LINES) {
        snprintf(output[lineCount], INPUT_BUFFER_SIZE, "Type 'help' for available commands.");
        lineCount++;
    }
    if (lineCount < MAX_LINES) {
        snprintf(output[lineCount], INPUT_BUFFER_SIZE, "");
        lineCount++;
    }

    const char *prompt = ">> ";
    bool running = true;
    SDL_Event e;

    // Enable text input for keyboard handling
    SDL_StartTextInput();

    printf("OCTO-Shell Emulator initialized successfully.\n");
    printf("Window size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    printf("Font sizes - Main: %d, Title: %d\n", FONT_SIZE, TITLE_FONT_SIZE);
    printf("Background enabled: %s\n", backgroundConfig.enabled ? "Yes" : "No");
    if (backgroundConfig.enabled) {
        printf("Background settings - Opacity: %d, Scale Mode: %d\n", 
               backgroundConfig.opacity, backgroundConfig.scaleMode);
    }

    // Main application loop
    while (running && !shell_should_exit())
    {
        // Process all pending events
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                        break;
                    }
                    // Fall through to input handler
                case SDL_TEXTINPUT:
                    input_handle_event(&e, inputBuffer, output, &lineCount, &cursorPos, &selection);
                    break;
                
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                    gui_handle_mouse_event(&e, output, inputBuffer, lineCount, &selection);
                    break;

                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                        printf("Window resized to: %dx%d\n", e.window.data1, e.window.data2);
                    }
                    break;
            }
        }

        // Render the current frame
        gui_render(prompt, inputBuffer, output, lineCount, cursorPos, &selection);
        
        // Control frame rate (~60 FPS)
        SDL_Delay(16);
    }

    printf("Shutting down OCTO-Shell Emulator...\n");

    // Cleanup resources
    SDL_StopTextInput();
    gui_cleanup();
    
    if (titleFont && titleFont != font) {
        TTF_CloseFont(titleFont);
        printf("Title font resources freed.\n");
    }
    
    if (font) {
        TTF_CloseFont(font);
        printf("Main font resources freed.\n");
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        printf("Renderer destroyed.\n");
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        printf("Window destroyed.\n");
    }
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    printf("OCTO-Shell Emulator terminated successfully.\n");
    
    // Keep console window open in debug mode
    #ifdef _DEBUG
    printf("Press Enter to close console...\n");
    getchar();
    #endif


    return 0;
}