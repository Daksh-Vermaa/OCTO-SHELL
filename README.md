# OCTO-SHELL ⚡
# A GUI-Based Shell Emulator (SDL2)

It is a **Custom Shell Emulator** built using **C**, **SDL2**, and **SDL_ttf** that simulates a terminal inside a clean, responsive graphical user interface. It combines real shell power with GUI design to deliver a minimalist yet powerful terminal experience on Windows.

---

## 🖼 Features

✅ **Graphical Terminal Interface**  using SDL2  
✅ **Real-time Command Input**  with live rendering    
✅ **Command Output Display**  with scroll-like interface  
✅ **Clean Font Rendering**  using SDL2_ttf  
✅ **History Tracking**  (internally)  
✅ **Works on Windows**  using MinGW-w64 toolchain

---

## 🔧 Tech Stack

- 💻 **C**
- 🧱 **SDL2** for graphics and input
- 🅰️ **SDL2_ttf** for font rendering
- 🛠️ **MinGW-w64** for compilation on Windows
- 🧰 **Makefile** for easy building

---


## 📂 Folder Structure

```
📁 Octo-Shell/
├── 📁 assets/                   # Fonts or other assets
│   └── FiraCode-Light.ttf
│
├── 📁 include/                  # Header files
│   ├── config.h                # Constants 
│   ├── gui.h                   # GUI-related declarations
│   ├── input.h                 # Keyboard input handling
│   └── shell.h                 # Shell logic (command handling)
│
├── 📁 src/                      # Source files
│   ├── gui.c                   # Renders GUI 
│   ├── input.c                 # Handles input 
│   ├── main.c                  # SDL init and main loop
│   └── shell.c                 # Shell logic 
│
├── 🛠️  Makefile                  # Build instructions using make
├── 📄 SDL2_ttf.dll              # SDL2_ttf runtime DLL
├── 📄 SDL2.dll                  # SDL2 runtime DLL


```

### BUILD INSTRUCTION

1. **Clone this repo**
```bash
   git clone https://github.com/Daksh-Vermaa/OCTO-SHELL.git
```
2. **Build using Makefile**
```bash
   mingw32-make
```
3. **Run the shell**
```bash
   mingw32-make runn
```
4. **Delete the shell**
```bash
   mingw32-make clean
```

---

## 🚀 Getting Started

### 📦 Requirements

- **SDL2**  development libraries
- **SDL2_ttf**  development libraries
- **MinGW-w64**  (64-bit)
- A  **Font.ttf**  file  (already included)

---


## ✨ Planned Features
 
 ~ Command history navigation (↑ ↓)

 ~ Scrollback (mouse and keyboard)

 ~ Built-in commands (cd, clear, exit, etc.)

 ~ Syntax highlighting

 ~ Auto-completion (TAB)

 ~ Custom shell scripting support

 ---

## 📜 License
MIT License — feel free to use and modify.

---

## 🤝 Contributing
Pull requests, feedback, and suggestions are welcome! If you find bugs or have cool feature ideas, open an issue or a PR.

---


<h2>📬 Contact</h2>
<p>Made by <a href="https://github.com/Daksh-Vermaa" target="_blank">Daksh Verma </a>⚡</p>
