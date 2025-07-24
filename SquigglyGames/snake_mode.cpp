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

// snake timer for delay movement(per frame)
float snake_timer = 0.0f;
float snake_move_delay = 0.05f;

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

void simulate_snake(Input* input, float dt)
{
    clear_screen(0xFF211E1E);
    draw_arena_borders(arena_half_size_x, arena_half_size_y, 0xFF0BF100);

    // Spawn food if fewer than 10
    while ((int)food_positions.size() < 10)
    {
        Vec2 new_food = { random_range(-85, 85), random_range(-45, 45) };
        if (!position_occupied(new_food))
            food_positions.push_back(new_food);
    }

    // Draw all food
    for (const Vec2& food : food_positions)
    {
        draw_rect((float)food.x, (float)food.y, 1.0f, 1.0f, 0xFFFF0000);
    }

    // Handle input
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

        // Check if new head is on food
        bool ate_food = false;
        for (auto it = food_positions.begin(); it != food_positions.end(); ++it)
        {
            if (aabb_vs_aabb(
                (float)new_head.x, (float)new_head.y, snake_half_size, snake_half_size,
                (float)it->x, (float)it->y, food_half_size, food_half_size))
            {
                ate_food = true;
                food_positions.erase(it);
                break;
            }
        }

        snake_segments.push_front(new_head);
        if (!ate_food)
        {
            snake_segments.pop_back();  // only remove tail if no food eaten
        }
    }

    // Draw snake (normal and lerp_color)
    /*for (const Vec2& segment : snake_segments)
    {
        draw_rect((float)segment.x, (float)segment.y, 1, 1, 0xFF00FFFF);
    }*/
    int start_color = 0xFF00FFFF; // Magenta head
    int end_color = 0xFF000000; // Black tail

    int count = (int)snake_segments.size();
    for (int i = 0; i < count; ++i)
    {
        float t = (float)i / (float)(count - 1); // 0.0 to 1.0
        int color = lerp_color(start_color, end_color, t);

        Vec2& segment = snake_segments[i];
        draw_rect((float)segment.x, (float)segment.y, 1, 1, color);
    }
}
