#include "breakout_mode.h"
#include "game.h" // to access global state
#include "renderer.h"
#include "utils.h"

void simulate_breakout(Input* input, float dt) 
{
	clear_screen(0xFF211E1E);  // dark background
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xFF0BF100);  // green borders

	// ?? Initialize bricks ONCE
	/*static bool bricks_initialized = false;
	const int BRICK_ROWS = 5;
	const int BRICK_COLUMNS = 13;
	static Brick bricks[BRICK_ROWS * BRICK_COLUMNS];*/

	// initialize bricks 
	if (!bricks_initialized)
	{
		float start_x = -70.f;
		float start_y = 20.f;
		float gap = 2.f;
		float brick_width = 10.f;
		float brick_height = 3.f;

		bricks_remaining = 0;

		for (int row = 0; row < BRICK_ROWS; ++row)
		{
			for (int col = 0; col < BRICK_COLUMNS; ++col)
			{
				Brick& b = bricks[row * BRICK_COLUMNS + col];
				b.half_width = brick_width * 0.5f;
				b.half_height = brick_height * 0.5f;
				b.x = start_x + col * (brick_width + gap);
				b.y = start_y + row * (brick_height + gap);
				b.is_alive = true;
				bricks_remaining++;
			}
		}
		bricks_initialized = true;
	}

	// render bricks
	for (int i = 0; i < BRICK_ROWS * BRICK_COLUMNS; ++i)
	{
		if (bricks[i].is_alive)
		{
			draw_rect(bricks[i].x, bricks[i].y, bricks[i].half_width, bricks[i].half_height, 0xFFFF0000); // orange
		}
	}

	// check collision for the bricks
	for (int i = 0; i < BRICK_ROWS * BRICK_COLUMNS; ++i)
	{
		Brick& b = bricks[i];
		if (b.is_alive && aabb_vs_aabb(
			breakout_ball_p_x, breakout_ball_p_y, breakout_ball_half_size, breakout_ball_half_size,
			b.x, b.y, b.half_width, b.half_height))
		{

			b.is_alive = false;
			if (bricks_remaining > 0) {
				bricks_remaining--;
			}

			// intialize particle and direction
			for (int j = 0; j < 10; ++j) {
				for (int i = 0; i < MAX_PARTICLES; ++i)
				{
					if (!particles[i].active)
					{
						particles[i].x = b.x;
						particles[i].y = b.y;

						// just random X/Y velocity between -70 and +70
						particles[i].dx = (random_float() * 2.f - 1.f) * 70.f;
						particles[i].dy = (random_float() * 2.f - 1.f) * 70.f;

						particles[i].life = .5f; // lives for 1 second
						particles[i].active = true;
						break;
					}
				}
			}

			// Basic bounce: just reverse Y for now
			breakout_ball_dp_y *= -1;

			break; // Optional: stop after one collision per frame
		}
	}

	// Player movement
	{
		float paddle_ddp = 0.f;
		if (is_down(BUTTON_A)) paddle_ddp -= 1500.f;
		if (is_down(BUTTON_D)) paddle_ddp += 1500.f;

		simulate_paddle(&paddle_p_x, &paddle_dp_x, paddle_ddp, dt,
			paddle_half_size_x, arena_half_size_x);
	}

	// Clamp paddle inside arena borders
	if (paddle_p_x + paddle_half_size_x > arena_half_size_x)
	{
		paddle_p_x = arena_half_size_x - paddle_half_size_x;
	}
	else if (paddle_p_x - paddle_half_size_x < -arena_half_size_x)
	{
		paddle_p_x = -arena_half_size_x + paddle_half_size_x;
	}

	breakout_ball_p_x += breakout_ball_dp_x * dt;
	breakout_ball_p_y -= breakout_ball_dp_y * dt;

	if (breakout_ball_needs_reset)
	{
		breakout_ball_p_x = 0;
		breakout_ball_p_y = -10;        // near top of arena
		breakout_ball_dp_x = 0;        // no side movement
		breakout_ball_dp_y = 80.f;   // fall straight down
		breakout_ball_needs_reset = false;
	}

	// aabb for ball for walls and paddle
	{
		if (aabb_vs_aabb(
			breakout_ball_p_x, breakout_ball_p_y, breakout_ball_half_size, breakout_ball_half_size,
			paddle_p_x, paddle_p_y, paddle_half_size_x, paddle_half_size_y))
		{
			// Push ball above the paddle
			breakout_ball_p_y = paddle_p_y + paddle_half_size_y + breakout_ball_half_size;

			// Invert Y to bounce up
			breakout_ball_dp_y *= -1;

			// Similar to Pong: use hit position and paddle velocity to affect X
			float hit_offset = breakout_ball_p_x - paddle_p_x; // horizontal offset
			breakout_ball_dp_x = hit_offset * 5.f + paddle_dp_x * 0.75f;

			// Optionally increase vertical speed slightly over time
			breakout_ball_dp_y *= 1.03f;
		}

		if (breakout_ball_p_x + breakout_ball_half_size > arena_half_size_x)
		{
			breakout_ball_p_x = arena_half_size_x - breakout_ball_half_size;
			breakout_ball_dp_x *= -1;
		}
		else if (breakout_ball_p_x - breakout_ball_half_size < -arena_half_size_x)
		{
			breakout_ball_p_x = -arena_half_size_x + breakout_ball_half_size;
			breakout_ball_dp_x *= -1;
		}

		if (breakout_ball_p_y + breakout_ball_half_size > arena_half_size_y)
		{
			breakout_ball_p_y = arena_half_size_y - breakout_ball_half_size;
			breakout_ball_dp_y *= -1;
		}
	}

	// Optional: lose condition
	if (breakout_ball_p_y - breakout_ball_half_size < -arena_half_size_y)
	{
		breakout_ball_needs_reset = true;
	}

	// move particles and change color
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		Particle& p = particles[i];
		if (p.active)
		{
			p.x += p.dx * dt;
			p.y += p.dy * dt;
			p.life -= dt;

			if (p.life <= 0)
			{
				p.active = false;
			}
			else
			{
				float alpha = p.life; // fade out
				// Use a color blending macro or manually pack color if needed
				int color = lerp_color(0xFF000000, 0xFFFF0000, alpha);
				draw_rect(p.x, p.y, 0.5f, 0.5f, color);
			}
		}
	}

	draw_number(bricks_remaining, 0, 40, 1.f, 0xFFFFFF00);
	draw_rect(breakout_ball_p_x, breakout_ball_p_y, breakout_ball_half_size, breakout_ball_half_size, 0xFF0BF100);
	draw_rect(paddle_p_x, paddle_p_y, paddle_half_size_x, paddle_half_size_y, 0xFF0BF100); //player

	if (bricks_remaining == 0)
	{
		breakout_ball_dp_x = 0;
		breakout_ball_dp_y = 0;
		breakout_ball_p_x = 0;
		breakout_ball_p_y = -35;

		draw_text("GAME WON", -25, 10, 1, 0xFF0BF100);
		draw_text("PRESS ENTER TO CONTINUE", -35, 0, .5, 0xFF0BF100);

		if (pressed(BUTTON_ENTER))
		{
			bricks_initialized = false;
			breakout_ball_needs_reset = true;
		}
	}
}