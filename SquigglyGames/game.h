#ifndef GAME_H
#define GAME_H

#include "utils.h"
#include "platform_common.h"
#include "renderer.h"

// Game constants
#define BRICK_ROWS 5
#define BRICK_COLUMNS 13
#define MAX_PARTICLES 256

// Input macros
#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

// Game mode enumeration
enum Gamemode {
    GM_MENU,
    GM_GAMEPLAY,
    GM_BREAKOUT,
};

// Game structures
struct Brick {
    float x, y;
    float half_width, half_height;
    bool is_alive;
};

struct Particle {
    float x, y;
    float dx, dy;
    float life;
    float r, g, b;
    bool active;
};

// Game variables (declared as extern in header)
extern float player_1_p, player_1_dp, player_2_p, player_2_dp;
extern float arena_half_size_x, arena_half_size_y;
extern float player_half_size_x, player_half_size_y;
extern float ball_p_x, ball_p_y, ball_dp_x, ball_dp_y, ball_half_size;
extern int player_1_score, player_2_score;

// Breakout variables
extern float paddle_p_x, paddle_p_y, paddle_dp_x;
extern float paddle_half_size_x, paddle_half_size_y;
extern float breakout_ball_p_x, breakout_ball_p_y, breakout_ball_dp_x, breakout_ball_dp_y;
extern float breakout_ball_half_size;
extern bool breakout_ball_needs_reset;

// Game state
extern Gamemode current_gamemode;
extern int hot_button;
extern bool enemy_is_ai;
extern Brick bricks[BRICK_ROWS * BRICK_COLUMNS];
extern bool bricks_initialized;
extern int bricks_remaining;
extern bool breakout_game_won;
extern Particle particles[MAX_PARTICLES];

// Function declarations
 void simulate_player(float* p, float* dp, float ddp, float dt);
 void simulate_paddle(float* p, float* dp, float ddp, float dt, float half_size, float arena_half_size);
 bool aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y, float p2x, float p2y, float hs2x, float hs2y);
 void simulate_game(Input* input, float dt);
float random_float();
int lerp_color(int a, int b, float t);

#endif