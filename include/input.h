#ifndef INPUT_H
#define INPUT_H

#include "config.h"
#include <SDL.h>

extern char inputBuffer[MAX_INPUT_LENGTH];
extern int inputLength;

void input_handle_event(SDL_Event *e);

#endif
