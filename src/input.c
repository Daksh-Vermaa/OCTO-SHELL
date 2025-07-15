#include "config.h" 
#include "input.h" 
#include "shell.h"  

#include <string.h> 

// DEFINING the variables
char inputBuffer[MAX_INPUT_LENGTH];
int inputLength = 0;

void input_handle_event(SDL_Event *e)
{
    if (e->type == SDL_TEXTINPUT)
    {
        if (inputLength + strlen(e->text.text) < MAX_INPUT_LENGTH)
        {
            strcat(inputBuffer, e->text.text);
            inputLength += strlen(e->text.text);
        }
    }
    else if (e->type == SDL_KEYDOWN)
    {
        if (e->key.keysym.sym == SDLK_BACKSPACE && inputLength > 0)
        {
            inputBuffer[--inputLength] = '\0';
        }
        else if (e->key.keysym.sym == SDLK_RETURN)
        {
            shell_add_line(inputBuffer);
            inputBuffer[0] = '\0';
            inputLength = 0;
        }
    }
}
