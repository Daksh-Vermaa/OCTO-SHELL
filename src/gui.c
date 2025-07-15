#include "config.h"
#include "gui.h"
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

static SDL_Renderer *gRenderer = NULL;
static TTF_Font *gFont = NULL;

// ⏳ Cursor blink variables
static Uint32 lastCursorToggle = 0;
static bool cursorVisible = true;

void gui_init(SDL_Renderer *renderer, TTF_Font *font)
{
    gRenderer = renderer;
    gFont = font;
    lastCursorToggle = SDL_GetTicks();
    cursorVisible = true;
}

void render_text(const char *text, int x, int y)
{
    if (!text || strlen(text) == 0 || !gFont || !gRenderer)
        return;

    SDL_Color white = {255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(gFont, text, white);
    if (!surface)
        return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    if (!texture)
    {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(gRenderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render_cursor(int x, int y)
{
    if (!cursorVisible || !gRenderer)
        return;

    int cursorWidth = 2;
    int cursorHeight = FONT_SIZE;

    SDL_Rect cursorRect = {x, y, cursorWidth, cursorHeight};
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255); // white
    SDL_RenderFillRect(gRenderer, &cursorRect);
}

void gui_render(const char *prompt, const char *inputBuffer, char output[][2048], int lineCount)
{
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    // Toggle cursor every 500 ms
    Uint32 now = SDL_GetTicks();
    if (now - lastCursorToggle >= 500)
    {
        cursorVisible = !cursorVisible;
        lastCursorToggle = now;
    }

    int y = 10;

    for (int i = 0; i < lineCount && i < MAX_LINES; i++)
    {
        render_text(output[i], 10, y);
        y += FONT_SIZE + 4;
    }

    render_text(prompt, 10, y);
    render_text(inputBuffer, 80, y);

    // Get width of current input for cursor position
    int inputWidth = 0;
    if (strlen(inputBuffer) > 0)
    {
        TTF_SizeText(gFont, inputBuffer, &inputWidth, NULL);
    }

    render_cursor(80 + inputWidth + 2, y);

    SDL_RenderPresent(gRenderer);
}

void gui_cleanup()
{
    // For future resource freeing
}
