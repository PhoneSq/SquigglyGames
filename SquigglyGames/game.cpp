#include "game.h"
#include "breakout_mode.h"
#include <stdlib.h>

// Game variables definitions
float player_1_p, player_1_dp, player_2_p, player_2_dp;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_p_x, ball_p_y, ball_dp_x = 130, ball_dp_y, ball_half_size = 1;
int player_1_score, player_2_score;

// Breakout variables
float paddle_p_x = 0.f;
float paddle_p_y = -40;
float paddle_dp_x = 0.f;
float paddle_half_size_x = 10.f;
float paddle_half_size_y = 2.5f;
float breakout_ball_p_x = 0.f;
float breakout_ball_p_y = 0.f;
float breakout_ball_dp_x = 100.f;
float breakout_ball_dp_y = 100.f;
float breakout_ball_half_size = 1.f;
bool breakout_ball_needs_reset = true;

// Game state
Gamemode current_gamemode;
int hot_button;
bool enemy_is_ai;
Brick bricks[BRICK_ROWS * BRICK_COLUMNS];
bool bricks_initialized = false;
int bricks_remaining = 0;
Particle particles[MAX_PARTICLES];

float random_float()
{
    return (float)rand() / (float)RAND_MAX;
}

int lerp_color(int a, int b, float t)
{
    unsigned char a_r = (a >> 16) & 0xFF;
    unsigned char a_g = (a >> 8) & 0xFF;
    unsigned char a_b = a & 0xFF;

    unsigned char b_r = (b >> 16) & 0xFF;
    unsigned char b_g = (b >> 8) & 0xFF;
    unsigned char b_b = b & 0xFF;

    unsigned char r = (unsigned char)((1 - t) * a_r + t * b_r);
    unsigned char g = (unsigned char)((1 - t) * a_g + t * b_g);
    unsigned char b_ = (unsigned char)((1 - t) * a_b + t * b_b);

    return 0xFF000000 | (r << 16) | (g << 8) | b_;
}

void
simulate_player(float* p, float* dp, float ddp, float dt)
{
    ddp -= *dp * 10.f;

    *p = *p + *dp * dt + ddp * dt * dt * .5f;
    *dp = *dp + ddp * dt;

    if (*p + player_half_size_y > arena_half_size_y)
    {
        *p = arena_half_size_y - player_half_size_y;
        *dp = 0;
    }
    else if (*p - player_half_size_y < -arena_half_size_y)
    {
        *p = -arena_half_size_y + player_half_size_y;
        *dp = 0;
    }
}

 void
simulate_paddle(float* p, float* dp, float ddp, float dt,
    float half_size, float arena_half_size)
{
    ddp -= *dp * 10.f;

    *p = *p + *dp * dt + ddp * dt * dt * 0.5f;
    *dp = *dp + ddp * dt;

    if (*p + half_size > arena_half_size)
    {
        *p = arena_half_size - half_size;
        *dp = 0;
    }
    else if (*p - half_size < -arena_half_size)
    {
        *p = -arena_half_size + half_size;
        *dp = 0;
    }
}

 bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
    float p2x, float p2y, float hs2x, float hs2y)
{
    return (p1x + hs1x > p2x - hs2x &&
        p1x - hs1x < p2x + hs2x &&
        p1y + hs1y > p2y - hs2y &&
        p1y - hs1y < p2y + hs2y);
}

 void
