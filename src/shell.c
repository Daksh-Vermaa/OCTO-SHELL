#include "shell.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "config.h"

static bool shouldExit = false;

void shell_execute(const char *input, char output[][INPUT_BUFFER_SIZE], int *lineCount)
{
    if (!input || !output || !lineCount)
        return;

    // Scroll output if buffer is full
    if (*lineCount >= MAX_LINES)
    {
        for (int i = 0; i < MAX_LINES - 1; i++)
        {
            strcpy(output[i], output[i + 1]);
        }
        (*lineCount)--;
    }

    if (strcmp(input, "clear") == 0)
    {
        *lineCount = 0;
        return;
    }
    else if (strncmp(input, "echo ", 5) == 0)
    {
        if (*lineCount < MAX_LINES)
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "%s", input + 5);
            (*lineCount)++;
        }
    }
    else if (strcmp(input, "help") == 0)
    {
        if (*lineCount < MAX_LINES)
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "Available commands:");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  clear - Clear the screen");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  echo <text> - Display text");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  help - Show this help");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  shortcuts - Show keyboard shortcuts");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  exit/quit - Close the application");
            (*lineCount)++;
        }
    }
    else if (strcmp(input, "shortcuts") == 0) 
    {
        if (*lineCount < MAX_LINES)
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "Keyboard shortcuts:");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  Ctrl+C - Copy selected text");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  Ctrl+V - Paste text");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  Ctrl+A - Select all text");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  Arrow keys - Move cursor");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  Home/End Keys - Jump to start/end of line");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) 
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  Escape Key - Close application");
            (*lineCount)++;
        }
    }
    else if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
    {
        if (*lineCount < MAX_LINES)
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "Goodbye! Closing OCTO-Shell...");
            (*lineCount)++;
        }
        shouldExit = true;
    }
    else if (strlen(input) == 0)
    {
        // Empty command - do nothing
        return;
    }
    else
    {
        if (*lineCount < MAX_LINES)
        {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "Unknown command: %s", input);
            (*lineCount)++;
        }
    }
}

bool shell_should_exit()
{
    return shouldExit;
}

void shell_reset_exit_flag()
{
    shouldExit = false;
}