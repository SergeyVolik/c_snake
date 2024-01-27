#ifndef MY_HEADER_FILE
#define MY_HEADER_FILE
#include <Windows.h>

typedef struct Vector2_Int
{
	int x;
	int y;
} Vector2_Int;

typedef struct Rect2D_Int
{
	int min_x;
	int max_x;
	int max_y;
	int min_y;
} Rect2D_Int;

typedef struct Snake_BodyPart
{
	Vector2_Int position;
	char simbol;
} Snake_BodyPart;

typedef struct Snake
{
	Snake_BodyPart* snake_body;
	int body_size;
	Vector2_Int head_move_direction;
	Vector2_Int tail_last_position;
} Snake;

typedef struct Reward
{
	Vector2_Int position;
} Reward;

typedef struct Grid
{
	char** grid;
	int width;
	int height;
	Rect2D_Int full_rect;
	Rect2D_Int playeable_rect;
} Grid;

const char shake_head_char;
const char shake_tail_char;
const char reward_char;
const char left_bottom_char;
const char right_bottom_char;
const char left_top_char;
const char right_top_char;
const char hw_char;
const char vw_char;
const char emp_char;

#define true   1
#define false  0
#define W_KEY 			0x57
#define S_KEY	 		0x53
#define D_KEY			0x44
#define A_KEY			0x41
#define R_KEY			0x52

#define UP_KEY			0x26
#define LEFT_KEY		VK_LEFT
#define RIGHT_KEY		VK_RIGHT
#define DOWN_KEY		VK_DOWN
#define ESCAPE_KEY		VK_ESCAPE
#define SPACE_KEY 		VK_SPACE

Vector2_Int move_input;

float move_snake_t;
float move_tick_time;
float delta_time;
float total_time;
clock_t old_time;
int score;
int max_score;
float speed;
float speed_increase;

int exit_app;
int pause;

HANDLE console_handle_in;
HANDLE console_handle_out;
DWORD num_of_events;

Reward reward;
Grid global_grid;
Snake player_snake;

int Vector2_equals(const Vector2_Int vec1, const Vector2_Int vec2);

void grid_draw(Grid* grid, Reward* reward, Snake* player_snake);
void app_update_time();
void game_update();
void time_init();
int random_int(int min, int max);
int teleport_if_on_boarder(int min, int max, int current);

Snake snake_create();
void snake_move(Snake* snake, const Rect2D_Int playable_rect);
Snake_BodyPart snake_get_head(Snake* snake);
void snake_extend_body(Snake* snake);
int snake_has_intersections(Snake* snake);
void snake_delete(Snake* snake);

void input_read();
void game_init();

Grid grid_create(int width, int height);
void grid_delete(Grid* grid);

void fill_console_character(char character, int x, int y);
void delete_game_data();
void game_restart();

Vector2_Int reward_generate_random_position(const Rect2D_Int rect, Snake* snake);
Vector2_Int Vector2_Int_vector(const  Vector2_Int start_pos, const  Vector2_Int end_pos);
float Vector2_Int_dot(const Vector2_Int vec1, const Vector2_Int vec2);
void reward_collect(Reward* reward, Snake* snake);

#endif