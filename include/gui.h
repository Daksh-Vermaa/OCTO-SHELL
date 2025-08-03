#ifndef GUI_H
#define GUI_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "config.h"

typedef struct {
    int startLine;
    int startChar;
    int endLine;
    int endChar;
    bool active;
} TextSelection;

void gui_init(SDL_Renderer *renderer, TTF_Font *font);
void gui_render(const char *prompt, const char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int lineCount, int cursorPos, TextSelection *selection);
void gui_handle_mouse_event(SDL_Event *e, char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection);
void gui_cleanup();
void gui_get_selected_text(char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection, char *buffer, int bufferSize);
int gui_get_char_width();

#endif