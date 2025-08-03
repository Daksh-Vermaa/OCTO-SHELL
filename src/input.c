#include "input.h"
#include <string.h>
#include <SDL.h>
#include <stdio.h>
#include <windows.h>
#include "shell.h"
#include "config.h"

static char clipboard[CLIPBOARD_SIZE] = "";

void input_handle_event(SDL_Event *e, char *inputBuffer, char output[][INPUT_BUFFER_SIZE], int *lineCount, int *cursorPos, TextSelection *selection)
{
    if (!inputBuffer || !output || !lineCount || !cursorPos)
        return;

    if (e->type == SDL_TEXTINPUT)
    {
        size_t currentLen = strlen(inputBuffer);
        size_t inputLen = strlen(e->text.text);
        
        if (inputLen == 0 || currentLen >= INPUT_BUFFER_SIZE - 1)
            return;
            
        if (currentLen + inputLen < INPUT_BUFFER_SIZE - 1)
        {
            // Insert text at cursor position
            memmove(inputBuffer + *cursorPos + inputLen, inputBuffer + *cursorPos, currentLen - *cursorPos + 1);
            memcpy(inputBuffer + *cursorPos, e->text.text, inputLen);
            *cursorPos += (int)inputLen;
        }
    }
    else if (e->type == SDL_KEYDOWN)
    {
        SDL_Keycode key = e->key.keysym.sym;
        Uint16 mod = e->key.keysym.mod;

        // Handle Ctrl combinations
        if (mod & KMOD_CTRL)
        {
            switch (key)
            {
            case SDLK_c: // Copy
                input_copy_to_clipboard(output, *lineCount, selection);
                break;
            case SDLK_v: // Paste
                input_paste_from_clipboard(inputBuffer, cursorPos);
                break;
            case SDLK_a: // Select All
                input_select_all(output, *lineCount, selection);
                break;
            }
        }
        else
        {
            switch (key)
            {
            case SDLK_BACKSPACE:
                if (*cursorPos > 0)
                {
                    size_t len = strlen(inputBuffer);
                    memmove(inputBuffer + *cursorPos - 1, inputBuffer + *cursorPos, len - *cursorPos + 1);
                    (*cursorPos)--;
                }
                break;
            case SDLK_DELETE:
                if (*cursorPos < (int)strlen(inputBuffer))
                {
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
                    
                    // Execute command
                    shell_execute(inputBuffer, output, lineCount);
                    
                    // Clear input buffer and reset cursor
                    inputBuffer[0] = '\0';
                    *cursorPos = 0;
                }
                break;
            }
        }

        // Clear selection on most key presses (except copy and select all)
        if (selection && key != SDLK_c && !(mod & KMOD_CTRL && key == SDLK_a))
        {
            selection->active = false;
        }
    }
}

void input_copy_to_clipboard(char output[][INPUT_BUFFER_SIZE], int lineCount, TextSelection *selection)
{
    if (!selection || !selection->active)
        return;

    char selectedText[CLIPBOARD_SIZE];
    gui_get_selected_text(output, lineCount, selection, selectedText, CLIPBOARD_SIZE);
    
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
            }
            CloseClipboard();
        }
    }
}

void input_paste_from_clipboard(char *inputBuffer, int *cursorPos)
{
    char pasteText[CLIPBOARD_SIZE] = "";
    
    // Try to get from Windows clipboard first
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
    
    // Fallback to internal clipboard
    if (strlen(pasteText) == 0)
    {
        strcpy(pasteText, clipboard);
    }

    // Insert pasted text at cursor position
    if (strlen(pasteText) > 0)
    {
        size_t currentLen = strlen(inputBuffer);
        size_t pasteLen = strlen(pasteText);
        
        // Validate cursor position
        if (*cursorPos < 0 || *cursorPos > (int)currentLen)
            *cursorPos = (int)currentLen;
            
        // Remove newlines from pasted text for single-line input
        for (size_t i = 0; pasteText[i]; i++)
        {
            if (pasteText[i] == '\n' || pasteText[i] == '\r')
                pasteText[i] = ' ';
        }
        
        // Limit paste length to available space
        if (pasteLen > INPUT_BUFFER_SIZE - currentLen - 1)
            pasteLen = INPUT_BUFFER_SIZE - currentLen - 1;
        
        if (currentLen + pasteLen < INPUT_BUFFER_SIZE - 1)
        {
            // Move existing text to make room
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

    selection->active = true;
    selection->startLine = 0;
    selection->startChar = 0;
    selection->endLine = lineCount - 1;
    selection->endChar = (int)strlen(output[lineCount - 1]);
}