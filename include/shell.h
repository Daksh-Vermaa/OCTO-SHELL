#ifndef SHELL_H
#define SHELL_H

#include "config.h"

#define OUTPUT_LINE_SIZE 2048

extern char outputLines[MAX_LINES][2048];
extern int lineCount;

void shell_add_line(const char *line);

#endif
