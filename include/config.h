#ifndef CONFIG_H
#define CONFIG_H

// Window settings
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Font settings
#define FONT_PATH "assets/Typewriter.ttf"
#define FONT_SIZE 18

// Title font settings
#define TITLE_FONT_PATH "assets/FiraCode-Bold.ttf"
#define TITLE_FONT_SIZE 42
#define TITLE_TEXT "OCTO-SHELL Emulator "

// Background image settings
#define BACKGROUND_IMAGE_PATH "assets/background.png"
#define BACKGROUND_OPACITY 20          // 0-255, where 0 is transparent and 255 is opaque
#define BACKGROUND_SCALE_MODE 1        // 0 = stretch, 1 = fit (maintain aspect ratio), 2 = fill, 3 = tile
#define BACKGROUND_ENABLED 1           // 0 = disabled, 1 = enabled
// #define BACKGROUND_BLUR_ENABLED 1      // 0 = no blur, 1 = apply blur effect
// #define BACKGROUND_TINT_R 255          // Red component of background tint (0-255)
// #define BACKGROUND_TINT_G 255          // Green component of background tint (0-255)
// #define BACKGROUND_TINT_B 255          // Blue component of background tint (0-255)
// #define BACKGROUND_OVERLAY_COLOR_R 0   // Red component of overlay color (0-255)
// #define BACKGROUND_OVERLAY_COLOR_G 0   // Green component of overlay color (0-255)
// #define BACKGROUND_OVERLAY_COLOR_B 0   // Blue component of overlay color (0-255)
// #define BACKGROUND_OVERLAY_OPACITY 100  // Overlay opacity (0-255)

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
    int active; // Changed from bool to int for better compatibility
} TextSelection;

// Background configuration structure
typedef struct {
    char imagePath[512];
    int opacity;           // 0-255
    int scaleMode;        // 0=stretch, 1=fit, 2=fill, 3=tile
    int enabled;          // 0=disabled, 1=enabled
    int blurEnabled;      // 0=no blur, 1=blur
    int tintR, tintG, tintB;           // RGB tint values
    int overlayR, overlayG, overlayB;  // RGB overlay color
    int overlayOpacity;   // Overlay opacity
} BackgroundConfig;

// Word wrap state
extern int wordWrapEnabled;

// Background configuration global
extern BackgroundConfig backgroundConfig;

#endif