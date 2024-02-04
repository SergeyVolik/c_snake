#ifndef GAME_MATH_H
#define GAME_MATH_H

#include "linmath.h"

typedef struct Rect
{
	float min_x;
	float max_x;
	float max_y;
	float min_y;
} Rect;

#define PI 3.14f
#define RAD PI / 180

float angle_to_radians(float angle);
void random_init();
int random_int(int min, int max);

#endif
