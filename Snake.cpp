// Snake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream> 
#include <Windows.h>

enum Snake_Move_Direction
{
	MOVE_LEFT = 0,
	MOVE_RIGHT = 1,
	MOVE_TOP = 2,
	MOVE_BOTTOM = 3
};

struct Vector2_Int
{
	int x;
	int y;
};

struct Rect2D_Int
{
	int min_x;
	int max_x;
	int max_y;
	int min_y;
};

bool Vector2_equals(Vector2_Int vec1, Vector2_Int vec2);

struct Snake_BodyPart
{
	Vector2_Int position;
	char simbol;
};

struct Snake
{
	Snake_BodyPart* snake_body;
	Snake_Move_Direction move_direction;
	int body_size;
};

struct Reward
{
	Vector2_Int position;
};

struct Grid
{
	char** grid;
	int width;
	int height;
	Rect2D_Int full_rect;
	Rect2D_Int playeable_rect;
};

Snake_Move_Direction last_player_move_direction = Snake_Move_Direction::MOVE_RIGHT;

void grid_draw(Grid* grid, Reward* reward, Snake* player_snake);
void update_app_time();
void game_update();
void time_init();
int random_int(int min, int max);
int teleport_if_on_boarder(int min, int max, int current);

Snake* snake_create();
void snake_move(Snake* snake);
Snake_BodyPart snake_get_head(Snake* snake);
void snake_delete(Snake* snake);

void input_read();
void game_init();

Grid grid_create(int width, int height);
int grid_get_min_x();
int grid_get_max_x();
int grid_get_max_y();
int grid_get_min_y();
void grid_delete(Grid* grid);

void fill_console_character(char character, int x, int y);
void delete_game_data();

Vector2_Int reward_generate_random_position();
void reward_collect(Reward* reward, Snake* snake);

const char shake_char = '#';//254;
const char reward_char = 'x';
const char left_bottom_char = 192;
const char right_bottom_char = 217;
const char left_top_char = 218;
const char right_top_char = 191;
const char hw_char = 196;
const char vw_char = 179;
const char emp_char = ' ';

const int W_KEY = 0x57;
const int S_KEY = 0x53;
const int D_KEY = 0x44;
const int A_KEY = 0x41;
const int UP_KEY = VK_UP;
const int LEFT_KEY = VK_LEFT;
const int RIGHT_KEY = VK_RIGHT;
const int DOWN_KEY = VK_DOWN;
const int ESCAPE_KEY = VK_ESCAPE;

float render_rate_sec = 0.02f;
float render_t = 0;
float move_snake_t = 0;

float delta_time = 0;
float total_time = 0;
clock_t old_time;

bool exit_app = false;

const int grid_width_size = 25;
const int grid_height_size = 15;

HANDLE console_handle_in;
HANDLE console_handle_out;
DWORD num_of_events;

Reward reward;
Grid global_grid;
Snake* player_snake;

int main()
{
	game_init();

	while (exit_app == false)
	{
		game_update();
	}

	delete_game_data();
	return 0;
}

int grid_get_min_x()
{
	return 1;
}

int grid_get_max_x()
{
	return grid_width_size - 2;
}

int grid_get_max_y()
{
	return grid_height_size - 2;
}

int grid_get_min_y()
{
	return 1;
}

void game_init()
{
	//init random
	srand(unsigned int(time(NULL)));

	//get console handles
	console_handle_in = GetStdHandle(STD_INPUT_HANDLE);
	console_handle_out = GetStdHandle(STD_OUTPUT_HANDLE);

	player_snake = snake_create();

	time_init();

	global_grid = grid_create(grid_width_size, grid_height_size);
	reward.position = reward_generate_random_position();
}

bool Vector2_equals(Vector2_Int vec1, Vector2_Int vec2)
{
	return vec1.x == vec2.x && vec1.y == vec2.y;
}

Snake_BodyPart snake_get_head(Snake* snake)
{
	return snake->snake_body[0];
}

