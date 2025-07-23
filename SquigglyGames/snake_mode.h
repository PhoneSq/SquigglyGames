#ifndef SNAKE_MODE_H
#define SNAKE_MODE_H

#include "platform_common.h"
#include <deque>

int length = 0;
int head_x_position = 0;
int head_y_position = 0;

struct Vec2
{
	int x, y;
};

std::deque<Vec2> snake_segments;

void simulate_snake(Input* input, float dt);

#endif