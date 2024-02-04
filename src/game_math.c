#include "game_math.h"
#include <time.h>
#include <stdlib.h>

float angle_to_radians(float angle)
{
	return angle * RAD;
}

void random_init()
{
	srand((unsigned int)time(NULL));
}

//Sample
// Retrieve a random number between 100 and 200
// min = 100
// max = 201
int random_int(int min, int max)
{
	return min + (rand() % (max - min));
}