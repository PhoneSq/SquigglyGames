#ifndef RENDERER_H
#define RENDERER_H

#include "utils.h"
#include <windows.h>

// Render state structure
struct Render_State {
    int height, width;
    void* memory;
    BITMAPINFO bitmap_info;
};

// Global render state
extern Render_State render_state;

// Rendering functions
 void clear_screen(u32 color);
 void draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color);
 void draw_arena_borders(float arena_x, float arena_y, u32 color);
 void draw_rect(float x, float y, float half_size_x, float half_size_y, u32 color);
 void draw_text(const char* text, float x, float y, float size, u32 color);
 void draw_number(int number, float x, float y, float size, u32 color);

#endif