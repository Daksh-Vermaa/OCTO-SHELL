#include "shell.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "config.h"

void shell_execute(const char *input, char output[][INPUT_BUFFER_SIZE], int *lineCount)
{
    if (!input || !output || !lineCount)
        return;

    // Prevent buffer overflow
    if (*lineCount >= MAX_LINES)
    {
        // Shift lines up to make room
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
        snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "%s", input + 5);
        (*lineCount)++;
    }
    else if (strcmp(input, "help") == 0)
    {
        snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "Available commands:");
        (*lineCount)++;
        if (*lineCount < MAX_LINES) {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  clear - Clear the screen");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  echo <text> - Display text");
            (*lineCount)++;
        }
        if (*lineCount < MAX_LINES) {
            snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "  help - Show this help");
            (*lineCount)++;
        }
    }
    else if (strlen(input) == 0)
    {
        // Empty command, do nothing
        return;
    }
    else
    {
        snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "Unknown command: %s", input);
        (*lineCount)++;
    }
}