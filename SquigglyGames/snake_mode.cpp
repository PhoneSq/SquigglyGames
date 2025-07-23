#include "snake_mode.h"
#include "game.h" // to access global state
#include "renderer.h"
#include "utils.h"

void simulate_snake(Input* input, float dt)
{
	clear_screen(0xFF211E1E);  // dark background
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xFF0BF100);  // green borders


}