simulate_game(Input* input, float dt)
{
	// green 0xFF0BF100
	// black 0xFF211E1E
	/*clear_screen(0xFF211E1E);
	draw_rect(0, 0, 87, 47, 0xFF0BF100);
	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xFF211E1E);*/

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xFF211E1E);
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xFF0BF100);

	//float speed = 5.f; // units per second
	//if (pressed(BUTTON_UP)) player_pos_y += speed;
	//if (pressed(BUTTON_DOWN)) player_pos_y -= speed;
	//if (pressed(BUTTON_RIGHT)) player_pos_x += speed;
	//if (pressed(BUTTON_LEFT)) player_pos_x -= speed;

	//if (is_down(BUTTON_UP)) player_pos_y += 0.3f;
	//if (is_down(BUTTON_DOWN)) player_pos_y -= 0.3f;
	//if (is_down(BUTTON_RIGHT)) player_pos_x += 0.3f;
	//if (is_down(BUTTON_LEFT)) player_pos_x -= 0.3f;

	// make it run with frame per second.
	//float speed = 60.f; // units per second
	//if (is_down(BUTTON_UP)) player_pos_y += speed * dt;
	//if (is_down(BUTTON_DOWN)) player_pos_y -= speed * dt;
	//if (is_down(BUTTON_RIGHT)) player_pos_x += speed * dt;
	//if (is_down(BUTTON_LEFT)) player_pos_x -= speed * dt;

	// draw_rect(player_pos_x, player_pos_y, 1, 1, 0xFF0BF100);
		if (current_gamemode == GM_GAMEPLAY)
		{
			// p1 input movement (AI)
			float player_1_ddp = 0.f;
			//if (is_down(BUTTON_UP)) player_1_ddp += 2000;
			//if (is_down(BUTTON_DOWN)) player_1_ddp -= 2000;
			if (!enemy_is_ai)
			{
				if (is_down(BUTTON_UP)) player_1_ddp += 2000;
				if (is_down(BUTTON_DOWN)) player_1_ddp -= 2000;
			}
			else
			{
				//if (ball_p_y > player_1_p+2.f) player_1_ddp += 1300;
			//if (ball_p_y < player_1_p-2.f) player_1_ddp -= 1300;
				player_1_ddp = (ball_p_y - player_1_p) * 100;
				if (player_1_ddp > 1300) player_1_ddp = 1300;
				if (player_1_ddp < -1300) player_1_ddp = -1300;
			}

			// p2 input movement
			float player_2_ddp = 0.f;
			if (is_down(BUTTON_W)) player_2_ddp += 2000;
			if (is_down(BUTTON_S)) player_2_ddp -= 2000;

			simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
			simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);

			// p1 physics (speed and collision)
			/*player_1_ddp -= player_1_dp * 10.f;

			player_1_p = player_1_p + player_1_dp * dt + player_1_ddp * dt * dt * .5f;
			player_1_dp = player_1_dp + player_1_ddp * dt;

			if (player_1_p + player_half_size_y > arena_half_size_y)
			{
				player_1_p = arena_half_size_y - player_half_size_y;
				player_1_dp = 0;
			}
			else if (player_1_p - player_half_size_y < -arena_half_size_y)
			{
				player_1_p = -arena_half_size_y + player_half_size_y;
				player_1_dp = 0;
			}*/

			// p2 physics (speed and collision)
			/*player_2_ddp -= player_2_dp * 10.f;

			player_2_p = player_2_p + player_2_dp * dt + player_2_ddp * dt * dt * .5f;
			player_2_dp = player_2_dp + player_2_ddp * dt;

			if (player_2_p + player_half_size_y > arena_half_size_y)
			{
				player_2_p = arena_half_size_y - player_half_size_y;
				player_2_dp = 0;
			}
			else if (player_2_p - player_half_size_y < -arena_half_size_y)
			{
				player_2_p = -arena_half_size_y + player_half_size_y;
				player_2_dp = 0;
			}*/

			// Simulate ball
			{
				// ball movement
				ball_p_x += ball_dp_x * dt;
				ball_p_y += ball_dp_y * dt;


				draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xFF0BF100);

				// ball collision
				if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size,
					80, player_1_p, player_half_size_x, player_half_size_y))
				{
					ball_p_x = 80 - player_half_size_x - ball_half_size;
					ball_dp_x *= -1;
					//ball_dp_y = player_1_dp * .75f;
					//ball_dp_y = (ball_p_y - player_1_p)*2;
					ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp * .75f;
				}
				else if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size,
					-80, player_2_p, player_half_size_x, player_half_size_y))
				{
					ball_p_x = -80 + player_half_size_x + ball_half_size;
					ball_dp_x *= -1;
					//ball_dp_y = player_2_dp * .75f;
					//ball_dp_y = (ball_p_y - player_2_p)*2;
					ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp * .75f;
				}

				if (ball_p_y + ball_half_size > arena_half_size_y)
				{
					ball_p_y = arena_half_size_y - ball_half_size;
					ball_dp_y *= -1;
				}
				else if (ball_p_y - ball_half_size < -arena_half_size_y)
				{
					ball_p_y = -arena_half_size_y + ball_half_size;
					ball_dp_y *= -1;
				}

				// win simulate and counter
				if (ball_p_x + ball_half_size > arena_half_size_x)
				{
					ball_dp_x *= -1;
					ball_dp_y = 0;
					ball_p_x = 0;
					ball_p_y = 0;
					player_1_score++;
				}
				else if (ball_p_x - ball_half_size < -arena_half_size_x)
				{
					ball_dp_x *= -1;
					ball_dp_y = 0;
					ball_p_x = 0;
					ball_p_y = 0;
					player_2_score++;
				}
			}

			// score with rectangles
			//float at_x = -80;
			//for (int i = 0; i < player_1_score; i++)
			//{
			//	draw_rect(at_x, 47.f, 1.f, 1.f, 0xFF0BF100);
			//	at_x += 2.5f;
			//}

			//at_x = 80;
			//for (int i = 0; i < player_2_score; i++)
			//{
			//	draw_rect(at_x, 47.f, 1.f, 1.f, 0xFF0BF100);
			//	at_x -= 2.5f;
			//}

			// Score system
			draw_number(player_1_score, -10, 40, 1.f, 0xFF0BF100);
			draw_number(player_2_score, 10, 40, 1.f, 0xFF0BF100);

			// rendering
			draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xFF0BF100);
			draw_rect(80, player_1_p, player_half_size_x, player_half_size_y, 0xFF0BF100);
			draw_rect(-80, player_2_p, player_half_size_x, player_half_size_y, 0xFF0BF100);
		}
		else if (current_gamemode == GM_BREAKOUT)
		{
			simulate_breakout(input, dt);
		}
		else
		{
			if (pressed(BUTTON_UP))
			{
				hot_button = (hot_button + 2) % 3; // go left (wraps around)
			}
			if (pressed(BUTTON_DOWN))
			{
				hot_button = (hot_button + 1) % 3; // go right
			}

			if (pressed(BUTTON_ENTER))
			{
				if (hot_button == 0)
				{
					current_gamemode = GM_GAMEPLAY;
					enemy_is_ai = true;
				}
				else if (hot_button == 1)
				{
					current_gamemode = GM_GAMEPLAY;
					enemy_is_ai = false;
				}
				else if (hot_button == 2)
				{
					current_gamemode = GM_BREAKOUT;
				}
			}

			//if (hot_button == 0)
			//{
			//	draw_text("SIGLE PLAYER", -80, -10, 1, 0xFFFFFFFF);
			//	draw_text("PLAY WITH AI", -80, -20, .5, 0xFFFFFFFF);
			//	draw_text("MULTIPLAYER", 20, -10, 1, 0xFF0BF100);
			//	draw_text("PLAY WITH FRIEND", 20, -20, .5, 0xFF0BF100);
			//	//draw_rect(20, 0, 10, 10, 0xFF1BD86E);
			//	//draw_rect(-20, 0, 10, 10, 0xFF0BF100);
			//}
			//else
			//{
			//	draw_text("SIGLE PLAYER", -80, -10, 1, 0xFF0BF100);
			//	draw_text("PLAY WITH AI", -80, -20, .5, 0xFF0BF100);
			//	draw_text("MULTIPLAYER", 20, -10, 1, 0xFFFFFFFF);
			//	draw_text("PLAY WITH FRIEND", 20, -20, .5, 0xFFFFFFFF);
			//	//draw_rect(20, 0, 10, 10, 0xFF0BF100);
			//	//draw_rect(-20, 0, 10, 10, 0xFF1BD86E);
			//}
			draw_text("SIGLE PLAYER", -60, 10, 1, hot_button == 0 ? 0xFFFFFFFF : 0xFF0BF100);
			draw_text("PLAY WITH AI", -60, 0, .5, hot_button == 0 ? 0xFFFFFFFF : 0xFF0BF100);

			draw_text("MULTIPLAYER", -60, -10, 1, hot_button == 1 ? 0xFFFFFFFF : 0xFF0BF100);
			draw_text("PLAY WITH FRIEND", -60, -20, .5, hot_button == 1 ? 0xFFFFFFFF : 0xFF0BF100);

			draw_text("BREAKOUT", -60, -30, 1, hot_button == 2 ? 0xFFFFFFFF : 0xFF0BF100);
			draw_text("BRICK BREAKER MODE", -60, -40, .5, hot_button == 2 ? 0xFFFFFFFF : 0xFF0BF100);

			draw_text("PONG GAME", -55, 40, 2, 0xFF0BF100);
			draw_text("PROGRAMMED BY SQUIGGLY", -75, 20, .5, 0xFF0BF100);
		}
}