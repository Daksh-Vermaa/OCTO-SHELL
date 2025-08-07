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
    if (!inputBuffer) return;
    
    // Validate cursor position
    int len = (int)strlen(inputBuffer);
    if (cursorPos < 0) cursorPos = 0;
    if (cursorPos > len) cursorPos = len;
    
    // Avoid duplicate states
    if (undoCount > 0 && 
        strcmp(undoStack[undoCount-1].buffer, inputBuffer) == 0 &&
        undoStack[undoCount-1].cursorPos == cursorPos) {
        return;
    }

    if (undoCount < MAX_UNDO_STATES)
    {
        strncpy(undoStack[undoCount].buffer, inputBuffer, INPUT_BUFFER_SIZE - 1);
        undoStack[undoCount].buffer[INPUT_BUFFER_SIZE - 1] = '\0';
        undoStack[undoCount].cursorPos = cursorPos;
        undoCount++;
    }
    else
    {
        // Shift array left to make room for new state
        for (int i = 0; i < MAX_UNDO_STATES - 1; i++)
        {
            undoStack[i] = undoStack[i + 1];
        }
        strncpy(undoStack[MAX_UNDO_STATES - 1].buffer, inputBuffer, INPUT_BUFFER_SIZE - 1);
        undoStack[MAX_UNDO_STATES - 1].buffer[INPUT_BUFFER_SIZE - 1] = '\0';
        undoStack[MAX_UNDO_STATES - 1].cursorPos = cursorPos;
    }
    redoCount = 0; // Clear redo stack when new action is performed
}

void input_undo(char *inputBuffer, int *cursorPos)
{
    if (!inputBuffer || !cursorPos || undoCount <= 0) return;
    
    // Save current state to redo stack
    if (redoCount < MAX_UNDO_STATES)
    {
        strncpy(redoStack[redoCount].buffer, inputBuffer, INPUT_BUFFER_SIZE - 1);
        redoStack[redoCount].buffer[INPUT_BUFFER_SIZE - 1] = '\0';
        redoStack[redoCount].cursorPos = *cursorPos;
        redoCount++;
    }
    
    // Restore previous state
    undoCount--;
    strncpy(inputBuffer, undoStack[undoCount].buffer, INPUT_BUFFER_SIZE - 1);
    inputBuffer[INPUT_BUFFER_SIZE - 1] = '\0';
    *cursorPos = undoStack[undoCount].cursorPos;

    // Validate cursor position
    int len = (int)strlen(inputBuffer);
    if (*cursorPos > len) *cursorPos = len;
    if (*cursorPos < 0) *cursorPos = 0;
}

void input_redo(char *inputBuffer, int *cursorPos)
{
    if (!inputBuffer || !cursorPos || redoCount <= 0) return;
    
    // Save current state to undo stack
    if (undoCount < MAX_UNDO_STATES)
    {
        strncpy(undoStack[undoCount].buffer, inputBuffer, INPUT_BUFFER_SIZE - 1);
        undoStack[undoCount].buffer[INPUT_BUFFER_SIZE - 1] = '\0';
        undoStack[undoCount].cursorPos = *cursorPos;
        undoCount++;
    }
    
    // Restore redo state
    redoCount--;
    strncpy(inputBuffer, redoStack[redoCount].buffer, INPUT_BUFFER_SIZE - 1);
    inputBuffer[INPUT_BUFFER_SIZE - 1] = '\0';
    *cursorPos = redoStack[redoCount].cursorPos;

    // Validate cursor position
    int len = (int)strlen(inputBuffer);
    if (*cursorPos > len) *cursorPos = len;
    if (*cursorPos < 0) *cursorPos = 0;
}

