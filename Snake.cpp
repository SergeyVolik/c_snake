// Snake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream> 
#include <Windows.h>
#include <cassert>

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

struct Snake_BodyPart
{
	Vector2_Int position;
	char simbol;
};

struct Snake
{
	Snake_BodyPart* snake_body;
	int body_size;
	Vector2_Int head_move_direction;
	Vector2_Int tail_last_position;
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

Vector2_Int move_input = { 1, 0 };

bool Vector2_equals(const Vector2_Int& vec1, const Vector2_Int& vec2);

void grid_draw(Grid* grid, Reward* reward, Snake* player_snake);
void app_update_time();
void game_update();
void time_init();
int random_int(int min, int max);
int teleport_if_on_boarder(int min, int max, int current);

Snake* snake_create();
void snake_move(Snake* snake, const Rect2D_Int& playable_rect);
Snake_BodyPart snake_get_head(Snake* snake);
void snake_extend_body(Snake* snake);
bool snake_has_intersections(Snake* snake);
void snake_delete(Snake* snake);

void input_read();
void game_init();

Grid grid_create(int width, int height);
void grid_delete(Grid* grid);

void fill_console_character(char character, int x, int y);
void delete_game_data();
void game_restart();

Vector2_Int reward_generate_random_position(const Rect2D_Int& rect, Snake* snake);


Vector2_Int Vector2_Int_vector(const Vector2_Int& start_pos, const Vector2_Int& end_pos);
float Vector2_Int_dot(const Vector2_Int& vec1, const Vector2_Int& vec2);

Vector2_Int Vector2_Int_vector(const Vector2_Int& start_pos, const Vector2_Int& end_pos)
{
	Vector2_Int vec;
	vec.x = end_pos.x - start_pos.x;
	vec.y = end_pos.y - start_pos.y;
	return vec;
}

float Vector2_Int_dot(const Vector2_Int& vec1, const Vector2_Int& vec2)
{
	return ((float)vec1.x) * ((float)vec2.x) + ((float)vec1.y) * ((float)vec2.y);
}

void reward_collect(Reward* reward, Snake* snake);

const char shake_head_char = '0';//254;
const char shake_tail_char = 'o';//254;
const char reward_char = 254;
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
const int R_KEY = 0x52;

const int UP_KEY = VK_UP;
const int LEFT_KEY = VK_LEFT;
const int RIGHT_KEY = VK_RIGHT;
const int DOWN_KEY = VK_DOWN;
const int ESCAPE_KEY = VK_ESCAPE;
const int SPACE_KEY = VK_SPACE;


float move_snake_t = 0;
float move_tick_time = 0.3f;
float delta_time = 0;
float total_time = 0;
clock_t old_time;
int score;
int max_score;
float speed = 1;
float speed_increase = 0.1f;

bool exit_app = false;
bool pause = false;

HANDLE console_handle_in;
HANDLE console_handle_out;
DWORD num_of_events;

Reward reward;
Grid global_grid;
Snake* player_snake;

int main()
{
	srand(unsigned int(time(NULL)));

	game_init();

	move_snake_t = move_tick_time;
	while (exit_app == false)
	{
		input_read();
		app_update_time();

		if (pause == true)
			continue;

		game_update();
	}

	delete_game_data();
	return 0;
}

void game_init()
{
	//get console handles
	console_handle_in = GetStdHandle(STD_INPUT_HANDLE);
	console_handle_out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX cfi = {sizeof(cfi)};
	
	if (0 == GetCurrentConsoleFontEx(console_handle_out, true, &cfi))
	{
		printf("error: %i", GetLastError());
	}

	cfi.dwFontSize.X = 10;
	cfi.dwFontSize.Y = 30;
	
	SetCurrentConsoleFontEx(console_handle_out, true, &cfi);
	player_snake = snake_create();

	time_init();

	int grid_width_size = 20;
	int grid_height_size = 20;


	global_grid = grid_create(grid_width_size, grid_height_size);
	reward.position = reward_generate_random_position(global_grid.playeable_rect, player_snake);
}

void game_restart()
{
	delete_game_data();
	game_init();
	score = 0;
	speed = 1;
}

bool snake_has_intersections(Snake* snake)
{
	Snake_BodyPart part = snake_get_head(snake);

	for (int i = 1; i < snake->body_size; i++)
	{
		if (Vector2_equals(part.position, snake->snake_body[i].position))
		{
			return true;
		}
	}

	return false;
}

bool Vector2_equals(const Vector2_Int& vec1, const Vector2_Int& vec2)
{
	return vec1.x == vec2.x && vec1.y == vec2.y;
}

Snake_BodyPart snake_get_head(Snake* snake)
{
	return snake->snake_body[0];
}

void snake_extend_body(Snake* snake)
{
	int old_size = snake->body_size;
	int new_size = old_size + 1;

	Snake_BodyPart* new_parts = new Snake_BodyPart[new_size];

	for (int i = 0; i < old_size; i++)
	{
		new_parts[i] = snake->snake_body[i];
	}

	Snake_BodyPart bodypart;

	bodypart.simbol = shake_tail_char;
	bodypart.position = snake->tail_last_position;

	new_parts[old_size] = bodypart;

	delete[] snake->snake_body;

	snake->snake_body = new_parts;
	snake->body_size = new_size;
}

void reward_collect(Reward* reward, Snake* snake)
{
	Snake_BodyPart body = snake_get_head(snake);

	bool is_collected = Vector2_equals(body.position, reward->position);

	if (is_collected)
	{
		snake_extend_body(snake);
		reward->position = reward_generate_random_position(global_grid.playeable_rect, player_snake);
		score += 1;
		speed += speed_increase;
	}
}

Vector2_Int reward_generate_random_position(const Rect2D_Int& rect, Snake* snake)
{	
	Vector2_Int result;

	while (true)
	{
		result.x = random_int(rect.min_x, rect.max_x);
		result.y = random_int(rect.min_y, rect.max_y);
		bool is_valid = true;

		for (int i = 0; i < snake->body_size; i++)
		{
			if (Vector2_equals(result, snake->snake_body[i].position))
			{
				is_valid = false;
				break;
			}
		}

		if (is_valid == true)
			break;
	}

	return result;
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
	move_snake_t += delta_time * speed;

	if (move_snake_t > move_tick_time)
	{
		move_snake_t = 0;
		reward_collect(&reward, player_snake);
		snake_move(player_snake, global_grid.playeable_rect);
		grid_draw(&global_grid, &reward, player_snake);
		
		
		printf("score: %i max score: %i\n", score, max_score);

		if (snake_has_intersections(player_snake))
		{
			pause = true;
			max_score = score;
			printf("you are dead!\n");
		}
		else
		{
			printf("move: W A S D \n");
		}

		printf("restart: R\n");
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
	for (int i = 0; i < grid->height; i++)
	{
		delete[] grid->grid[i];
	}

	delete[] grid->grid;
}

void input_read()
{
	INPUT_RECORD inp;

	// Read Console Input
	bool hasInput = GetNumberOfConsoleInputEvents(console_handle_in, &num_of_events);

	if (hasInput == false)
	{
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
			move_input = { 0, -1 };		
			break;

		case LEFT_KEY:
		case A_KEY:
			move_input = { -1, 0 };
			break;

		case DOWN_KEY:
		case S_KEY:
			move_input = { 0, 1 };
			break;

		case RIGHT_KEY:
		case D_KEY:
			move_input = { 1, 0 };
			break;

		case ESCAPE_KEY:
			exit_app = true;
			break;

		case SPACE_KEY:
			pause = !pause;
			break;

		case R_KEY:
			pause = false;
			game_restart();
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

	snake_obj->head_move_direction = {1, 0};
	snake_obj->snake_body = new Snake_BodyPart[1];

	Snake_BodyPart snakePart = Snake_BodyPart();

	snakePart.simbol = shake_head_char;
	snakePart.position = Vector2_Int{ 2, 1 };
	snake_obj->snake_body[0] = snakePart;
	snake_obj->tail_last_position = Vector2_Int{ 1, 1 };
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

void snake_move(Snake* snake, const Rect2D_Int& playable_rect)
{
	Vector2_Int prev_pos = { 0, 0 };

	Snake_BodyPart head_part = snake->snake_body[0];

	Vector2_Int next_head_pos = head_part.position;
	Vector2_Int tail_last_position = head_part.position;

	int minX = playable_rect.min_x;
	int maxX = playable_rect.max_x;
	int minY = playable_rect.min_y;
	int maxY = playable_rect.max_y;

	//printf("head vec x: %i y: %i \n", snake->head_move_direction.x, snake->head_move_direction.y);
	//printf("input vec x: %i y: %i \n", move_input.x, move_input.y);
	if (snake->body_size > 1)
	{
		Snake_BodyPart second_part = snake->snake_body[1];

		Vector2_Int tail_pos = second_part.position;
		Vector2_Int head_pos = head_part.position;

		//Vector2_Int tail_vec = Vector2_Int_vector(tail_pos, head_pos);

		next_head_pos = head_part.position;

		next_head_pos.x += move_input.x;
		next_head_pos.y += move_input.y;

		next_head_pos.x = teleport_if_on_boarder(minX, maxX, next_head_pos.x);
		next_head_pos.y = teleport_if_on_boarder(minY, maxY, next_head_pos.y);

		//float dot = Vector2_Int_dot(move_input, tail_vec);
		
		//printf("tail vec x: %i y: %i \n", tail_vec.x, tail_vec.y);

		//printf("dot %f \n", dot);

		if (false == Vector2_equals(next_head_pos, tail_pos))
		{
			snake->head_move_direction = move_input;
		}
	}
	else
	{
		snake->head_move_direction = move_input;
	}
	//printf("head new vec x: %i y: %i \n", snake->head_move_direction.x, snake->head_move_direction.y);

	next_head_pos = head_part.position;

	next_head_pos.x += snake->head_move_direction.x;
	next_head_pos.y += snake->head_move_direction.y;

	next_head_pos.x = teleport_if_on_boarder(minX, maxX, next_head_pos.x);
	next_head_pos.y = teleport_if_on_boarder(minY, maxY, next_head_pos.y);
	

	

	head_part.position = next_head_pos;

	snake->snake_body[0] = head_part;

	for (int i = 1; i < snake->body_size; i++)
	{
		Snake_BodyPart next_part = snake->snake_body[i];
		auto pos = next_part.position;
		next_part.position = tail_last_position;
		tail_last_position = pos;
		snake->snake_body[i] = next_part;
	}

	snake->tail_last_position = tail_last_position;
}

void grid_draw(Grid* grid, Reward* reward, Snake* snake)
{
	int width = grid->width;
	int height = grid->height;

	COORD coordScreen = { 0, height };
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

	for (int i = 0; i < snake->body_size; i++)
	{
		Snake_BodyPart bodyPart = snake->snake_body[i];

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

void app_update_time()
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
