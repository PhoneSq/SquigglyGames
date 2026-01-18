#include "renderer.h"

// Color constants
// green 0xFF0BF100
// black 0xFF211E1E
// lighter_green 0xFF59FF00
// bluish_green 0xFF1BD86E

global_variable float render_scale = 0.01f;

// Letter definitions for text rendering
const char* letters[][7] = {
	" 00",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"000",
	"0  0",
	"0  0",
	"000",
	"0  0",
	"0  0",
	"000",

	" 000",
	"0",
	"0",
	"0",
	"0",
	"0",
	" 000",

	"000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"000",

	"0000",
	"0",
	"0",
	"000",
	"0",
	"0",
	"0000",

	"0000",
	"0",
	"0",
	"000",
	"0",
	"0",
	"0",

	" 000",
	"0",
	"0",
	"0 00",
	"0  0",
	"0  0",
	" 000",

	"0  0",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"000",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",
	"000",

	" 000",
	"   0",
	"   0",
	"   0",
	"0  0",
	"0  0",
	" 000",

	"0  0",
	"0  0",
	"0 0",
	"00",
	"0 0",
	"0  0",
	"0  0",

	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0000",

	"00 00",
	"0 0 0",
	"0 0 0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",

	"00  0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0  00",

	"0000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0000",

	" 000",
	"0  0",
	"0  0",
	"000",
	"0",
	"0",
	"0",

	" 000 ",
	"0   0",
	"0   0",
	"0   0",
	"0 0 0",
	"0  0 ",
	" 00 0",

	"000",
	"0  0",
	"0  0",
	"000",
	"0  0",
	"0  0",
	"0  0",

	" 000",
	"0",
	"0 ",
	" 00",
	"   0",
	"   0",
	"000 ",

	"000",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",

	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	" 00",

	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	" 0 0",
	"  0",

	"0   0 ",
	"0   0",
	"0   0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	" 0 0 ",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	" 0 0",
	"0   0",
	"0   0",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	"  0",
	"  0",
	"  0",

	"0000",
	"   0",
	"  0",
	" 0",
	"0",
	"0",
	"0000",

	"",
	"",
	"",
	"",
	"",
	"",
	"0",

	"   0",
	"  0",
	"  0",
	" 0",
	" 0",
	"0",
	"0",
};

 void
clear_screen(u32 color)
{
    u32* pixel = (u32*)render_state.memory;
    for (int y = 0; y < render_state.height; y++)
    {
        for (int x = 0; x < render_state.width; x++)
        {
            *pixel++ = color;
        }
    }
}

 void
draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color)
{
    x0 = clamp(0, x0, render_state.width);
    x1 = clamp(0, x1, render_state.width);
    y0 = clamp(0, y0, render_state.height);
    y1 = clamp(0, y1, render_state.height);

    for (int y = y0; y < y1; y++)
    {
        u32* pixel = (u32*)render_state.memory + x0 + (y * render_state.width);
        for (int x = x0; x < x1; x++)
        {
            *pixel++ = color;
        }
    }
}

 void
draw_arena_borders(float arena_x, float arena_y, u32 color) 
{
    arena_x *= render_state.height * render_scale;
    arena_y *= render_state.height * render_scale;

    int x0 = (int)((float)render_state.width * .5f - arena_x);
    int x1 = (int)((float)render_state.width * .5f + arena_x);
    int y0 = (int)((float)render_state.height * .5f - arena_y);
    int y1 = (int)((float)render_state.height * .5f + arena_y);

    draw_rect_in_pixels(0, 0, render_state.width, y0, color);
    draw_rect_in_pixels(0, y1, x1, render_state.height, color);
    draw_rect_in_pixels(0, y0, x0, y1, color);
    draw_rect_in_pixels(x1, y0, render_state.width, render_state.height, color);
}

 void
draw_rect(float x, float y, float half_size_x, float half_size_y, u32 color)
{
    x *= render_state.height * render_scale;
    y *= render_state.height * render_scale;
    half_size_x *= render_state.height * render_scale;
    half_size_y *= render_state.height * render_scale;

    x += render_state.width / 2.f;
    y += render_state.height / 2.f;

    // Change to pixels
    int x0 = x - half_size_x;
    int x1 = x + half_size_x;
    int y0 = y - half_size_y;
    int y1 = y + half_size_y;

    draw_rect_in_pixels(x0, y0, x1, y1, color);
}

 void
