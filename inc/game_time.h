#ifndef GAME_TIME1_H
#define GAME_TIME1_H

#include <time.h>

typedef struct AppTime
{
	float delta_time;
	float total_time;

} AppTime;

AppTime glob_time;
clock_t old_time;

AppTime app_time_get();
void app_update_time();

#endif
