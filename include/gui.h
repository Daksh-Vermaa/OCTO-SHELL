#ifndef GUI_H
#define GUI_H

#include <SDL.h>
#include <SDL_ttf.h>

void gui_init(SDL_Renderer *renderer, TTF_Font *font);
void gui_render(const char *prompt, const char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int lineCount);
void gui_handle_mouse_event(SDL_Event *e);
void gui_cleanup();
void gui_get_selection(char *buffer, int bufferSize);

#endif