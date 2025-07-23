# OCTO-Shell Emulator ⚡

A lightweight, cross-platform shell emulator built with C and SDL2, featuring a graphical interface with text rendering, mouse selection, and basic shell command functionality.

![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)
![SDL2](https://img.shields.io/badge/SDL2-2.28.5-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-grey)
![License](https://img.shields.io/badge/License-GPL-green)

## ✨ Features

### 🎯 Core Functionality
- **Interactive Shell Interface**: Command-line interface with prompt and input handling
- **Real-time Text Rendering**: Uses SDL2_ttf for crisp font rendering
- **Command Execution**: Built-in command processor with extensible command system
- **Mouse Selection**: Click and drag to select text output
- **Scrolling Output**: Automatic line management with configurable maximum lines
- **Cursor Animation**: Blinking cursor for better user experience

### 🎨 Visual Features
- **Custom Font Support**: Uses FiraCode-Light for better readability
- **Responsive UI**: 800x600 window with proper text layout
- **Text Selection**: Visual feedback for selected text regions
- **Clean Terminal Design**: Black background with white text for classic terminal feel

### 🔧 Built-in Commands
- `help` - Display available commands and usage
- `echo <text>` - Display the specified text
- `clear` - Clear the terminal output
- **Extensible**: Easy to add new commands in the shell module

## 🚀 Quick Start

### Prerequisites
- **C Compiler**: MinGW-w64 or Visual Studio (Windows)
- **SDL2 Development Libraries** (v2.28.5 or newer)
- **SDL2_ttf Development Libraries** (v2.20.2 or newer)
- **Make**: For building with the provided Makefile


### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/Daksh-Vermaa/OCTO-SHELL.git
   cd octo-shell-emulator
   ```

2. **Install SDL2 Dependencies**
   ```bash
   # Download SDL2 and SDL2_ttf development libraries
   # Extract to C:/Libs/ or modify Makefile paths accordingly
   
   # Directory structure should be:
   # C:/Libs/SDL2-2.28.5/
   # C:/Libs/SDL2_ttf-2.20.2/
   ```

3. **Add Font File**
   ```bash
   # Create assets directory and add FiraCode-Light.ttf
   mkdir assets
   # Copy FiraCode-Light.ttf to assets/ directory
   ```

4. **Build the Project**
   ```bash
   # Using the provided Makefile
   make

   # Or compile manually
   x86_64-w64-mingw32-gcc -Wall -I./include -IC:/Libs/SDL2-2.28.5/x86_64-w64-mingw32/include/SDL2 -IC:/Libs/SDL2_ttf-2.20.2/x86_64-w64-mingw32/include/SDL2 -o shell.exe main.c gui.c input.c shell.c -LC:/Libs/SDL2-2.28.5/x86_64-w64-mingw32/lib -LC:/Libs/SDL2_ttf-2.20.2/x86_64-w64-mingw32/lib -lSDL2 -lSDL2_ttf
   ```

5. **Run the Emulator**
   ```bash
   ./shell.exe
   ```

## 📖 Usage

### Basic Commands
```bash
# Display help information
> help

# Echo text to output
> echo Hello, World!

# Clear the terminal
> clear
```

### Keyboard Shortcuts
- **Enter**: Execute current command
- **Backspace**: Delete previous character
- **Escape**: Exit the application

### Mouse Interaction
- **Left Click + Drag**: Select text in the output area
- **Text Selection**: Visual highlighting of selected regions

## 🏗️ Project Structure

```

📁 Octo-Shell/
├── 📁 assets/                  # Fonts or other assets
│   └── FiraCode-Light.ttf
│
├── 📁 include/                 # Header files
│   ├── config.h                # Constants 
│   ├── gui.h                   # GUI-related declarations
│   ├── input.h                 # Keyboard input handling
│   └── shell.h                 # Shell logic (command handling)
│
├── 📁 src/                     # Source files
│   ├── gui.c                   # Renders GUI 
│   ├── input.c                 # Handles input 
│   ├── main.c                  # SDL init and main loop
│   └── shell.c                 # Shell logic 
│
├── 🛠️  Makefile                # Build instructions using make
├── 📄 SDL2_ttf.dll             # SDL2_ttf runtime DLL
├── 📄 SDL2.dll                 # SDL2 runtime DLL

```

## ⚙️ Configuration

### Window Settings
Modify `config.h` to customize the application:

```c
#define WINDOW_WIDTH 800        // Window width in pixels
#define WINDOW_HEIGHT 600       // Window height in pixels
#define FONT_SIZE 18           // Font size in points
#define MAX_LINES 100          // Maximum output lines
#define INPUT_BUFFER_SIZE 2048 // Input buffer size
```

### Font Configuration
```c
#define FONT_PATH "assets/FiraCode-Light.ttf"  // Font file path
```

## 🔧 Development

### Adding New Commands

1. **Modify `shell.c`** - Add command logic in `shell_execute()`:
   ```c
   else if (strcmp(input, "your_command") == 0)
   {
       snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "Your command output");
       (*lineCount)++;
   }
   ```

2. **Update Help** - Add command description to the help output

### Extending GUI Features

The GUI system is modular and can be extended:
- **Text Rendering**: Modify `render_text_colored()` in `gui.c`
- **Mouse Handling**: Extend `gui_handle_mouse_event()`
- **Visual Effects**: Add new rendering functions

### Custom Input Handling

Input processing can be customized in `input.c`:
- **Key Bindings**: Add new key combinations in `input_handle_event()`
- **Text Processing**: Modify input validation and processing

## 🛠️ Technical Details

### Dependencies
- **SDL2**: Cross-platform multimedia library
- **SDL2_ttf**: TrueType font rendering extension
- **MinGW-w64**: Windows compiler toolchain

### Architecture
- **Modular Design**: Separated concerns (GUI, Input, Shell, Config)
- **Event-Driven**: SDL2 event loop for responsive UI
- **Memory Safe**: Proper buffer management and bounds checking
- **Cross-Platform Ready**: SDL2 base allows easy porting

### Performance
- **60 FPS Rendering**: Smooth visual updates
- **Efficient Text Rendering**: Cached font rendering
- **Low Memory Footprint**: Minimal resource usage

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/amazing-feature
   ```
3. **Make your changes**
   - Follow existing code style
   - Add comments for complex logic
   - Test on Windows platform
4. **Commit your changes**
   ```bash
   git commit -m 'Add some amazing feature'
   ```
5. **Push to the branch**
   ```bash
   git push origin feature/amazing-feature
   ```
6. **Open a Pull Request**

### Development Guidelines
- **Code Style**: Follow existing C conventions
- **Memory Management**: Always check for NULL pointers and buffer overflows
- **Error Handling**: Provide meaningful error messages
- **Documentation**: Comment complex functions and algorithms

## 🐛 Troubleshooting

### Common Issues

1. **Font Loading Failed**
   ```
   Solution: Ensure FiraCode-Light.ttf is in the assets/ directory
   Alternative: The program will try fallback font paths
   ```

2. **SDL2 Library Not Found**
   ```
   Solution: Check library paths in Makefile match your installation
   Verify: SDL2 development libraries are properly installed
   ```

3. **Window Creation Failed**
   ```
   Solution: Ensure graphics drivers support SDL2
   Check: System meets minimum requirements
   ```

### Debug Mode
Compile with debug flags for troubleshooting:
```bash
make CFLAGS="-Wall -g -DDEBUG"
```

## 📝 License

This project is licensed under the GPL-3.0 - see the [LICENSE](LICENSE) file for details.

## 🎯 Roadmap

### Planned Features
- [ ] **Command History**: Up/down arrow navigation
- [ ] **Tab Completion**: Auto-complete commands and paths
- [ ] **Configuration File**: External config file support
- [ ] **Themes**: Multiple color schemes
- [ ] **File Operations**: Basic file system commands
- [ ] **Scripting**: Simple script execution support

### Long-term Goals
- [ ] **Linux/macOS Support**: Cross-platform compilation
- [ ] **Plugin System**: External command modules
- [ ] **Network Commands**: Basic networking functionality
- [ ] **Advanced Text Selection**: Copy/paste support

## 🙏 Acknowledgments

- **SDL2 Team**: For the excellent multimedia library
- **FiraCode**: For the beautiful monospace font
- **MinGW-w64**: For the Windows compilation toolchain

## 📞 Support

If you encounter issues or have questions:

- **GitHub Issues**: [Create an Issue](https://github.com//Daksh-Vermaa/OCTO-SHELL/issues)
- **Feature Requests**: Use the issue tracker with "enhancement" label
- **Bug Reports**: Include system info and steps to reproduce

---

**Built by <a href="https://github.com/Daksh-Vermaa" target="_blank">Daksh Verma </a>⚡ and C**

*A minimalist approach to shell emulation with modern graphics*
