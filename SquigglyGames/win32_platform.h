#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H


#include "utils.h"
#include "platform_common.h"
#include "renderer.h"
#include "game.h"
#include <windows.h>

// Global variables
extern bool running;

// Function declarations
LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);

#endif