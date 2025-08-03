#ifndef CONFIG_H
#define CONFIG_H

// Window settings
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Font settings
#define FONT_PATH "assets/FiraCode-Light.ttf"
#define FONT_SIZE 18

// Buffer settings
#define MAX_LINES 100
#define INPUT_BUFFER_SIZE 2048
#define CLIPBOARD_SIZE 4096

// Word wrap settings
#define MAX_LINE_WIDTH 20  // Characters per line for word wrap

// Text selection structure
typedef struct {
    int startLine;
    int startChar;
    int endLine;
    int endChar;
    int active; 
} TextSelection;

// Word wrap state
extern int wordWrapEnabled;

#endif 