void input_handle_event(SDL_Event *e, char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int *lineCount, int *cursorPos, TextSelection *selection)
{
    if (!inputBuffer || !output || !lineCount || !cursorPos || !e)
        return;

    if (e->type == SDL_TEXTINPUT)
    {
        // Validate input text
        if (!e->text.text || strlen(e->text.text) == 0)
            return;
            
        // Skip control characters and non-printable characters
        bool hasValidChars = false;
        for (int i = 0; e->text.text[i]; i++) {
            if ((unsigned char)e->text.text[i] >= 32 && (unsigned char)e->text.text[i] < 127) {
                hasValidChars = true;
                break;
            }
        }
        
        if (!hasValidChars) return;
        
        save_input_state(inputBuffer, *cursorPos);
        
        size_t currentLen = strlen(inputBuffer);
        size_t inputLen = strlen(e->text.text);
        
        // Validate cursor position
        if (*cursorPos < 0) *cursorPos = 0;
        if (*cursorPos > (int)currentLen) *cursorPos = (int)currentLen;
        
        // Check if there's enough space
        if (currentLen + inputLen >= INPUT_BUFFER_SIZE - 1)
            return;
            
        // Insert text at cursor position
        memmove(inputBuffer + *cursorPos + inputLen, 
                inputBuffer + *cursorPos, 
                currentLen - *cursorPos + 1);
        memcpy(inputBuffer + *cursorPos, e->text.text, inputLen);
        *cursorPos += (int)inputLen;
    }
    else if (e->type == SDL_KEYDOWN)
    {
        SDL_Keycode key = e->key.keysym.sym;
        Uint16 mod = e->key.keysym.mod;

        if (mod & KMOD_CTRL)
        {
            switch (key)
            {
                case SDLK_a:
                    // Ctrl+A: Select All
                    input_select_all(output, *lineCount, selection);
                    return;
                    
                case SDLK_c:
                    // Ctrl+C: Copy selection
                    input_copy_to_clipboard(output, inputBuffer, *lineCount, selection);
                    return;
                    
                case SDLK_v:
                    // Ctrl+V: Paste
                    save_input_state(inputBuffer, *cursorPos);
                    input_paste_from_clipboard(inputBuffer, cursorPos);
                    return;
                    
                case SDLK_z:
                    // Ctrl+Z: Undo
                    input_undo(inputBuffer, cursorPos);
                    return;
                    
                case SDLK_y:
                    // Ctrl+Y: Redo
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
                    
                    // Validate cursor position
                    if (*cursorPos > (int)len) *cursorPos = (int)len;
                    
                    memmove(inputBuffer + *cursorPos - 1, 
                            inputBuffer + *cursorPos, 
                            len - *cursorPos + 1);
                    (*cursorPos)--;
                }
                break;
                
            case SDLK_DELETE:
                {
                    size_t len = strlen(inputBuffer);
                    if (*cursorPos < (int)len)
                    {
                        save_input_state(inputBuffer, *cursorPos);
                        memmove(inputBuffer + *cursorPos, 
                                inputBuffer + *cursorPos + 1, 
                                len - *cursorPos);
                    }
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
                    
                    // Clear undo/redo history after command execution
                    undoCount = 0;
                    redoCount = 0;
                }
                break;
            }
        }

        // Clear selection on most key presses (except Ctrl combinations)
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
            HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, textLen + 1);
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
    }
}

void input_paste_from_clipboard(char *inputBuffer, int *cursorPos)
{
    if (!inputBuffer || !cursorPos) return;
    
    char pasteText[CLIPBOARD_SIZE] = "";
    
    // Try to get text from Windows clipboard first
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
    
    // Fallback to internal clipboard if Windows clipboard is empty
    if (strlen(pasteText) == 0)
    {
        strcpy(pasteText, clipboard);
    }

    if (strlen(pasteText) > 0)
    {
        size_t currentLen = strlen(inputBuffer);
        size_t pasteLen = strlen(pasteText);
        
        // Validate cursor position
        if (*cursorPos < 0) *cursorPos = 0;
        if (*cursorPos > (int)currentLen) *cursorPos = (int)currentLen;
        
        // Replace newlines and carriage returns with spaces
        for (size_t i = 0; i < pasteLen; i++)
        {
            if (pasteText[i] == '\n' || pasteText[i] == '\r' || pasteText[i] == '\t')
                pasteText[i] = ' ';
        }
        
        // Limit paste length to available space
        if (pasteLen > INPUT_BUFFER_SIZE - currentLen - 1)
            pasteLen = INPUT_BUFFER_SIZE - currentLen - 1;
        
        if (pasteLen > 0 && currentLen + pasteLen < INPUT_BUFFER_SIZE - 1)
        {
            // Make room for pasted text
            memmove(inputBuffer + *cursorPos + pasteLen, 
                    inputBuffer + *cursorPos, 
                    currentLen - *cursorPos + 1);
            // Insert pasted text
            memcpy(inputBuffer + *cursorPos, pasteText, pasteLen);
            *cursorPos += (int)pasteLen;
        }
    }
}

void input_select_all(char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection)
{
    if (!selection || lineCount <= 0)
        return;

    selection->active = 1;
    selection->startLine = 0;
    selection->startChar = 0;
    selection->endLine = lineCount - 1;
    selection->endChar = (int)strlen(output[lineCount - 1]);
}