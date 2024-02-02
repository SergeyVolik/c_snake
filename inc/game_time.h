#ifndef GAME_TIME_H
#define GAME_TIME_H

#include <time.h>

typedef struct AppTime
{
	float delta_time;
	float total_time;
	int fps;

} AppTime;

AppTime glob_time;

AppTime app_time_get();
void app_update_time();

int app_fps();
void app_update_fps(float delta_time);


#endif
