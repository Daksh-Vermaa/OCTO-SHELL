#include "config.h"
#include "gui.h"
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <ctype.h>

static SDL_Renderer *gRenderer = NULL;
static TTF_Font *gFont = NULL;
static TTF_Font *gTitleFont = NULL;

static Uint32 lastCursorToggle = 0;
static bool cursorVisible = true;

// Cursor management
static SDL_Cursor *arrowCursor = NULL;
static SDL_Cursor *ibeamCursor = NULL;
static bool isIbeamCursorActive = false;

// Word wrap global variable
extern int wordWrapEnabled; 

void gui_init(SDL_Renderer *renderer, TTF_Font *font)
{
    gRenderer = renderer;
    gFont = font;
    gTitleFont = NULL; // Will be set separately
    lastCursorToggle = SDL_GetTicks();
    cursorVisible = true;
    
    // Initialize cursors
    arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    ibeamCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    isIbeamCursorActive = false;
}

void gui_set_title_font(TTF_Font *titleFont)
{
    gTitleFont = titleFont;
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

void render_centered_title(const char *title, int y)
{
    if (!title || strlen(title) == 0 || !gRenderer)
        return;

    // Use title font if available, otherwise use regular font
    TTF_Font *fontToUse = gTitleFont ? gTitleFont : gFont;
    if (!fontToUse)
        return;

    // Create colored text surface
    SDL_Color titleColor = {0, 255, 255, 255}; // Cyan color for title
    SDL_Color bgColor = {0, 0, 0, 255};        // Black background
    
    SDL_Surface *surface = TTF_RenderText_Shaded(fontToUse, title, titleColor, bgColor);
    if (!surface)
        return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    if (!texture)
    {
        SDL_FreeSurface(surface);
        return;
    }

    // Calculate centered position
    int textWidth = surface->w;
    int textHeight = surface->h;
    int centerX = (WINDOW_WIDTH - textWidth) / 2;
    
    SDL_Rect dst = {centerX, y, textWidth, textHeight};
    SDL_RenderCopy(gRenderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render_selection_highlight(int x, int y, int width, int height)
{
    if (!gRenderer || width <= 0 || height <= 0)
        return;
        
     SDL_Rect selectionRect = {x, y, width, height};
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gRenderer, 0, 120, 215, 150); // Blue with alpha
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

int get_text_width_in_chars(void)
{
    int charWidth = gui_get_char_width();
    int availableWidth = WINDOW_WIDTH - 20; // Accounting for margins
    return availableWidth / charWidth;
}

void wrap_text(const char *text, char wrapped[][INPUT_BUFFER_SIZE], int *wrappedLineCount, int maxWidth)
{
    if (!text || !wrapped || !wrappedLineCount)
        return;

    *wrappedLineCount = 0;
    int textLen = strlen(text);
    
    if (textLen == 0)
        return;

    int currentLine = 0;
    int currentPos = 0;
    int i = 0;

    while (i < textLen && currentLine < MAX_LINES)
    {
        int lineStart = i;
        int lastSpace = -1;
        int lineLength = 0;

        // Find the end of the current line
        while (i < textLen && lineLength < maxWidth)
        {
            if (text[i] == ' ')
                lastSpace = i;
            
            if (text[i] == '\n')
            {
                // Force line break
                break;
            }
            
            lineLength++;
            i++;
        }

        // Determine where to break the line
        int lineEnd;
        if (i >= textLen || text[i] == '\n')
        {
            // End of text or explicit newline
            lineEnd = i;
        }
        else if (lineLength >= maxWidth)
        {
            // Need to wrap
            if (lastSpace != -1 && lastSpace > lineStart)
            {
                // Break at last space
                lineEnd = lastSpace;
                i = lastSpace + 1; // Skip the space
            }
            else
            {
                // No space found, break at max width
                lineEnd = i;
            }
        }
        else
        {
            lineEnd = i;
        }

        // Copy the line
        int copyLength = lineEnd - lineStart;
        if (copyLength > INPUT_BUFFER_SIZE - 1)
            copyLength = INPUT_BUFFER_SIZE - 1;

        strncpy(wrapped[currentLine], text + lineStart, copyLength);
        wrapped[currentLine][copyLength] = '\0';

        currentLine++;
        
        // Skip newline character if present
        if (i < textLen && text[i] == '\n')
            i++;
    }

    *wrappedLineCount = currentLine;
}

bool gui_is_point_in_text_area(int mouseX, int mouseY, int lineCount)
{
    int textAreaLeft = 10;
    int textAreaRight = WINDOW_WIDTH - 10;
    int textAreaTop = 10;
    int lineHeight = FONT_SIZE + 4;
    int textAreaBottom = 10 + (lineCount + 1) * lineHeight;
    
    return (mouseX >= textAreaLeft && mouseX <= textAreaRight && 
            mouseY >= textAreaTop && mouseY <= textAreaBottom);
}

void gui_update_cursor(int mouseX, int mouseY, int lineCount)
{
    bool shouldShowIbeam = gui_is_point_in_text_area(mouseX, mouseY, lineCount);
    
    if (shouldShowIbeam && !isIbeamCursorActive)
    {
        SDL_SetCursor(ibeamCursor);
        isIbeamCursorActive = true;
    }
    else if (!shouldShowIbeam && isIbeamCursorActive)
    {
        SDL_SetCursor(arrowCursor);
        isIbeamCursorActive = false;
    }
}

void gui_render(const char *prompt, const char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int lineCount, int cursorPos, TextSelection *selection)
{
    if (!gRenderer || !gFont) return;

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    Uint32 now = SDL_GetTicks();
    if (now - lastCursorToggle >= 500) {
        cursorVisible = !cursorVisible;
        lastCursorToggle = now;
    }

    int y = 10;
    int charWidth = gui_get_char_width();
    int maxWidth = get_text_width_in_chars();

    // === Render Centered Title First ===
    render_centered_title(TITLE_TEXT, y);
    
    // Calculate title height to adjust starting position for content
    int titleHeight = TITLE_FONT_SIZE + 10; // Add some padding
    y += titleHeight;

    // === 1. Render Output Lines + Highlights ===
    int displayLineCount = 0;
    
    for (int i = 0; i < lineCount && i < MAX_LINES; i++) {
        SDL_Color fg = {255, 255, 255, 255};
        SDL_Color bg = {0, 0, 0, 255};

        if (wordWrapEnabled) {
            char wrappedLines[MAX_LINES][INPUT_BUFFER_SIZE];
            int wrappedCount = 0;
            wrap_text(output[i], wrappedLines, &wrappedCount, maxWidth);
            
            for (int j = 0; j < wrappedCount; j++) {
                render_text_colored(wrappedLines[j], 10, y, fg, bg);
                
                // Handle selection highlighting for wrapped lines
                if (selection && selection->active) {
                    int startLine = selection->startLine;
                    int endLine = selection->endLine;
                    int startChar = selection->startChar;
                    int endChar = selection->endChar;

                    if (startLine > endLine || (startLine == endLine && startChar > endChar)) {
                        int tmpLine = startLine, tmpChar = startChar;
                        startLine = endLine; startChar = endChar;
                        endLine = tmpLine; endChar = tmpChar;
                    }

                    if (i >= startLine && i <= endLine) {
                        int selStart = (i == startLine) ? startChar : 0;
                        int selEnd = (i == endLine) ? endChar : (int)strlen(output[i]);
                        
                        // Adjust selection for this wrapped line segment
                        int segmentStart = j * maxWidth;
                        int segmentEnd = segmentStart + strlen(wrappedLines[j]);
                        
                        if (selStart < segmentEnd && selEnd > segmentStart) {
                            int highlightStart = (selStart > segmentStart) ? selStart - segmentStart : 0;
                            int highlightEnd = (selEnd < segmentEnd) ? selEnd - segmentStart : strlen(wrappedLines[j]);
                            
                            if (highlightStart < highlightEnd) {
                                int highlightX = 10 + highlightStart * charWidth;
                                int highlightWidth = (highlightEnd - highlightStart) * charWidth;
                                render_selection_highlight(highlightX, y, highlightWidth, FONT_SIZE);
                            }
                        }
                    }
                }
                
                y += FONT_SIZE + 4;
                displayLineCount++;
            }
        } else {
            // Original non-wrapped rendering
            render_text_colored(output[i], 10, y, fg, bg);

            if (selection && selection->active) {
                int startLine = selection->startLine;
                int endLine = selection->endLine;
                int startChar = selection->startChar;
                int endChar = selection->endChar;

                if (startLine > endLine || (startLine == endLine && startChar > endChar)) {
                    int tmpLine = startLine, tmpChar = startChar;
                    startLine = endLine; startChar = endChar;
                    endLine = tmpLine; endChar = tmpChar;
                }

                if (i >= startLine && i <= endLine) {
                    int selStart = (i == startLine) ? startChar : 0;
                    int selEnd = (i == endLine) ? endChar : (int)strlen(output[i]);
                    
                    int lineLen = (int)strlen(output[i]);
                    if (selStart > lineLen) selStart = lineLen;
                    if (selEnd > lineLen) selEnd = lineLen;

                    if (selStart < selEnd) {
                        int highlightX = 10 + selStart * charWidth;
                        int highlightWidth = (selEnd - selStart) * charWidth;
                        render_selection_highlight(highlightX, y, highlightWidth, FONT_SIZE);
                    }
                }
            }
            y += FONT_SIZE + 4;
            displayLineCount++;
        }
    }

    // === 2. Render Input Line + Highlight ===
    if (prompt)
        render_text_colored(prompt, 10, y, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});

    if (inputBuffer) {
        render_text_colored(inputBuffer, 80, y, (SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255});
        
        if (selection && selection->active) {
            int startLine = selection->startLine;
            int endLine = selection->endLine;
            int startChar = selection->startChar;
            int endChar = selection->endChar;

            if (startLine > endLine || (startLine == endLine && startChar > endChar)) {
                int tmpLine = startLine, tmpChar = startChar;
                startLine = endLine; startChar = endChar;
                endLine = tmpLine; endChar = tmpChar;
            }

            if (lineCount >= startLine && lineCount <= endLine) {
                int selStart = (lineCount == startLine) ? startChar : 0;
                int selEnd = (lineCount == endLine) ? endChar : (int)strlen(inputBuffer);
                int lineLen = (int)strlen(inputBuffer);
                if (selStart > lineLen) selStart = lineLen;
                if (selEnd > lineLen) selEnd = lineLen;

                 if (selStart < selEnd) {
                    int highlightX = 80 + selStart * charWidth;
                    int highlightWidth = (selEnd - selStart) * charWidth;
                    render_selection_highlight(highlightX, y, highlightWidth, FONT_SIZE);
                }
            }
        }
    }

    // === 3. Cursor ===
    int cursorX = 80 + cursorPos * charWidth;
    render_cursor(cursorX, y);

    SDL_RenderPresent(gRenderer);
}

void gui_handle_mouse_event(SDL_Event *e, char output[][INPUT_BUFFER_SIZE], const char *inputBuffer, int lineCount, TextSelection *selection)
{
    if (!selection) return;

    int charWidth = gui_get_char_width();
    int lineHeight = FONT_SIZE + 4;
    
    // Adjust for title offset
    int titleOffset = TITLE_FONT_SIZE + 10;
    int inputLineY = 10 + titleOffset + lineCount * lineHeight;

    // Update cursor appearance based on mouse position
    if (e->type == SDL_MOUSEMOTION)
    {
        gui_update_cursor(e->motion.x, e->motion.y, lineCount);
    }
    
    int mouseX, mouseY;
    if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
        mouseX = e->button.x;
        mouseY = e->button.y;
    } else if (e->type == SDL_MOUSEMOTION) {
        mouseX = e->motion.x;
        mouseY = e->motion.y;
    } else {
        return; // Not a mouse event we handle for selection
    }

    // Adjust mouse Y position for title offset
    int adjustedMouseY = mouseY - titleOffset;

    // Determine clicked line and character
    int clickedLine, clickedChar;
    if (adjustedMouseY >= inputLineY - titleOffset && adjustedMouseY < inputLineY - titleOffset + lineHeight) {
        clickedLine = lineCount; // Input line
        clickedChar = (mouseX - 80) / charWidth;
    } else {
        clickedLine = (adjustedMouseY - 10) / lineHeight; // Output line
        clickedChar = (mouseX - 10) / charWidth;
    }

    // Get the correct line length for clamping
    int lineLen = 0;
    if (clickedLine == lineCount) {
        lineLen = (int)strlen(inputBuffer);
    } else if (clickedLine >= 0 && clickedLine < lineCount) {
        lineLen = (int)strlen(output[clickedLine]);
    }
    
    // Clamp character position
    if (clickedChar < 0) clickedChar = 0;
    if (clickedChar > lineLen) clickedChar = lineLen;

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT)
    {
        if (clickedLine >= 0 && clickedLine <= lineCount) {
            selection->active = 1; // Set active on down press
            selection->startLine = clickedLine;
            selection->startChar = clickedChar;
            selection->endLine = clickedLine;
            selection->endChar = clickedChar;
        } else {
            selection->active = 0; // Clicked outside text area
        }
    }
    else if (e->type == SDL_MOUSEMOTION && (e->motion.state & SDL_BUTTON_LMASK))
    {
        if (selection->active) { // Only drag if selection has been started
             if (clickedLine >= 0 && clickedLine <= lineCount) {
                selection->endLine = clickedLine;
                selection->endChar = clickedChar;
             }
        }
    }
}


