#include "config.h"
#include "gui.h"
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

static SDL_Renderer *gRenderer = NULL;
static TTF_Font *gFont = NULL;

static Uint32 lastCursorToggle = 0;
static bool cursorVisible = true;

void gui_init(SDL_Renderer *renderer, TTF_Font *font)
{
    gRenderer = renderer;
    gFont = font;
    lastCursorToggle = SDL_GetTicks();
    cursorVisible = true;
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

void render_selection_highlight(int x, int y, int width, int height)
{
    SDL_Rect selectionRect = {x, y, width, height};
    SDL_SetRenderDrawColor(gRenderer, 100, 150, 255, 128);
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(gRenderer, &selectionRect);
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
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

int gui_get_char_width()
{
    if (!gFont) return 8;
    int w;
    TTF_SizeText(gFont, "W", &w, NULL);
    return w;
}

void gui_render(const char *prompt, const char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int lineCount, int cursorPos, TextSelection *selection)
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
    int charWidth = gui_get_char_width();

    // Render output lines with selection highlighting
    for (int i = 0; i < lineCount && i < MAX_LINES; i++)
    {
        SDL_Color fg = {255, 255, 255, 255};
        SDL_Color bg = {0, 0, 0, 255};
        
        // Check if this line is selected
        if (selection && selection->active)
        {
            bool lineSelected = false;
            int selStart = 0, selEnd = 0;

            if (i >= selection->startLine && i <= selection->endLine)
            {
                lineSelected = true;
                if (i == selection->startLine && i == selection->endLine)
                {
                    // Selection within same line
                    selStart = selection->startChar;
                    selEnd = selection->endChar;
                }
                else if (i == selection->startLine)
                {
                    // First line of multi-line selection
                    selStart = selection->startChar;
                    selEnd = (int)strlen(output[i]);
                }
                else if (i == selection->endLine)
                {
                    // Last line of multi-line selection
                    selStart = 0;
                    selEnd = selection->endChar;
                }
                else
                {
                    // Middle line of multi-line selection
                    selStart = 0;
                    selEnd = (int)strlen(output[i]);
                }
            }

            if (lineSelected && selStart < selEnd)
            {
                // Render selection highlight
                int highlightX = 10 + selStart * charWidth;
                int highlightWidth = (selEnd - selStart) * charWidth;
                render_selection_highlight(highlightX, y, highlightWidth, FONT_SIZE);
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

    // Render cursor at cursor position
    int cursorX = 80 + cursorPos * charWidth;
    render_cursor(cursorX, y);

    SDL_RenderPresent(gRenderer);
}

void gui_handle_mouse_event(SDL_Event *e, char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection)
{
    if (!selection) return;

    int charWidth = gui_get_char_width();
    int lineHeight = FONT_SIZE + 4;

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT)
    {
        int clickedLine = (e->button.y - 10) / lineHeight;
        int clickedChar = (e->button.x - 10) / charWidth;

        if (clickedLine >= 0 && clickedLine < lineCount)
        {
            // Clamp character position to line length
            int lineLen = (int)strlen(output[clickedLine]);
            if (clickedChar < 0) clickedChar = 0;
            if (clickedChar > lineLen) clickedChar = lineLen;

            selection->active = true;
            selection->startLine = clickedLine;
            selection->startChar = clickedChar;
            selection->endLine = clickedLine;
            selection->endChar = clickedChar;
        }
    }
    else if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
    {
        if (selection->active)
        {
            int currentLine = (e->button.y - 10) / lineHeight;
            int currentChar = (e->button.x - 10) / charWidth;

            if (currentLine >= 0 && currentLine < lineCount)
            {
                // Clamp character position to line length
                int lineLen = (int)strlen(output[currentLine]);
                if (currentChar < 0) currentChar = 0;
                if (currentChar > lineLen) currentChar = lineLen;

                selection->endLine = currentLine;
                selection->endChar = currentChar;
            }
            
            // If start and end are the same, deactivate selection
            if (selection->startLine == selection->endLine && 
                selection->startChar == selection->endChar)
            {
                selection->active = false;
            }
        }
    }
    else if (e->type == SDL_MOUSEMOTION && (e->motion.state & SDL_BUTTON_LMASK))
    {
        if (selection->active)
        {
            int currentLine = (e->motion.y - 10) / lineHeight;
            int currentChar = (e->motion.x - 10) / charWidth;

            if (currentLine >= 0 && currentLine < lineCount)
            {
                // Clamp character position to line length
                int lineLen = (int)strlen(output[currentLine]);
                if (currentChar < 0) currentChar = 0;
                if (currentChar > lineLen) currentChar = lineLen;

                selection->endLine = currentLine;
                selection->endChar = currentChar;
            }
        }
    }
}

void gui_get_selected_text(char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection, char *buffer, int bufferSize)
{
    if (!selection || !selection->active || !buffer || bufferSize <= 0)
    {
        if (buffer) buffer[0] = '\0';
        return;
    }

    buffer[0] = '\0';
    int pos = 0;

    // Ensure proper order (start <= end)
    int startLine = selection->startLine;
    int startChar = selection->startChar;
    int endLine = selection->endLine;
    int endChar = selection->endChar;

    if (startLine > endLine || (startLine == endLine && startChar > endChar))
    {
        // Swap if selection is backwards
        int tempLine = startLine;
        int tempChar = startChar;
        startLine = endLine;
        startChar = endChar;
        endLine = tempLine;
        endChar = tempChar;
    }

    for (int i = startLine; i <= endLine && i < lineCount; i++)
    {
        int copyStartChar = (i == startLine) ? startChar : 0;
        int copyEndChar = (i == endLine) ? endChar : (int)strlen(output[i]);

        if (copyStartChar < copyEndChar)
        {
            int copyLen = copyEndChar - copyStartChar;
            if (pos + copyLen < bufferSize - 1)
            {
                strncpy(buffer + pos, output[i] + copyStartChar, copyLen);
                pos += copyLen;
            }
        }

        if (i < endLine && pos < bufferSize - 2)
        {
            buffer[pos++] = '\n'; // Add newline between lines
        }
    }

    buffer[pos] = '\0';
}

void gui_cleanup()
{
    // Cleanup if needed
}