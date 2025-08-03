#ifndef GUI_H
#define GUI_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#include "config.h"

// Function declarations
void gui_init(SDL_Renderer *renderer, TTF_Font *font);
void gui_render(const char *prompt, const char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int lineCount, int cursorPos, TextSelection *selection);
void gui_handle_mouse_event(SDL_Event *e, char output[][INPUT_BUFFER_SIZE], const char *inputBuffer, int lineCount, TextSelection *selection);
void gui_get_selected_text(char output[][INPUT_BUFFER_SIZE], const char *inputBuffer, int lineCount, TextSelection *selection, char *buffer, int bufferSize);
void gui_cleanup(void);

// Basic functions
void render_text_colored(const char *text, int x, int y, SDL_Color fg, SDL_Color bg);
void render_selection_highlight(int x, int y, int width, int height);
void render_cursor(int x, int y);
int gui_get_char_width(void);

// Word wrap functions
void wrap_text(const char *text, char wrapped[][INPUT_BUFFER_SIZE], int *wrappedLineCount, int maxWidth);
int get_text_width_in_chars(void);

#endif