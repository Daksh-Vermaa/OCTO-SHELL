#ifndef SHELL_H
#define SHELL_H
#include "config.h"
#include <stdbool.h>

void shell_execute(const char *input, char output[][INPUT_BUFFER_SIZE], int *lineCount);
bool shell_should_exit(void);
void shell_reset_exit_flag(void);

#endif