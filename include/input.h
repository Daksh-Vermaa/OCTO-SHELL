#ifndef INPUT_H
#define INPUT_H
#include "config.h"
#include "gui.h"
#include <SDL.h>

void input_handle_event(SDL_Event *e, char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int *lineCount, int *cursorPos, TextSelection *selection);
void input_copy_to_clipboard(char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection);
void input_paste_from_clipboard(char *inputBuffer, int *cursorPos);
void input_select_all(char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection);

#endif