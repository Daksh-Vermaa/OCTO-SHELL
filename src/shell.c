#include "config.h"
#include "shell.h"
#include <string.h>

char outputLines[MAX_LINES][OUTPUT_LINE_SIZE];
int lineCount = 0;

void shell_add_line(const char *line)
{
    if (lineCount < MAX_LINES)
    {
        strncpy(outputLines[lineCount++], line, OUTPUT_LINE_SIZE - 1);
    }
}