#ifndef SNAKE_MODE_H
#define SNAKE_MODE_H

#include "platform_common.h"
#include <deque>

struct Vec2
{
	int x, y;
};
void simulate_snake(Input* input, float dt);
int random_range(int min, int max);

#endif