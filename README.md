# 🐙 OCTO-Shell Emulator v2.0

A modern, lightweight terminal emulator built with SDL2 that brings retro computing vibes to your desktop! Features a sleek interface with customizable fonts, text selection, clipboard integration, and dynamic word wrapping.

[C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)
![SDL2](https://img.shields.io/badge/SDL2-2.28.5-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-GPL-green)

## ✨ Features

### 🖥️ **Modern Terminal Experience**
- **Retro Aesthetic**: Classic terminal look with modern functionality
- **Smooth Rendering**: Hardware-accelerated graphics with 60 FPS
- **Resizable Window**: Dynamically adjust to your preferred size
- **Custom Fonts**: Support for TTF fonts with fallback options

### 📝 **Advanced Text Handling**
- **Word Wrapping**: Toggle dynamic text wrapping on/off
- **Text Selection**: Click and drag to select text like a pro
- **Copy & Paste**: Full clipboard integration with system clipboard
- **Undo/Redo**: Never lose your work with 50-level undo history

### ⌨️ **Rich Input Support**
- **Cursor Navigation**: Arrow keys, Home, End for precise editing
- **Keyboard Shortcuts**: Familiar Ctrl+C, Ctrl+V, Ctrl+A, Ctrl+Z shortcuts
- **Smart Input**: Intelligent text insertion and deletion

### 🔧 **Built-in Commands**
- `help` - Display all available commands
- `echo <text>` - Print text to output
- `clear` - Clear the terminal screen
- `wordwrap <on/off>` - Toggle word wrapping
- `shortcuts` - Show all keyboard shortcuts
- `exit/quit` - Gracefully close the application

## 🚀 Getting Started

### Prerequisites
- Windows Operating System
- SDL2 Development Libraries
- SDL2_ttf Library
- C Compiler (GCC, MSVC, or MinGW)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/Daksh-Vermaa/OCTO-SHELL.git
   cd Octo-Shell
   ```

2. **Install SDL2 Dependencies**
   - Download SDL2 and SDL2_ttf from [libsdl.org](https://www.libsdl.org/)
   - Extract to your development environment
   - Set up include and library paths

3. **Add Font Files**
   Create an `assets` folder and add your preferred TTF fonts:
   ```
   assets/
   ├── Typewriter.ttf      (Main font)
   └── FiraCode-Bold.ttf   (Title font)
   ```

4. **Compile the Project**
   ```bash
   gcc -o octo-shell main.c gui.c input.c shell.c -lSDL2 -lSDL2_ttf
   ```

5. **Run the Emulator**
   ```bash
   ./shell.exe
   ```

## 🎮 Usage

### Basic Commands
```bash
>> help                    # Show all commands
>> echo Hello World!       # Display text
>> wordwrap on             # Enable word wrapping
>> clear                   # Clear screen
>> exit                    # Close application
```

### Keyboard Shortcuts
| Shortcut | Action |
|----------|--------|
| `Ctrl + C` | Copy selected text |
| `Ctrl + V` | Paste from clipboard |
| `Ctrl + A` | Select all text |
| `Ctrl + Z` | Undo last action |
| `Ctrl + Y` | Redo last action |
| `Arrow Keys` | Navigate cursor |
| `Home/End` | Jump to line start/end |
| `Escape` | Exit application |

### Mouse Controls
- **Click and Drag**: Select text
- **Double Click**: Select word (future feature)
- **Right Click**: Context menu (future feature)

## 🔧 Configuration

Edit `config.h` to customize your experience:

```c
#define WINDOW_WIDTH 800        // Initial window width
#define WINDOW_HEIGHT 600       // Initial window height
#define FONT_SIZE 18           // Main font size
#define TITLE_FONT_SIZE 24     // Title font size
#define MAX_LINE_WIDTH 20      // Characters per line for word wrap
```

## 📁 Project Structure

```

📁 Octo-Shell/
├── 📁 assets/                  # Fonts or other assets
│   ├── FiraCode-Light.ttf
│   ├── FiraCdoe-Bold.ttf
|   └── Typewriter.ttf
|
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

## 🛠️ Technical Details

### Architecture
- **Modular Design**: Separated concerns for easy maintenance
- **Event-Driven**: Responsive to user input and system events
- **Memory Safe**: Proper buffer management and bounds checking
- **Cross-Platform Ready**: Built with portability in mind

### Dependencies
- **SDL2**: Core graphics and window management
- **SDL2_ttf**: TrueType font rendering
- **Windows API**: Clipboard integration (Windows-specific)

## 🚧 Roadmap

### Upcoming Features
- [ ] **Command History**: Navigate through previous commands
- [ ] **Tab Completion**: Auto-complete commands and file names
- [ ] **Themes**: Multiple color schemes and visual themes
- [ ] **File Operations**: Basic file system navigation
- [ ] **Scripting**: Simple script execution capabilities
- [ ] **Plugin System**: Extensible command architecture

### Known Issues
- Word wrap calculation needs refinement for edge cases
- Font fallback chain could be more robust
- Mouse selection sometimes glitchy on rapid movements

## 🤝 Contributing

We welcome contributions! Here's how you can help:

1. **Fork the repository**
2. **Create a feature branch** (`git checkout -b feature/amazing-feature`)
3. **Commit your changes** (`git commit -m 'Add amazing feature'`)
4. **Push to the branch** (`git push origin feature/amazing-feature`)
5. **Open a Pull Request**

### Development Guidelines
- Follow existing code style and conventions
- Add comments for complex logic
- Test on different screen resolutions
- Ensure no memory leaks

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **SDL2 Team** - For the amazing graphics library
- **Font Authors** - For beautiful typefaces
- **Community Contributors** - For bug reports and suggestions
- **Retro Computing Enthusiasts** - For inspiration

## 📞 Support

Having issues? 

- 📧 **Direct Contact**: dakshr117@gmail.com

---

<div align="center">

**Made with ❤️ by [<a href="https://github.com/Daksh-Vermaa" target="_blank">Daksh Verma </a>⚡]**

*If you found this project helpful, please consider giving it a ⭐!*

</div>
