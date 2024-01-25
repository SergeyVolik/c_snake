// Snake.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
#include <stdio.h>
#include <time.h>
#include "header.h"

int main()
{
	srand((unsigned int)time(NULL));

	game_init();

	move_snake_t = move_tick_time;
	while (exit_app == 0)
	{
		input_read();
		app_update_time();

		if (pause == 1)
			continue;

		game_update();
	}

	delete_game_data();
	return 0;
}