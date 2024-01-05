#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

void createMainWindow(HINSTANCE hInstance);
void createControls(HWND hwnd);
LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
