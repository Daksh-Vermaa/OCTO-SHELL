#include <string.h>

#include <stdbool.h>

#include <stdio.h>

#include <SDL.h>

#include <SDL_ttf.h>

#include <SDL_image.h>

#include <ctype.h>

#include "config.h"

#include "gui.h"

static SDL_Renderer * gRenderer = NULL;
static TTF_Font * gFont = NULL;
static TTF_Font * gTitleFont = NULL;
static SDL_Window * gWindow = NULL;

static Uint32 lastCursorToggle = 0;
static bool cursorVisible = true;

// Cursor management
static SDL_Cursor * arrowCursor = NULL;
static SDL_Cursor * ibeamCursor = NULL;
static bool isIbeamCursorActive = false;

static SDL_Texture * gBackgroundTexture = NULL;
static float gBackgroundOpacity = 1.0f;

// Word wrap global variable
extern int wordWrapEnabled;

int gui_set_background_image(const char * imagePath) {
  if (!gRenderer || !imagePath) return 0;

  // Clean up existing background texture
  if (gBackgroundTexture) {
    SDL_DestroyTexture(gBackgroundTexture);
    gBackgroundTexture = NULL;
  }

  // Load new background image using SDL_image for better format support
  SDL_Surface * surface = IMG_Load(imagePath);
  if (!surface) {
    printf("Warning: Could not load background image: %s\n", IMG_GetError());
    return 0;
  }

  gBackgroundTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
  SDL_FreeSurface(surface);

  if (!gBackgroundTexture) {
    printf("Warning: Could not create background texture: %s\n", SDL_GetError());
    return 0;
  }

  return 1; // Success
}

void gui_set_background_opacity(float opacity) {
  if (opacity < 0.0f) opacity = 0.0f;
  if (opacity > 1.0f) opacity = 1.0f;
  gBackgroundOpacity = opacity;
}

void gui_cleanup_background(void) {
  if (gBackgroundTexture) {
    SDL_DestroyTexture(gBackgroundTexture);
    gBackgroundTexture = NULL;
  }
}

void gui_init(SDL_Renderer * renderer, TTF_Font * font) {
  if (!renderer || !font) return;

  gRenderer = renderer;
  gFont = font;
  gTitleFont = NULL;
  lastCursorToggle = SDL_GetTicks();
  cursorVisible = true;

  gWindow = SDL_RenderGetWindow(renderer);

  // Initialize cursors
  arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  ibeamCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
  isIbeamCursorActive = false;
}

void gui_set_title_font(TTF_Font * titleFont) {
  gTitleFont = titleFont;
}

void gui_get_window_size(int * width, int * height) {
  if (gWindow && width && height) {
    SDL_GetWindowSize(gWindow, width, height);
  } else {
    if (width) * width = WINDOW_WIDTH;
    if (height) * height = WINDOW_HEIGHT;
  }
}

