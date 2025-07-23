#ifndef SHELL_H
#define SHELL_H
#include "config.h"


void shell_execute(const char *input, char output[][INPUT_BUFFER_SIZE], int *lineCount);

#endif