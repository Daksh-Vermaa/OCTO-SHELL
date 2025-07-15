#ifndef GUI_H
#define GUI_H

#include <SDL.h>
#include <SDL_ttf.h>

void gui_init(SDL_Renderer *renderer, TTF_Font *font);
void gui_render(const char *prompt, const char *inputBuffer, char output[][2048], int lineCount);
void gui_cleanup();

#endif
