#include "config.h"
#include "gui.h"
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

static SDL_Renderer *gRenderer = NULL;
static TTF_Font *gFont = NULL;

static int selectStartY = -1;
static int selectEndY = -1;
static bool selecting = false;

static Uint32 lastCursorToggle = 0;
static bool cursorVisible = true;

void gui_init(SDL_Renderer *renderer, TTF_Font *font)
{
    gRenderer = renderer;
    gFont = font;
    lastCursorToggle = SDL_GetTicks();
    cursorVisible = true;
    selectStartY = -1;
    selectEndY = -1;
    selecting = false;
}

void render_text_colored(const char *text, int x, int y, SDL_Color fg, SDL_Color bg)
{
    if (!text || strlen(text) == 0 || !gFont || !gRenderer)
        return;

    SDL_Surface *surface = TTF_RenderText_Shaded(gFont, text, fg, bg);
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
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(gRenderer, &cursorRect);
}

void gui_render(const char *prompt, const char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int lineCount)
{
    if (!gRenderer || !gFont)
        return;

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    Uint32 now = SDL_GetTicks();
    if (now - lastCursorToggle >= 500)
    {
        cursorVisible = !cursorVisible;
        lastCursorToggle = now;
    }

    int y = 10;

    // Render output lines
    for (int i = 0; i < lineCount && i < MAX_LINES; i++)
    {
        SDL_Color fg = {255, 255, 255, 255};
        SDL_Color bg = {0, 0, 0, 255};

        int lineTop = y;
        int lineBottom = y + FONT_SIZE;

        // Check if line is selected
        if (selectStartY != -1 && selectEndY != -1)
        {
            int selMin = (selectStartY < selectEndY) ? selectStartY : selectEndY;
            int selMax = (selectStartY > selectEndY) ? selectStartY : selectEndY;

            if ((lineTop >= selMin && lineTop <= selMax) ||
                (lineBottom >= selMin && lineBottom <= selMax) ||
                (lineTop <= selMin && lineBottom >= selMax))
            {
                fg = (SDL_Color){0, 0, 0, 255};
                bg = (SDL_Color){255, 255, 255, 255};
            }
        }

        render_text_colored(output[i], 10, y, fg, bg);
        y += FONT_SIZE + 4;
    }

    // Render prompt and input
    if (prompt)
    {
        render_text_colored(prompt, 10, y, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});
    }

    if (inputBuffer)
    {
        render_text_colored(inputBuffer, 80, y, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});
    }

    // Render cursor
    int inputWidth = 0;
    if (inputBuffer && strlen(inputBuffer) > 0)
    {
        TTF_SizeText(gFont, inputBuffer, &inputWidth, NULL);
    }

    render_cursor(80 + inputWidth + 2, y);

    SDL_RenderPresent(gRenderer);
}

void gui_handle_mouse_event(SDL_Event *e)
{
    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT)
    {
        selecting = true;
        selectStartY = e->button.y;
        selectEndY = selectStartY;
    }
    else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
    {
        selecting = false;
    }
    else if (e->type == SDL_MOUSEMOTION && selecting)
    {
        selectEndY = e->motion.y;
    }
}

void gui_get_selection(char *buffer, int bufferSize)
{
    if (buffer && bufferSize > 0)
    {
        buffer[0] = '\0';
        // This is a placeholder - full implementation would extract selected text
        if (selectStartY != -1 && selectEndY != -1 && selectStartY != selectEndY)
        {
            strncpy(buffer, "Selected text", bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
        }
    }
}

void gui_cleanup()
{
    // Reset selection state
    selectStartY = -1;
    selectEndY = -1;
    selecting = false;
}