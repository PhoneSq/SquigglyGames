#include "snake_mode.h"
#include "game.h" // to access global state
#include "renderer.h"
#include "utils.h"
#include <cstdlib>  // for rand(), srand()
#include <ctime>    // for time()

using namespace std;

static Vec2 direction = { 1, 0 }; // initially moving right
static deque<Vec2> snake_segments = { {10, 10} }; // start at grid (10, 10)
static deque<Vec2> food_positions;
static bool snake_game_over = false;
static bool snake_reset_requested = false;

int food_eaten = 0;
// snake timer for delay movement(per frame)
float snake_timer = 0.0f;
float snake_move_delay = 0.06f; // Made it a bit slower for better gameplay
const float snake_half_size = 1.0f;    // 1x1 block
const float food_half_size = 1.0f;     // slightly bigger to be easier to hit

int random_range(int min, int max)
{
    static bool seeded = false;
    if (!seeded)
    {
        srand((unsigned int)time(nullptr));
        seeded = true;
    }
    return min + (rand() % (max - min + 1));
}

bool position_occupied(const Vec2& pos)
{
    for (const Vec2& segment : snake_segments)
        if (segment.x == pos.x && segment.y == pos.y)
            return true;
    for (const Vec2& food : food_positions)
        if (food.x == pos.x && food.y == pos.y)
            return true;
    return false;
}

bool check_wall_collision(const Vec2& head)
{
    // Check if head hits arena boundaries
    return (head.x <= -arena_half_size_x || head.x >= arena_half_size_x ||
        head.y <= -arena_half_size_y || head.y >= arena_half_size_y);
}

bool check_self_collision(const Vec2& head)
{
    // Check if head collides with any body segment (skip the first segment which is the current head)
    for (size_t i = 1; i < snake_segments.size(); ++i)
    {
        if (head.x == snake_segments[i].x && head.y == snake_segments[i].y)
            return true;
    }
    return false;
}

void create_death_particles()
{
    // Create particles for each snake segment
    for (const Vec2& segment : snake_segments)
    {
        // Create multiple particles per segment for better effect
        for (int j = 0; j < 3; ++j)
        {
            for (int i = 0; i < MAX_PARTICLES; ++i)
            {
                if (!particles[i].active)
                {
                    particles[i].x = (float)segment.x;
                    particles[i].y = (float)segment.y;

                    // Random velocity in all directions
                    particles[i].dx = (random_float() * 2.f - 1.f) * 100.f;
                    particles[i].dy = (random_float() * 2.f - 1.f) * 100.f;

                    particles[i].life = 1.0f + random_float() * 0.5f; // Lives for 1-1.5 seconds
                    particles[i].active = true;
                    break;
                }
            }
        }
    }
}

void reset_snake_game()
{
    snake_segments.clear();
    snake_segments = { {10, 10} }; // Reset to starting position
    direction = { 1, 0 }; // Reset direction
    food_positions.clear();
    food_eaten = 0;
    snake_game_over = false;
    snake_reset_requested = false;
    snake_timer = 0.0f;

    // Clear any remaining particles
    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        particles[i].active = false;
    }
}

void simulate_snake(Input* input, float dt)
{
    clear_screen(0xFF211E1E);
    draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xFF0BF100);

    // Handle game over state
    if (snake_game_over)
    {
        // Update and draw particles
        for (int i = 0; i < MAX_PARTICLES; ++i)
        {
            Particle& p = particles[i];
            if (p.active)
            {
                p.x += p.dx * dt;
                p.y += p.dy * dt;
                p.life -= dt;

                // Add gravity effect
                p.dy -= 50.0f * dt;

                if (p.life <= 0)
                {
                    p.active = false;
                }
                else
                {
                    float alpha = p.life; // fade out
                    int color = lerp_color(0xFF000000, 0xFF00FFFF, alpha); // Fade from cyan to black
                    draw_rect(p.x, p.y, 0.5f, 0.5f, color);
                }
            }
        }

        // Draw game over screen
        draw_text("GAME OVER", -30, 10, 1.5f, 0xFF0BF100);
        draw_text("FINAL SCORE", -25, 0, 0.8f, 0xFFFFFFFF);
        draw_number(food_eaten, 0, -10, 1.0f, 0xFFFFFF00);
        draw_text("PRESS ENTER TO PLAY AGAIN", -45, -25, 0.6f, 0xFF0BF100);

        if (pressed(BUTTON_ENTER))
        {
            reset_snake_game();
        }
        return;
    }

    // Normal gameplay continues here

    // Spawn food if fewer than 10
    while ((int)food_positions.size() < 10)
    {
        Vec2 new_food = {
            random_range(-((int)arena_half_size_x - 2), ((int)arena_half_size_x - 2)),
            random_range(-((int)arena_half_size_y - 2), ((int)arena_half_size_y - 2))
        };
        if (!position_occupied(new_food))
            food_positions.push_back(new_food);
    }

    // Draw all food
    for (const Vec2& food : food_positions)
    {
        draw_rect((float)food.x, (float)food.y, food_half_size, food_half_size, 0xFFFF0000);
    }

    // Handle input - prevent 180 degree turns
    if (pressed(BUTTON_A) && direction.x != 1)  direction = { -1, 0 };
    if (pressed(BUTTON_D) && direction.x != -1) direction = { 1, 0 };
    if (pressed(BUTTON_S) && direction.y != 1)  direction = { 0, -1 };
    if (pressed(BUTTON_W) && direction.y != -1) direction = { 0, 1 };

    snake_timer += dt;
    if (snake_timer >= snake_move_delay)
    {
        snake_timer = 0.0f;
        Vec2 head = snake_segments.front();
        Vec2 new_head = { head.x + direction.x, head.y + direction.y };

        // Check for collisions BEFORE moving
        if (check_wall_collision(new_head) || check_self_collision(new_head))
        {
            snake_game_over = true;
            create_death_particles();
            return; // Exit early to prevent further movement
        }

        // Check if new head is on food
        bool ate_food = false;
        for (auto it = food_positions.begin(); it != food_positions.end(); ++it)
        {
            if (aabb_vs_aabb(
                (float)new_head.x, (float)new_head.y, snake_half_size, snake_half_size,
                (float)it->x, (float)it->y, food_half_size, food_half_size))
            {
                ate_food = true;
                food_eaten++;
                food_positions.erase(it);

                // Slightly increase speed as snake grows
                if (snake_move_delay > 0.05f)
                    snake_move_delay *= 0.98f;
                break;
            }
        }

        snake_segments.push_front(new_head);
        if (!ate_food)
        {
            snake_segments.pop_back();  // only remove tail if no food eaten
        }
    }

    // Draw snake with gradient colors
    int start_color = 0xFF00FFFF; // Cyan for head
    int end_color = 0xFF004040;   // Dark teal for tail
    int count = (int)snake_segments.size();

    for (int i = 0; i < count; ++i)
    {
        float t = (count > 1) ? (float)i / (float)(count - 1) : 0.0f; // 0.0 to 1.0
        int color = lerp_color(start_color, end_color, t);
        const Vec2& segment = snake_segments[i];
        draw_rect((float)segment.x, (float)segment.y, snake_half_size, snake_half_size, color);
    }

    // Draw score
    /*draw_number(food_eaten, 0, 40, 1, 0xFFFFFF00);*/

    // Draw current length
    draw_text("LENGTH", 0, 40, 0.5f, 0xFFFFFFFF);
    draw_number((int)snake_segments.size(), 20, 38, 0.8f, 0xFF00FFFF);
}