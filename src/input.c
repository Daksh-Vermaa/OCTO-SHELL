#include "input.h"
#include <string.h>
#include <SDL.h>
#include <stdio.h>
#include "shell.h"
#include "config.h"

void input_handle_event(SDL_Event *e, char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int *lineCount)
{
    if (!inputBuffer || !output || !lineCount)
        return;

    if (e->type == SDL_TEXTINPUT)
    {
        size_t currentLen = strlen(inputBuffer);
        size_t inputLen = strlen(e->text.text);
        
        // Check if there's enough space
        if (currentLen + inputLen < INPUT_BUFFER_SIZE - 1)
        {
            strcat(inputBuffer, e->text.text);
        }
    }
    else if (e->type == SDL_KEYDOWN)
    {
        SDL_Keycode key = e->key.keysym.sym;

        if (key == SDLK_BACKSPACE && strlen(inputBuffer) > 0)
        {
            inputBuffer[strlen(inputBuffer) - 1] = '\0';
        }
        else if (key == SDLK_RETURN || key == SDLK_KP_ENTER)
        {
            if (*lineCount < MAX_LINES)
            {
                snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "> %s", inputBuffer);
                (*lineCount)++;
                
                // Execute command
                shell_execute(inputBuffer, output, lineCount);
                
                // Clear input buffer
                inputBuffer[0] = '\0';
            }
        }
    }
}
