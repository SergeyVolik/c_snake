#include "game_time.h"

AppTime glob_time = { 0, 0, 0 };
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

float fps_t = 0;
float fps_tick_interval = 0.2f;
int frames = 0;
int fps_number = 0;

void app_update_fps(float delta_time)
{
	fps_t += delta_time;
	frames++;

	if (fps_t > fps_tick_interval)
	{
		fps_t = 0;
		fps_number = frames/ fps_tick_interval;
		frames = 0;
	}
}

int app_fps()
{
	return fps_number;
}