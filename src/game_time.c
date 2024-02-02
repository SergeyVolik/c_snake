#include "game_time.h"

AppTime glob_time = { 0, 0 };
clock_t old_time = 0;

void app_update_time()
{
	clock_t current_time = clock();

	float t = (float)(current_time - old_time);
	glob_time.delta_time = t / CLOCKS_PER_SEC;
	glob_time.total_time += glob_time.delta_time;
	old_time = current_time;
}

AppTime app_time_get()
{
	AppTime time = glob_time;

	return time;
}