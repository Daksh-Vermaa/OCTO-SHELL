#include <string.h>
#include <SDL.h>

#include <stdio.h>
#include <windows.h>

#include "shell.h"
#include "input.h"
#include "config.h"
#include "gui.h"

static char clipboard[CLIPBOARD_SIZE] = "";

// Undo/Redo
#define MAX_UNDO_STATES 50
typedef struct {
    char buffer[INPUT_BUFFER_SIZE];
    int cursorPos;
} InputState;

static InputState undoStack[MAX_UNDO_STATES];
static InputState redoStack[MAX_UNDO_STATES];
static int undoCount = 0;
static int redoCount = 0;

void save_input_state(const char *inputBuffer, int cursorPos)
{
    if (undoCount < MAX_UNDO_STATES)
    {
        strcpy(undoStack[undoCount].buffer, inputBuffer);
        undoStack[undoCount].cursorPos = cursorPos;
        undoCount++;
    }
    else
    {
        for (int i = 0; i < MAX_UNDO_STATES - 1; i++)
        {
            undoStack[i] = undoStack[i + 1];
        }
        strcpy(undoStack[MAX_UNDO_STATES - 1].buffer, inputBuffer);
        undoStack[MAX_UNDO_STATES - 1].cursorPos = cursorPos;
    }
    redoCount = 0;
}

void input_undo(char *inputBuffer, int *cursorPos)
{
    if (undoCount > 0) 
    {
        if (redoCount < MAX_UNDO_STATES)
        {
            strcpy(redoStack[redoCount].buffer, inputBuffer);
            redoStack[redoCount].cursorPos = *cursorPos;
            redoCount++;
        }
        undoCount--;
        strcpy(inputBuffer, undoStack[undoCount].buffer);
        *cursorPos = undoStack[undoCount].cursorPos;

        int len = (int)strlen(inputBuffer);
        if (*cursorPos > len) *cursorPos = len;
        if (*cursorPos < 0) *cursorPos = 0;
    }
}

void input_redo(char *inputBuffer, int *cursorPos)
{
void save_input_state(const char *inputBuffer, int cursorPos) 
  {
    if (undoCount > 0 && 
        strcmp(undoStack[undoCount-1].buffer, inputBuffer) == 0 &&
        undoStack[undoCount-1].cursorPos == cursorPos) {
        return;
    }

    if (undoCount < MAX_UNDO_STATES) {
        strcpy(undoStack[undoCount].buffer, inputBuffer);
        undoStack[undoCount].cursorPos = cursorPos;
        undoCount++;
    } else {
        for (int i = 0; i < MAX_UNDO_STATES - 1; i++) {
            undoStack[i] = undoStack[i + 1];
        }
        strcpy(undoStack[MAX_UNDO_STATES - 1].buffer, inputBuffer);
        undoStack[MAX_UNDO_STATES - 1].cursorPos = cursorPos;
    }
   redoCount = 0;
  }
}

void input_handle_event(SDL_Event *e, char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int *lineCount, int *cursorPos, TextSelection *selection)
{
    if (!inputBuffer || !output || !lineCount || !cursorPos)
        return;

    if (e->type == SDL_TEXTINPUT)
    {
        save_input_state(inputBuffer, *cursorPos);
        
        size_t currentLen = strlen(inputBuffer);
        size_t inputLen = strlen(e->text.text);
        
        if (inputLen == 0 || currentLen >= INPUT_BUFFER_SIZE - 1)
            return;
            
        if (currentLen + inputLen < INPUT_BUFFER_SIZE - 1)
        {
            memmove(inputBuffer + *cursorPos + inputLen, inputBuffer + *cursorPos, currentLen - *cursorPos + 1);
            memcpy(inputBuffer + *cursorPos, e->text.text, inputLen);
            *cursorPos += (int)inputLen;
        }
    }
    else if (e->type == SDL_KEYDOWN)
    {
        SDL_Keycode key = e->key.keysym.sym;
        Uint16 mod = e->key.keysym.mod;

        if (mod & KMOD_CTRL)
        {
            if (key == SDLK_a)
            {
                // Ctrl+A: Select All
                input_select_all(output, *lineCount, selection);
                return;
            }
            else if (key == SDLK_c)
            {
                // Ctrl+C: Copy selection
                input_copy_to_clipboard(output, inputBuffer, *lineCount, selection);
                return;
            }
            else if (key == SDLK_v)
            {
                save_input_state(inputBuffer, *cursorPos);
                input_paste_from_clipboard(inputBuffer, cursorPos);
                return;
            }
            else if (key == SDLK_z)
            {
                input_undo(inputBuffer, cursorPos);
                return;
            }
            else if (key == SDLK_y)
            {
                input_redo(inputBuffer, cursorPos);
                return;
                    }
        }
        else
        {
            switch (key)
            {
            case SDLK_BACKSPACE:
                if (*cursorPos > 0)
                {
                    save_input_state(inputBuffer, *cursorPos);
                    size_t len = strlen(inputBuffer);
                    memmove(inputBuffer + *cursorPos - 1, inputBuffer + *cursorPos, len - *cursorPos + 1);
                    (*cursorPos)--;
                }
                break;
            case SDLK_DELETE:
                if (*cursorPos < (int)strlen(inputBuffer))
                {
                    save_input_state(inputBuffer, *cursorPos);
                    size_t len = strlen(inputBuffer);
                    memmove(inputBuffer + *cursorPos, inputBuffer + *cursorPos + 1, len - *cursorPos);
                }
                break;
            case SDLK_LEFT:
                if (*cursorPos > 0)
                    (*cursorPos)--;
                break;
            case SDLK_RIGHT:
                if (*cursorPos < (int)strlen(inputBuffer))
                    (*cursorPos)++;
                break;
            case SDLK_HOME:
                *cursorPos = 0;
                break;
            case SDLK_END:
                *cursorPos = (int)strlen(inputBuffer);
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                if (*lineCount < MAX_LINES)
                {
                    snprintf(output[*lineCount], INPUT_BUFFER_SIZE, "> %s", inputBuffer);
                    (*lineCount)++;
                    
                    shell_execute(inputBuffer, output, lineCount);
                    
                    inputBuffer[0] = '\0';
                    *cursorPos = 0;
                    
                    undoCount = 0;
                    redoCount = 0;
                }
                break;
            }
        }

        if (selection && !(mod & KMOD_CTRL))
        {
            selection->active = 0;
        }
    }
}

