#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>

#include "config.h"

// Function declarations
void input_handle_event(SDL_Event * e, char * inputBuffer, char output[][INPUT_BUFFER_SIZE], int * lineCount, int * cursorPos, TextSelection * selection);
void input_copy_to_clipboard(char output[][INPUT_BUFFER_SIZE],
  const char * inputBuffer, int lineCount, TextSelection * selection);
void input_paste_from_clipboard(char * inputBuffer, int * cursorPos);
void input_select_all(char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection * selection);

// undo/redo functions
void save_input_state(const char * inputBuffer, int cursorPos);
void input_undo(char * inputBuffer, int * cursorPos);
void input_redo(char * inputBuffer, int * cursorPos);

#endif