draw_text(const char *text, float x, float y, float size, u32 color)
{
    float half_size = size * .5f;
    float original_y = y;

    while (*text) 
    {
        if (*text != 32)
        {
            const char** a_letter = letters[*text - 'A'];
            float original_x = x;

            for (int i = 0; i < 7; i++)
            {
                const char* row = a_letter[i];
                while (*row)
                {
                    if (*row == '0')
                    {
                        draw_rect(x, y, half_size, half_size, color);
                    }
                    x += size;
                    row++;
                }
                y -= size;
                x = original_x;
            }
        }
        text++;
        x += size * 6.f;
        y = original_y;
    }
}

 void
draw_number(int number, float x, float y, float size, u32 color)
{
	float half_size = size * .5f;

	bool drew_number = false;
	while (number || !drew_number)
	{
		drew_number = true;
		int digit = number % 10;
		number = number / 10;
		switch (digit)
		{
		case 0: {
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x, y + size * 2.f, half_size, half_size, color);
			draw_rect(x, y - size * 2.f, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 1: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			x -= size * 2.f;
		} break;

		case 2: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x, y, 1.5f * size, half_size, color);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x + size, y + size, half_size, half_size, color);
			draw_rect(x - size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 3: {
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x - half_size, y, size, half_size, color);
			draw_rect(x - half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			x -= size * 4.f;
		} break;

		case 4: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - size, y + size, half_size, 1.5f * size, color);
			draw_rect(x, y, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 5: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x, y, 1.5f * size, half_size, color);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x - size, y + size, half_size, half_size, color);
			draw_rect(x + size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 6: {
			draw_rect(x + half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x + half_size, y, size, half_size, color);
			draw_rect(x + half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y - size, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 7: {
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			x -= size * 4.f;
		} break;

		case 8: {
			draw_rect(x - size, y, half_size, 2.5f * size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x, y + size * 2.f, half_size, half_size, color);
			draw_rect(x, y - size * 2.f, half_size, half_size, color);
			draw_rect(x, y, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		case 9: {
			draw_rect(x - half_size, y + size * 2.f, size, half_size, color);
			draw_rect(x - half_size, y, size, half_size, color);
			draw_rect(x - half_size, y - size * 2.f, size, half_size, color);
			draw_rect(x + size, y, half_size, 2.5f * size, color);
			draw_rect(x - size, y + size, half_size, half_size, color);
			x -= size * 4.f;
		} break;

		}
	}
}

 void draw_circle(float x, float y, float radius, u32 color)
 {
	 // Convert center and radius from scaled float coords to pixel coords
	 int cx = (int)(x * render_state.height * render_scale + render_state.width / 2.f);
	 int cy = (int)(y * render_state.height * render_scale + render_state.height / 2.f);
	 int r = (int)(radius * render_state.height * render_scale);

	 int x_offset = 0;
	 int y_offset = r;
	 int d = 1 - r;

	 auto plot_circle_points = [&](int xc, int yc, int x, int y) {
		 draw_pixel(xc + x, yc + y, color);
		 draw_pixel(xc - x, yc + y, color);
		 draw_pixel(xc + x, yc - y, color);
		 draw_pixel(xc - x, yc - y, color);
		 draw_pixel(xc + y, yc + x, color);
		 draw_pixel(xc - y, yc + x, color);
		 draw_pixel(xc + y, yc - x, color);
		 draw_pixel(xc - y, yc - x, color);
		 };

	 while (x_offset <= y_offset)
	 {
		 plot_circle_points(cx, cy, x_offset, y_offset);

		 if (d < 0)
		 {
			 d += 2 * x_offset + 3;
		 }
		 else
		 {
			 d += 2 * (x_offset - y_offset) + 5;
			 y_offset--;
		 }
		 x_offset++;
	 }
 }

 void draw_filled_circle(float x, float y, float radius, u32 color)
 {
	 int cx = (int)(x * render_state.height * render_scale + render_state.width / 2.f);
	 int cy = (int)(y * render_state.height * render_scale + render_state.height / 2.f);
	 int r = (int)(radius * render_state.height * render_scale);

	 int x_offset = 0;
	 int y_offset = r;
	 int d = 1 - r;

	 auto draw_horizontal_line = [&](int y_line, int x_start, int x_end) {
		 if (y_line < 0 || y_line >= render_state.height) return;

		 if (x_start < 0) x_start = 0;
		 if (x_end >= render_state.width) x_end = render_state.width - 1;

		 u32* pixel = (u32*)render_state.memory + y_line * render_state.width + x_start;
		 for (int x = x_start; x <= x_end; ++x)
		 {
			 *pixel++ = color;
		 }
		 };

	 while (x_offset <= y_offset)
	 {
		 draw_horizontal_line(cy + y_offset, cx - x_offset, cx + x_offset);
		 draw_horizontal_line(cy - y_offset, cx - x_offset, cx + x_offset);
		 draw_horizontal_line(cy + x_offset, cx - y_offset, cx + y_offset);
		 draw_horizontal_line(cy - x_offset, cx - y_offset, cx + y_offset);

		 if (d < 0)
		 {
			 d += 2 * x_offset + 3;
		 }
		 else
		 {
			 d += 2 * (x_offset - y_offset) + 5;
			 y_offset--;
		 }
		 x_offset++;
	 }
 }

 inline void draw_pixel(int px, int py, u32 color)
 {
	 if (px >= 0 && px < render_state.width &&
		 py >= 0 && py < render_state.height)
	 {
		 u32* pixel = (u32*)render_state.memory;
		 pixel[py * render_state.width + px] = color;
	 }
 }

 void draw_line(float x0, float y0, float x1, float y1, float half_size, u32 color)
 {
	 // Convert logical coordinates to pixel space
	 float scale = render_state.height * render_scale;

	 x0 = x0 * scale + render_state.width / 2.0f;
	 y0 = y0 * scale + render_state.height / 2.0f;
	 x1 = x1 * scale + render_state.width / 2.0f;
	 y1 = y1 * scale + render_state.height / 2.0f;

	 float dx = x1 - x0;
	 float dy = y1 - y0;
	 float length = sqrtf(dx * dx + dy * dy);
	 if (length == 0) return;

	 // Normalize direction vector
	 float nx = dx / length;
	 float ny = dy / length;

	 // Perpendicular vector
	 float px = -ny * half_size * scale;
	 float py = nx * half_size * scale;

	 // 4 corners of the rectangle (line with thickness)
	 int x_points[4] = {
		 (int)(x0 + px), (int)(x1 + px),
		 (int)(x1 - px), (int)(x0 - px)
	 };
	 int y_points[4] = {
		 (int)(y0 + py), (int)(y1 + py),
		 (int)(y1 - py), (int)(y0 - py)
	 };

	 // Bounding box and draw it using 2 triangles or simple scanline fill
	 // For now, a hacky way using min/max box
	 int min_x = *std::min_element(x_points, x_points + 4);
	 int max_x = *std::max_element(x_points, x_points + 4);
	 int min_y = *std::min_element(y_points, y_points + 4);
	 int max_y = *std::max_element(y_points, y_points + 4);

	 for (int y = min_y; y <= max_y; y++)
	 {
		 for (int x = min_x; x <= max_x; x++)
		 {
			 // Dot test to see if pixel is within the line shape
			 float px = x - x0;
			 float py = y - y0;
			 float proj = (px * dx + py * dy) / (length * length);
			 if (proj < 0 || proj > 1) continue;

			 float closest_x = x0 + proj * dx;
			 float closest_y = y0 + proj * dy;
			 float dist_x = x - closest_x;
			 float dist_y = y - closest_y;
			 float dist2 = dist_x * dist_x + dist_y * dist_y;

			 if (dist2 <= (half_size * scale) * (half_size * scale))
				 draw_pixel(x, y, color);
		 }
	 }
 }