CC = x86_64-w64-mingw32-gcc

CFLAGS = -Wall \
 -I./include \
 -IC:/Libs/SDL2-2.28.5/x86_64-w64-mingw32/include/SDL2 \
 -IC:/Libs/SDL2_ttf-2.20.2/x86_64-w64-mingw32/include/SDL2

LDFLAGS = -mconsole \
  -LC:/Libs/SDL2-2.28.5/x86_64-w64-mingw32/lib \
  -LC:/Libs/SDL2_ttf-2.20.2/x86_64-w64-mingw32/lib \
  -LC:/Libs/SDL2_image-2.8.2/x86_64-w64-mingw32/lib \
  -lSDL2_image -lSDL2_ttf -lSDL2

SRC = src/main.c src/gui.c src/input.c src/shell.c
TARGET = shell.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f shell.exe