void render_text_colored(const char * text, int x, int y, SDL_Color fg, SDL_Color bg) {
  if (!text || strlen(text) == 0 || !gFont || !gRenderer)
    return;

  // Check for command color codes
  SDL_Color textColor = fg;
  const char * displayText = text;

  if (strstr(text, "\033[32m") != NULL) { // Green for commands
    textColor = (SDL_Color) {
      0,
      255,
      0,
      255
    };
    // Skip color codes in display
    displayText = strstr(text, "\033[32m") + 5;
    char * endCode = strstr(displayText, "\033[0m");
    if (endCode) * endCode = '\0';
  }

  SDL_Surface * surface = TTF_RenderText_Shaded(gFont, displayText, textColor, bg);
  if (!surface)
    return;

  SDL_Texture * texture = SDL_CreateTextureFromSurface(gRenderer, surface);
  if (!texture) {
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect dst = {
    x,
    y,
    surface -> w,
    surface -> h
  };
  SDL_RenderCopy(gRenderer, texture, NULL, & dst);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void render_text_with_command_colors(const char * text, int x, int y, SDL_Color bg) {
  if (!text || strlen(text) == 0 || !gFont || !gRenderer)
    return;

  SDL_Color textColor = {
    NORMAL_COLOR_R,
    NORMAL_COLOR_G,
    NORMAL_COLOR_B,
    255
  };

  // Check if line starts with "> " (user input)
  if (strncmp(text, "> ", 2) == 0) {
    const char * command = text + 2;

    // Check for valid commands
    if (strncmp(command, "help", 4) == 0 || strncmp(command, "clear", 5) == 0 ||
      strncmp(command, "echo ", 5) == 0 || strncmp(command, "wordwrap", 8) == 0 ||
      strncmp(command, "background", 10) == 0 || strncmp(command, "version", 7) == 0 ||
      strncmp(command, "shortcuts", 9) == 0 || strncmp(command, "exit", 4) == 0 ||
      strncmp(command, "quit", 4) == 0) {
      textColor = (SDL_Color) {
        COMMAND_COLOR_R,
        COMMAND_COLOR_G,
        COMMAND_COLOR_B,
        255
      };
    }
    // Check for "Unknown command:" prefix for error color
    else if (strlen(command) > 0) {
      textColor = (SDL_Color) {
        ERROR_COLOR_R,
        ERROR_COLOR_G,
        ERROR_COLOR_B,
        255
      };
    }
  }
  // Check for error messages
  else if (strncmp(text, "Unknown command:", 16) == 0) {
    textColor = (SDL_Color) {
      ERROR_COLOR_R,
      ERROR_COLOR_G,
      ERROR_COLOR_B,
      255
    };
  }

  render_text_colored(text, x, y, textColor, bg);
}

void render_centered_title(const char * title, int y) {
  if (!title || strlen(title) == 0 || !gRenderer)
    return;

  TTF_Font * fontToUse = gTitleFont ? gTitleFont : gFont;
  if (!fontToUse)
    return;

  SDL_Color titleColor = {
    255,
    64,
    64,
    255
  };
  SDL_Color bgColor = {
    0,
    0,
    0,
    255
  };

  SDL_Surface * surface = TTF_RenderText_Shaded(fontToUse, title, titleColor, bgColor);
  if (!surface)
    return;

  SDL_Texture * texture = SDL_CreateTextureFromSurface(gRenderer, surface);
  if (!texture) {
    SDL_FreeSurface(surface);
    return;
  }

  int textWidth = surface -> w;
  int textHeight = surface -> h;

  int windowWidth, windowHeight;
  gui_get_window_size( & windowWidth, & windowHeight);

  int centerX = (windowWidth - textWidth) / 2;

  SDL_Rect dst = {
    centerX,
    y,
    textWidth,
    textHeight
  };
  SDL_RenderCopy(gRenderer, texture, NULL, & dst);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void render_selection_highlight(int x, int y, int width, int height) {
  if (!gRenderer || width <= 0 || height <= 0)
    return;

  SDL_Rect selectionRect = {
    x,
    y,
    width,
    height
  };
  SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(gRenderer, 0, 120, 215, 150);
  SDL_RenderFillRect(gRenderer, & selectionRect);
  SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);
}

void render_cursor(int x, int y) {
  if (!cursorVisible || !gRenderer)
    return;

  int cursorWidth = 2;
  int cursorHeight = FONT_SIZE;

  SDL_Rect cursorRect = {
    x,
    y,
    cursorWidth,
    cursorHeight
  };
  SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
  SDL_RenderFillRect(gRenderer, & cursorRect);
}

int gui_get_char_width() {
  if (!gFont) return 8;
  int w;
  TTF_SizeText(gFont, "W", & w, NULL);
  return w;
}

int get_text_width_in_chars(void) {
  int charWidth = gui_get_char_width();

  int windowWidth, windowHeight;
  gui_get_window_size( & windowWidth, & windowHeight);

  int availableWidth = windowWidth - 20;
  return availableWidth / charWidth;
}

void wrap_text(const char * text, char wrapped[][INPUT_BUFFER_SIZE], int * wrappedLineCount, int maxWidth) {
  if (!text || !wrapped || !wrappedLineCount || maxWidth <= 0)
    return;

  * wrappedLineCount = 0;
  int textLen = strlen(text);

  if (textLen == 0)
    return;

  int currentLine = 0;
  int i = 0;

  while (i < textLen && currentLine < MAX_LINES) {
    int lineStart = i;
    int lastSpace = -1;
    int lineLength = 0;

    // Find the end of the current line
    while (i < textLen && lineLength < maxWidth) {
      if (text[i] == ' ')
        lastSpace = i;

      if (text[i] == '\n') {
        break;
      }

      lineLength++;
      i++;
    }

    // Determine where to break the line
    int lineEnd;
    if (i >= textLen || text[i] == '\n') {
      lineEnd = i;
    } else if (lineLength >= maxWidth) {
      if (lastSpace != -1 && lastSpace > lineStart) {
        lineEnd = lastSpace;
        i = lastSpace + 1;
      } else {
        lineEnd = i;
      }
    } else {
      lineEnd = i;
    }

    // Copy the line
    int copyLength = lineEnd - lineStart;
    if (copyLength >= INPUT_BUFFER_SIZE)
      copyLength = INPUT_BUFFER_SIZE - 1;

    if (copyLength > 0) {
      strncpy(wrapped[currentLine], text + lineStart, copyLength);
      wrapped[currentLine][copyLength] = '\0';
    } else {
      wrapped[currentLine][0] = '\0';
    }

    currentLine++;

    if (i < textLen && text[i] == '\n')
      i++;
  }

  * wrappedLineCount = currentLine;
}

bool gui_is_point_in_text_area(int mouseX, int mouseY, int lineCount) {
  int textAreaLeft = 10;

  int windowWidth, windowHeight;
  gui_get_window_size( & windowWidth, & windowHeight);

  int textAreaRight = windowWidth - 10;
  int textAreaTop = 10;
  int lineHeight = FONT_SIZE + 4;
  int textAreaBottom = 10 + (lineCount + 1) * lineHeight;

  return (mouseX >= textAreaLeft && mouseX <= textAreaRight &&
    mouseY >= textAreaTop && mouseY <= textAreaBottom);
}

void gui_update_cursor(int mouseX, int mouseY, int lineCount) {
  bool shouldShowIbeam = gui_is_point_in_text_area(mouseX, mouseY, lineCount);

  if (shouldShowIbeam && !isIbeamCursorActive && ibeamCursor) {
    SDL_SetCursor(ibeamCursor);
    isIbeamCursorActive = true;
  } else if (!shouldShowIbeam && isIbeamCursorActive && arrowCursor) {
    SDL_SetCursor(arrowCursor);
    isIbeamCursorActive = false;
  }
}

void gui_render(const char * prompt,
  const char * inputBuffer, char output[][INPUT_BUFFER_SIZE], int lineCount, int cursorPos, TextSelection * selection) {
  if (!gRenderer || !gFont) return;

  SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
  SDL_RenderClear(gRenderer);

  // Render background image if available
  if (gBackgroundTexture) {
    int windowWidth, windowHeight;
    gui_get_window_size( & windowWidth, & windowHeight);

    SDL_Rect backgroundRect = {
      0,
      0,
      windowWidth,
      windowHeight
    };

    // Set opacity
    Uint8 alpha = (Uint8)(gBackgroundOpacity * 255);
    SDL_SetTextureAlphaMod(gBackgroundTexture, alpha);

    SDL_RenderCopy(gRenderer, gBackgroundTexture, NULL, & backgroundRect);
  }

  Uint32 now = SDL_GetTicks();
  if (now - lastCursorToggle >= 500) {
    cursorVisible = !cursorVisible;
    lastCursorToggle = now;
  }

  int windowWidth, windowHeight;
  gui_get_window_size( & windowWidth, & windowHeight);

  int y = 10;
  int charWidth = gui_get_char_width();
  int maxWidth = get_text_width_in_chars();

  // Render title
  render_centered_title(TITLE_TEXT, y);

  int titleHeight = TITLE_FONT_SIZE + 10;
  y += titleHeight;

  int lineHeight = FONT_SIZE + 4;
  int availableHeight = windowHeight - y - (lineHeight * 2);
  int maxVisibleLines = availableHeight / lineHeight;

  int startDisplayLine = 0;
  int actualDisplayLineCount = 0;

  // Count total display lines
  int totalDisplayLines = 0;
  for (int i = 0; i < lineCount && i < MAX_LINES; i++) {
    if (wordWrapEnabled) {
      char wrappedLines[MAX_LINES][INPUT_BUFFER_SIZE];
      int wrappedCount = 0;
      wrap_text(output[i], wrappedLines, & wrappedCount, maxWidth);
      totalDisplayLines += wrappedCount;
    } else {
      totalDisplayLines++;
    }
  }

  if (totalDisplayLines > maxVisibleLines) {
    startDisplayLine = totalDisplayLines - maxVisibleLines;
  }

  // Render output lines
  int currentDisplayLine = 0;

  for (int i = 0; i < lineCount && i < MAX_LINES; i++) {
    SDL_Color fg = {
      255,
      255,
      255,
      255
    };
    SDL_Color bg = {
      0,
      0,
      0,
      255
    };

    if (wordWrapEnabled) {
      char wrappedLines[MAX_LINES][INPUT_BUFFER_SIZE];
      int wrappedCount = 0;
      wrap_text(output[i], wrappedLines, & wrappedCount, maxWidth);

      for (int j = 0; j < wrappedCount; j++) {
        if (currentDisplayLine >= startDisplayLine && actualDisplayLineCount < maxVisibleLines) {
          render_text_with_command_colors(wrappedLines[j], 10, y, bg);

          // Handle selection highlighting for wrapped lines
          if (selection && selection -> active) {
            int startLine = selection -> startLine;
            int endLine = selection -> endLine;
            int startChar = selection -> startChar;
            int endChar = selection -> endChar;

            if (startLine > endLine || (startLine == endLine && startChar > endChar)) {
              int tmpLine = startLine, tmpChar = startChar;
              startLine = endLine;
              startChar = endChar;
              endLine = tmpLine;
              endChar = tmpChar;
            }

            if (i >= startLine && i <= endLine) {
              int selStart = (i == startLine) ? startChar : 0;
              int selEnd = (i == endLine) ? endChar : (int) strlen(output[i]);

              int segmentStart = j * maxWidth;
              int segmentEnd = segmentStart + strlen(wrappedLines[j]);

              if (selStart < segmentEnd && selEnd > segmentStart) {
                int highlightStart = (selStart > segmentStart) ? selStart - segmentStart : 0;
                int highlightEnd = (selEnd < segmentEnd) ? selEnd - segmentStart : strlen(wrappedLines[j]);

                if (highlightStart < highlightEnd) {
                  int highlightX = 10 + highlightStart * charWidth;
                  int highlightWidth = (highlightEnd - highlightStart) * charWidth;
                  render_selection_highlight(highlightX, y, highlightWidth, FONT_SIZE);
                }
              }
            }
          }

          y += lineHeight;
          actualDisplayLineCount++;
        }
        currentDisplayLine++;
      }
    } else {
      if (currentDisplayLine >= startDisplayLine && actualDisplayLineCount < maxVisibleLines) {
        render_text_with_command_colors(output[i], 10, y, bg);

        if (selection && selection -> active) {
          int startLine = selection -> startLine;
          int endLine = selection -> endLine;
          int startChar = selection -> startChar;
          int endChar = selection -> endChar;

          if (startLine > endLine || (startLine == endLine && startChar > endChar)) {
            int tmpLine = startLine, tmpChar = startChar;
            startLine = endLine;
            startChar = endChar;
            endLine = tmpLine;
            endChar = tmpChar;
          }

          if (i >= startLine && i <= endLine) {
            int selStart = (i == startLine) ? startChar : 0;
            int selEnd = (i == endLine) ? endChar : (int) strlen(output[i]);

            int lineLen = (int) strlen(output[i]);
            if (selStart > lineLen) selStart = lineLen;
            if (selEnd > lineLen) selEnd = lineLen;

            if (selStart < selEnd) {
              int highlightX = 10 + selStart * charWidth;
              int highlightWidth = (selEnd - selStart) * charWidth;
              render_selection_highlight(highlightX, y, highlightWidth, FONT_SIZE);
            }
          }
        }
        y += lineHeight;
        actualDisplayLineCount++;
      }
      currentDisplayLine++;
    }
  }

  // Render input line
  int inputY = windowHeight - (lineHeight * 2);
  if (inputY < y) inputY = y;

  if (prompt)
    render_text_colored(prompt, 10, inputY, (SDL_Color) {
      255,
      255,
      255,
      255
    }, (SDL_Color) {
      0,
      0,
      0,
      255
    });

  if (inputBuffer) {
    render_text_colored(inputBuffer, 80, inputY, (SDL_Color) {
      255,
      255,
      255,
      255
    }, (SDL_Color) {
      0,
      0,
      0,
      255
    });

    if (selection && selection -> active) {
      int startLine = selection -> startLine;
      int endLine = selection -> endLine;
      int startChar = selection -> startChar;
      int endChar = selection -> endChar;

      if (startLine > endLine || (startLine == endLine && startChar > endChar)) {
        int tmpLine = startLine, tmpChar = startChar;
        startLine = endLine;
        startChar = endChar;
        endLine = tmpLine;
        endChar = tmpChar;
      }

      if (lineCount >= startLine && lineCount <= endLine) {
        int selStart = (lineCount == startLine) ? startChar : 0;
        int selEnd = (lineCount == endLine) ? endChar : (int) strlen(inputBuffer);
        int lineLen = (int) strlen(inputBuffer);
        if (selStart > lineLen) selStart = lineLen;
        if (selEnd > lineLen) selEnd = lineLen;

        if (selStart < selEnd) {
          int highlightX = 80 + selStart * charWidth;
          int highlightWidth = (selEnd - selStart) * charWidth;
          render_selection_highlight(highlightX, inputY, highlightWidth, FONT_SIZE);
        }
      }
    }
  }

  // Render cursor
  if (cursorPos >= 0) {
    int cursorX = 80 + cursorPos * charWidth;
    render_cursor(cursorX, inputY);
  }

  SDL_RenderPresent(gRenderer);
}

void gui_handle_mouse_event(SDL_Event * e, char output[][INPUT_BUFFER_SIZE],
  const char * inputBuffer, int lineCount, TextSelection * selection) {
  if (!selection || !e) return;

  int charWidth = gui_get_char_width();
  int lineHeight = FONT_SIZE + 4;

  int windowWidth, windowHeight;
  gui_get_window_size( & windowWidth, & windowHeight);

  int titleOffset = TITLE_FONT_SIZE + 10;
  int inputLineY = windowHeight - (lineHeight * 2);

  if (e -> type == SDL_MOUSEMOTION) {
    gui_update_cursor(e -> motion.x, e -> motion.y, lineCount);
  }

  int mouseX, mouseY;
  if (e -> type == SDL_MOUSEBUTTONDOWN || e -> type == SDL_MOUSEBUTTONUP) {
    mouseX = e -> button.x;
    mouseY = e -> button.y;
  } else if (e -> type == SDL_MOUSEMOTION) {
    mouseX = e -> motion.x;
    mouseY = e -> motion.y;
  } else {
    return;
  }

  int adjustedMouseY = mouseY - titleOffset;

  int clickedLine, clickedChar;
  if (mouseY >= inputLineY && mouseY < inputLineY + lineHeight) {
    clickedLine = lineCount;
    clickedChar = (mouseX - 80) / charWidth;
  } else {
    clickedLine = (adjustedMouseY - 10) / lineHeight;
    clickedChar = (mouseX - 10) / charWidth;
  }

  int lineLen = 0;
  if (clickedLine == lineCount) {
    lineLen = inputBuffer ? (int) strlen(inputBuffer) : 0;
  } else if (clickedLine >= 0 && clickedLine < lineCount) {
    lineLen = (int) strlen(output[clickedLine]);
  }

  if (clickedChar < 0) clickedChar = 0;
  if (clickedChar > lineLen) clickedChar = lineLen;

  if (e -> type == SDL_MOUSEBUTTONDOWN && e -> button.button == SDL_BUTTON_LEFT) {
    if (clickedLine >= 0 && clickedLine <= lineCount) {
      selection -> active = 1;
      selection -> startLine = clickedLine;
      selection -> startChar = clickedChar;
      selection -> endLine = clickedLine;
      selection -> endChar = clickedChar;
    } else {
      selection -> active = 0;
    }
  } else if (e -> type == SDL_MOUSEMOTION && (e -> motion.state & SDL_BUTTON_LMASK)) {
    if (selection -> active) {
      if (clickedLine >= 0 && clickedLine <= lineCount) {
        selection -> endLine = clickedLine;
        selection -> endChar = clickedChar;
      }
    }
  }
}

void gui_get_selected_text(char output[][INPUT_BUFFER_SIZE],
  const char * inputBuffer, int lineCount, TextSelection * selection, char * buffer, int bufferSize) {
  if (!selection || !selection -> active || !buffer || bufferSize <= 0) {
    if (buffer) buffer[0] = '\0';
    return;
  }

  buffer[0] = '\0';
  int pos = 0;

  int startLine = selection -> startLine;
  int startChar = selection -> startChar;
  int endLine = selection -> endLine;
  int endChar = selection -> endChar;

  if (startLine > endLine || (startLine == endLine && startChar > endChar)) {
    int tempLine = startLine;
    int tempChar = startChar;
    startLine = endLine;
    startChar = endChar;
    endLine = tempLine;
    endChar = tempChar;
  }

  for (int i = startLine; i <= endLine && i <= lineCount; i++) {
    const char * lineText = (i == lineCount) ? (inputBuffer ? inputBuffer : "") : output[i];
    int copyStartChar = (i == startLine) ? startChar : 0;
    int copyEndChar = (i == endLine) ? endChar : (int) strlen(lineText);

    int lineLen = (int) strlen(lineText);
    if (copyStartChar > lineLen) copyStartChar = lineLen;
    if (copyEndChar > lineLen) copyEndChar = lineLen;

    if (copyStartChar < copyEndChar) {
      int copyLen = copyEndChar - copyStartChar;
      if (pos + copyLen < bufferSize - 1) {
        strncpy(buffer + pos, lineText + copyStartChar, copyLen);
        pos += copyLen;
      }
    }

    if (i < endLine && pos < bufferSize - 2) {
      buffer[pos++] = '\n';
    }
  }

  buffer[pos] = '\0';
}

void gui_cleanup() {
  if (arrowCursor) {
    SDL_FreeCursor(arrowCursor);
    arrowCursor = NULL;
  }
  if (ibeamCursor) {
    SDL_FreeCursor(ibeamCursor);
    ibeamCursor = NULL;
  }
  gui_cleanup_background();
}