void snake_extend_body(Snake* snake)
{
	
}

void reward_collect(Reward* reward, Snake* snake)
{
	Snake_BodyPart body = snake_get_head(snake);

	bool is_collected = Vector2_equals(body.position, reward->position);

	if (is_collected)
	{
		reward_generate_random_position();
	}
}

Vector2_Int reward_generate_random_position()
{
	int x = random_int(grid_get_min_x(), grid_get_max_x() + 1);
	int y = random_int(grid_get_min_y(), grid_get_max_y() + 1);

	return Vector2_Int{ x, y };
}

//Sample
// Retrieve a random number between 100 and 200
// min = 100
// max = 201
int random_int(int min, int max)
{
	return min + (rand() % (max - min));
}

Grid grid_create(int width, int height)
{
	Grid grid;

	grid.height = height;
	grid.width = width;


	char** grid_array = new char* [height];

	//create array
	for (int y = 0; y < width; y++) {

		// Declare a memory block
		// of size n
		grid_array[y] = new char[width];

		for (int x = 0; x < width; x++)
		{
			grid_array[y][x] = emp_char;
		}
	}

	Rect2D_Int playable_rect;

	playable_rect.max_x = width - 2;
	playable_rect.min_x = 1;
	playable_rect.max_y = height - 2;
	playable_rect.min_y = 1;

	grid.playeable_rect = playable_rect;

	Rect2D_Int full_rect;

	full_rect.max_x = width - 1;
	full_rect.min_x = 0;
	full_rect.max_y = height - 1;
	full_rect.min_y = 0;

	grid.full_rect = full_rect;

	int max_x = full_rect.max_x;
	int max_y = full_rect.max_y;

	//fill horizontal walls
	for (int x = 0; x < width; x++)
	{
		grid_array[0][x] = hw_char;
		grid_array[max_y][x] = hw_char;
	}

	//fill vertial walls
	for (int y = 0; y < height; y++)
	{
		grid_array[y][0] = vw_char;
		grid_array[y][max_x] = vw_char;
	}

	grid_array[0][0] = left_top_char;
	grid_array[max_y][0] = left_bottom_char;
	grid_array[0][max_x] = right_top_char;
	grid_array[max_y][max_x] = right_bottom_char;
	grid.grid = grid_array;

	return grid;
}

void game_update()
{
	input_read();
	update_app_time();

	render_t += delta_time;

	if (render_t > render_rate_sec)
	{
		render_t -= render_rate_sec;
		grid_draw(&global_grid, &reward, player_snake);
	}

	move_snake_t += delta_time;

	float move_tick_time = 0.2f;
	if (move_snake_t > move_tick_time)
	{
		move_snake_t -= move_tick_time;
		reward_collect(&reward, player_snake);
		snake_move(player_snake);
	}
}

void snake_delete(Snake* snake)
{
	delete player_snake->snake_body;
	delete player_snake;
}

void delete_game_data()
{
	//delete snake
	snake_delete(player_snake);
	grid_delete(&global_grid);
}

void grid_delete(Grid* grid)
{
	for (int i = 0; i < grid_height_size; i++)
	{
		delete[] grid->grid[i];
	}

	delete[] grid->grid;
}

void input_read()
{
	INPUT_RECORD inp;

	// Read Console Input
	//ReadConsoleInput(hIn, &inp, 1, &num_of_events);
	bool hasInput = GetNumberOfConsoleInputEvents(console_handle_in, &num_of_events);

	if (hasInput == false)
	{
		//std::cout << GetLastError();
		return;
	}

	PeekConsoleInput(console_handle_in, &inp, 1, &num_of_events);

	switch (inp.EventType)
	{
	case KEY_EVENT:
		switch (inp.Event.KeyEvent.wVirtualKeyCode)
		{
		case UP_KEY:
		case W_KEY:
			last_player_move_direction = MOVE_TOP;
			break;

		case LEFT_KEY:
		case A_KEY:
			last_player_move_direction = MOVE_LEFT;
			break;

		case DOWN_KEY:
		case S_KEY:
			last_player_move_direction = MOVE_BOTTOM;
			break;

		case RIGHT_KEY:
		case D_KEY:
			last_player_move_direction = MOVE_RIGHT;
			break;

		case ESCAPE_KEY:
			exit_app = true;
			break;
		}

		break;
	}

	FlushConsoleInputBuffer(console_handle_in);
}

