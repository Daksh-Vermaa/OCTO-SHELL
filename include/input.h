#ifndef INPUT_H
#define INPUT_H
#include "config.h"

#include <SDL.h>

void input_handle_event(SDL_Event *e, char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int *lineCount);

#endif