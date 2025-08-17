#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include <stdbool.h>

#include <SDL.h>

#include "config.h"

#include "shell.h"

#include "gui.h"

// External declaration for wordWrapEnabled (defined in main.c)
extern int wordWrapEnabled;

static bool shouldExit = false;
static bool exitRequested = false;
static Uint32 exitRequestTime = 0;

void shell_execute(const char * input, char output[][INPUT_BUFFER_SIZE], int * lineCount) {
  if (!input || !output || !lineCount)
    return;

  // Scroll output if buffer is full
  if ( * lineCount >= MAX_LINES) {
    for (int i = 0; i < MAX_LINES - 1; i++) {
      strcpy(output[i], output[i + 1]);
    }
    ( * lineCount) --;
  }

  // Trim whitespace from input
  char trimmedInput[INPUT_BUFFER_SIZE];
  const char * start = input;
  while ( * start && isspace((unsigned char) * start)) start++;

  const char * end = start + strlen(start) - 1;
  while (end > start && isspace((unsigned char) * end)) end--;

  size_t len = end - start + 1;
  if (len >= INPUT_BUFFER_SIZE) len = INPUT_BUFFER_SIZE - 1;
  strncpy(trimmedInput, start, len);
  trimmedInput[len] = '\0';

  if (strcmp(trimmedInput, "clear") == 0) {
    * lineCount = 0;
    return;
  } else if (strncmp(trimmedInput, "echo ", 5) == 0) {
    if ( * lineCount < MAX_LINES) {
      const char * echoText = trimmedInput + 5;
      // Skip leading spaces after "echo"
      while ( * echoText == ' ') echoText++;

      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "\033[32m%s\033[0m", echoText);
      ( * lineCount) ++;
    }
  } else if (strncmp(trimmedInput, "wordwrap ", 9) == 0) {
    const char * value = trimmedInput + 9;

    // Skip whitespace
    while ( * value == ' ') value++;

    if (strcmp(value, "true") == 0 || strcmp(value, "on") == 0 || strcmp(value, "1") == 0) {
      wordWrapEnabled = 1;
      if ( * lineCount < MAX_LINES) {
        snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "\033[32mWord-wrap Enabled \033[0m");
        ( * lineCount) ++;
      }
    } else if (strcmp(value, "false") == 0 || strcmp(value, "off") == 0 || strcmp(value, "0") == 0) {
      wordWrapEnabled = 0;
      if ( * lineCount < MAX_LINES) {
        snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Word wrap disabled");
        ( * lineCount) ++;
      }
    } else {
      if ( * lineCount < MAX_LINES) {
        snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Invalid wordwrap value. Use: true/false, on/off, or 1/0");
        ( * lineCount) ++;
      }
    }
  } else if (strcmp(trimmedInput, "wordwrap") == 0) {
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Word wrap is currently: %s",
        wordWrapEnabled ? "enabled" : "disabled");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Usage: wordwrap <true/false>");
      ( * lineCount) ++;
    }
  } else if (strncmp(trimmedInput, "background ", 11) == 0 || strncmp(trimmedInput, "bg ", 3) == 0) {
    const char * args = trimmedInput + (strncmp(trimmedInput, "bg ", 3) == 0 ? 3 : 11);
    while ( * args == ' ') args++; // Skip whitespace

    if (strncmp(args, "set ", 4) == 0) {
      const char * imagePath = args + 4;
      while ( * imagePath == ' ') imagePath++; // Skip whitespace

      gui_set_background_image(imagePath);
      if ( * lineCount < MAX_LINES) {
        snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Background image set to: %s", imagePath);
        ( * lineCount) ++;
      }
    } else if (strncmp(args, "opacity ", 8) == 0) {
      const char * opacityStr = args + 8;
      while ( * opacityStr == ' ') opacityStr++; // Skip whitespace

      float opacity = atof(opacityStr);
      gui_set_background_opacity(opacity);
      if ( * lineCount < MAX_LINES) {
        snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Background opacity set to: %.2f", opacity);
        ( * lineCount) ++;
      }
    } else if (strcmp(args, "clear") == 0) {
      gui_cleanup_background();
      if ( * lineCount < MAX_LINES) {
        snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Background image cleared");
        ( * lineCount) ++;
      }
    } else {
      if ( * lineCount < MAX_LINES) {
        snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Usage: background <set path|opacity value|clear>");
        ( * lineCount) ++;
      }
    }
  }  else if (strcmp(trimmedInput, "background") == 0 || strcmp(trimmedInput, "bg") == 0) {
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Background commands:");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  bg set <path> - Set background image");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  bg clear - Remove background");
      ( * lineCount) ++;
    }
  } else if (strcmp(trimmedInput, "version") == 0) {
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "OCTO-SHELL Emulator v2.1");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Built by Daksh Verma with SDL2");
      ( * lineCount) ++;
    }
  } else if (strcmp(trimmedInput, "help") == 0) {
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Available commands:");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  clear - Clear the screen");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  echo <text> - Display text");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  wordwrap <true/false> - Toggle word wrapping");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  bg - Background image commands");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  version - Show version information");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  help - Show this help");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  shortcuts - Show keyboard shortcuts");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  exit/quit - Close the application");
      ( * lineCount) ++;
    }
  } else if (strcmp(trimmedInput, "shortcuts") == 0) {
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Keyboard shortcuts:");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  Ctrl+C - Copy selected text");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  Ctrl+V - Paste text");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  Ctrl+A - Select all text");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  Ctrl+Z - Undo last action");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  Ctrl+Y - Redo last undone action");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  Arrow keys - Move cursor");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  Home/End Keys - Jump to start/end of line");
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "  Escape Key - Close application");
      ( * lineCount) ++;
    }
  } else if (strcmp(trimmedInput, "exit") == 0 || strcmp(trimmedInput, "quit") == 0) {
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Goodbye! Closing OCTO-Shell...");
      ( * lineCount) ++;
    }
    exitRequested = true;
    exitRequestTime = SDL_GetTicks();
  } else if (strlen(trimmedInput) == 0) {
    // Empty command - do nothing
    return;
  } else {
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Unknown command: %s", trimmedInput);
      ( * lineCount) ++;
    }
    if ( * lineCount < MAX_LINES) {
      snprintf(output[ * lineCount], INPUT_BUFFER_SIZE, "Type 'help' for available commands.");
      ( * lineCount) ++;
    }
  }
}

bool shell_should_exit() {
  if (exitRequested && !shouldExit) {
    // Check if 1 second has passed since exit was requested
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - exitRequestTime >= 1000) // 1000ms = 1 second
    {
      shouldExit = true;
    }
  }
  return shouldExit;
}

void shell_reset_exit_flag() {
  shouldExit = false;
  exitRequested = false;
  exitRequestTime = 0;
}