void fill_console_character(char character, int x, int y)
{
	HANDLE hConsole = console_handle_out;

	COORD coordScreen = { x, y };    // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	dwConSize = 1;

	// Fill the entire screen with blanks.
	if (!FillConsoleOutputCharacterA(hConsole,        // Handle to console screen buffer
		/*(TCHAR)*/character,      // Character to write to the buffer
		dwConSize,       // Number of cells to write
		coordScreen,     // Coordinates of first cell
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute
		coordScreen,      // Coordinates of first cell
		&cCharsWritten))  // Receive number of characters written
	{
		return;
	}
}

Snake* snake_create()
{
	Snake* snake_obj = new Snake;

	snake_obj->move_direction = Snake_Move_Direction::MOVE_RIGHT;
	snake_obj->snake_body = new Snake_BodyPart[1];

	Snake_BodyPart snakePart = Snake_BodyPart();

	snakePart.simbol = shake_char;
	snakePart.position = Vector2_Int{ 1, 1 };
	snake_obj->snake_body[0] = snakePart;

	snake_obj->body_size = 1;

	return snake_obj;
}

int teleport_if_on_boarder(int min, int max, int current)
{
	if (current > max)
	{
		return min;
	}
	else if (current < min)
	{
		return max;
	}

	return current;
}

void snake_move(Snake* snake)
{
	Vector2_Int prevPos = { 0, 0 };

	Snake_BodyPart bodyPart = snake->snake_body[0];

	Vector2_Int next_head_pos = bodyPart.position;

	snake->move_direction = last_player_move_direction;

	switch (snake->move_direction)
	{
	case MOVE_LEFT:
		next_head_pos.x -= 1;
		break;
	case MOVE_RIGHT:
		next_head_pos.x += 1;
		break;
	case MOVE_TOP:
		next_head_pos.y -= 1;
		break;
	case MOVE_BOTTOM:
		next_head_pos.y += 1;
		break;
	default:
		break;
	}

	int minX = 1;
	int maxX = grid_width_size - 2;
	int minY = 1;
	int maxY = grid_height_size - 2;

	next_head_pos.x = teleport_if_on_boarder(minX, maxX, next_head_pos.x);
	next_head_pos.y = teleport_if_on_boarder(minY, maxY, next_head_pos.y);

	bodyPart.position = next_head_pos;

	snake->snake_body[0] = bodyPart;
}

void grid_draw(Grid* grid, Reward* reward, Snake* player_snake)
{
	int width = grid->width;
	int height = grid->height;

	COORD coordScreen = { width, height };
	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(console_handle_out, coordScreen);
	Vector2_Int reward_pos = reward->position;

	char** grid_array = global_grid.grid;

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			grid_array[i][j] = emp_char;
		}
	}

	grid_array[(int)reward_pos.y][(int)reward_pos.x] = reward_char;

	for (int i = 0; i < player_snake->body_size; i++)
	{
		Snake_BodyPart bodyPart = player_snake->snake_body[i];

		grid_array[(int)bodyPart.position.y][(int)bodyPart.position.x] = bodyPart.simbol;
	}

	//draw grid
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			fill_console_character(grid_array[i][j], j, i);
		}
	}
}

void time_init()
{
	old_time = clock();
}

void update_app_time()
{
	delta_time = (clock() - old_time) / 1000.0f;
	total_time += delta_time;
	old_time = clock();
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