void input_copy_to_clipboard(char output[][INPUT_BUFFER_SIZE], const char *inputBuffer, int lineCount, TextSelection *selection)

{
    if (!selection || !selection->active)
    {
        printf("Copy failed: No active selection\n");
        return;
    }

    char selectedText[CLIPBOARD_SIZE];
    gui_get_selected_text(output, inputBuffer, lineCount, selection, selectedText, CLIPBOARD_SIZE);
    
    if (strlen(selectedText) > 0)
    {
        // Copy to internal clipboard
        strncpy(clipboard, selectedText, CLIPBOARD_SIZE - 1);
        clipboard[CLIPBOARD_SIZE - 1] = '\0';

        // Copy to Windows clipboard
        if (OpenClipboard(NULL))
        {
            EmptyClipboard();
            size_t textLen = strlen(selectedText);
            HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, textLen + 1);
            if (hClipboardData)
            {
                char *pchData = (char*)GlobalLock(hClipboardData);
                if (pchData)
                {
                    strcpy(pchData, selectedText);
                    GlobalUnlock(hClipboardData);
                    SetClipboardData(CF_TEXT, hClipboardData);
                }
                else
                {
                    GlobalFree(hClipboardData);
                }
            }
            CloseClipboard();
        }
        else
        {
            printf("Failed to open Windows clipboard\n");
        }
    }
    else
    {
        printf("Copy failed: No text selected\n");
    }
}

void input_paste_from_clipboard(char *inputBuffer, int *cursorPos)
{
    char pasteText[CLIPBOARD_SIZE] = "";
    
    if (OpenClipboard(NULL))
    {
        HANDLE hClipboardData = GetClipboardData(CF_TEXT);
        if (hClipboardData)
        {
            char *pchData = (char*)GlobalLock(hClipboardData);
            if (pchData)
            {
                strncpy(pasteText, pchData, CLIPBOARD_SIZE - 1);
                pasteText[CLIPBOARD_SIZE - 1] = '\0';
                GlobalUnlock(hClipboardData);
            }
        }
        CloseClipboard();
    }
    
    if (strlen(pasteText) == 0)
    {
        strcpy(pasteText, clipboard);
    }

    if (strlen(pasteText) > 0)
    {
        size_t currentLen = strlen(inputBuffer);
        size_t pasteLen = strlen(pasteText);
        
        if (*cursorPos < 0 || *cursorPos > (int)currentLen)
            *cursorPos = (int)currentLen;
            
        for (size_t i = 0; pasteText[i]; i++)
        {
            if (pasteText[i] == '\n' || pasteText[i] == '\r')
                pasteText[i] = ' ';
        }
        
        if (pasteLen > INPUT_BUFFER_SIZE - currentLen - 1)
            pasteLen = INPUT_BUFFER_SIZE - currentLen - 1;
        
        if (currentLen + pasteLen < INPUT_BUFFER_SIZE - 1)
        {
            memmove(inputBuffer + *cursorPos + pasteLen, inputBuffer + *cursorPos, currentLen - *cursorPos + 1);
            // Insert pasted text
            memcpy(inputBuffer + *cursorPos, pasteText, pasteLen);
            *cursorPos += (int)pasteLen;
        }
    }
}

void input_select_all(char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection)
{
    if (!selection || lineCount == 0)
        return;

    selection->active = 1;
    selection->startLine = 0;
    selection->startChar = 0;
    selection->endLine = lineCount - 1;
    selection->endChar = (int)strlen(output[lineCount - 1]);
    
}