void gui_get_selected_text(char output[][INPUT_BUFFER_SIZE], const char *inputBuffer, int lineCount, TextSelection *selection, char *buffer, int bufferSize)
{
    if (!selection || !selection->active || !buffer || bufferSize <= 0)
    {
        if (buffer) buffer[0] = '\0';
        return;
    }

    buffer[0] = '\0';
    int pos = 0;

    int startLine = selection->startLine;
    int startChar = selection->startChar;
    int endLine = selection->endLine;
    int endChar = selection->endChar;

    if (startLine > endLine || (startLine == endLine && startChar > endChar))
    {
        int tempLine = startLine;
        int tempChar = startChar;
        startLine = endLine;
        startChar = endChar;
        endLine = tempLine;
        endChar = tempChar;
    }

    for (int i = startLine; i <= endLine && i <= lineCount; i++)
    {
        const char *lineText = (i == lineCount) ? inputBuffer : output[i];
        int copyStartChar = (i == startLine) ? startChar : 0;
        int copyEndChar = (i == endLine) ? endChar : (int)strlen(lineText);

        int lineLen = (int)strlen(lineText);
        if (copyStartChar > lineLen) copyStartChar = lineLen;
        if (copyEndChar > lineLen) copyEndChar = lineLen;
        
        if (copyStartChar < copyEndChar)
        {
            int copyLen = copyEndChar - copyStartChar;
            if (pos + copyLen < bufferSize - 1)
            {
                strncpy(buffer + pos, lineText + copyStartChar, copyLen);
                pos += copyLen;
            }
        }

        if (i < endLine && pos < bufferSize - 2)
        {
            buffer[pos++] = '\n';
        }
    }

    buffer[pos] = '\0';
}

void gui_cleanup()
{
    if (arrowCursor)
    {
        SDL_FreeCursor(arrowCursor);
        arrowCursor = NULL;
    }
    if (ibeamCursor)
    {
        SDL_FreeCursor(ibeamCursor);
        ibeamCursor = NULL;
    }
}