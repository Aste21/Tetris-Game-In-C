#include "primlib.h"
#include "shapes.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#define WIDTH_OF_GAME_FIELD 10
#define HEIGHT_OF_GAME_FIELD 20
#define BLOCKS_ON_TOP_OF_GAME_FIELD 4
#define SCREEN_MARGIN 50
#define SIZE_OF_NEXT 10
#define TETROMINO_FALLING_SPEED 30
#define NUMBER_OF_TETROMINOS 7
#define NUMBER_OF_ROTATIONS 4
#define HORIZONTAL_MOVE_DELAY 4
#define DELAY_OF_GAME 16
#define TEXT_GAP 20
#define SIZE_OF_TET_ARRAY 4
#define NUMBER_OF_TILES_IN_SINGLE_TETROMINO 4
#define STARTING_TETROMINO_Y BLOCKS_ON_TOP_OF_GAME_FIELD / 2
#define DELAY_AT_END 2000
#define DELAY_AT_LOSING_MOMENT 1000

void clear_screen()
{
	gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), BLACK);
}

void drawing_grid(int grid_coord[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD][2], int size_of_block)
{
	for (int num_of_column = 0; num_of_column < WIDTH_OF_GAME_FIELD; num_of_column++)
	{
		for (int num_of_row = BLOCKS_ON_TOP_OF_GAME_FIELD; num_of_row < (HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD); num_of_row++)
		{
			gfx_rect(grid_coord[num_of_column][num_of_row][0], 
					 grid_coord[num_of_column][num_of_row][1], 
					 grid_coord[num_of_column][num_of_row][0] + size_of_block, 
					 grid_coord[num_of_column][num_of_row][1] + size_of_block, WHITE);
		}
	}
}

int finding_x_border_corner(int block_nr, int block_x, int block_y, int rotation_nr, 
							char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY])
{
	for (int num_of_column = 0; num_of_column < SIZE_OF_TET_ARRAY; num_of_column++)
	{
		for (int num_of_row = 0; num_of_row < SIZE_OF_TET_ARRAY; num_of_row++)
		{
			if (rotations[block_nr][rotation_nr][num_of_column][num_of_row] == 2)
			{
				return block_x - num_of_column;
			}
		}
	}
	return 1;
}

int finding_y_border_corner(int block_nr, int block_x, int block_y, int rotation_nr, 
							char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY])
{
	for (int num_of_column = 0; num_of_column < SIZE_OF_TET_ARRAY; num_of_column++)
	{
		for (int num_of_row = 0; num_of_row < SIZE_OF_TET_ARRAY; num_of_row++)
		{
			if (rotations[block_nr][rotation_nr][num_of_column][num_of_row] == 2)
			{
				return block_y - num_of_row;
			}
		}
	}
	return 1;
}

int colision_detection(int block_nr, int corner_x, int corner_y, int rotation_nr, 
					   char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY], 
					   int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD])
{
	/* returns true when there is no collision detected
	   and false if there is a collision detected */
	for (int num_of_column = 0; num_of_column < SIZE_OF_TET_ARRAY; num_of_column++)
	{
		for (int num_of_row = 0; num_of_row < SIZE_OF_TET_ARRAY; num_of_row++)
		{
			// checking if not too far left
			if (corner_x + num_of_column < 0 && rotations[block_nr][rotation_nr][num_of_column][num_of_row] > 0)
			{
				return false;
			}
			else
			{
				if (rotations[block_nr][rotation_nr][num_of_column][num_of_row] > 0)
				{
					// checking if it touches bottom border
					if (corner_y + num_of_row + 1 > HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD)
					{
						return false;
					}
					// checking if there are blocks beneth

					else if (board_state[corner_x + num_of_column][corner_y + num_of_row] > 0 && rotations[block_nr][rotation_nr][num_of_column][num_of_row] > 0)
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

void drawing_tetromino(int block_nr, int corner_x, int corner_y, int rotation_nr, 
					   char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY], 
					   int grid_coord[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD][2], int TILE_SIZE)
{
	int temp_var = 0;
	for (int num_of_column = 0; num_of_column < SIZE_OF_TET_ARRAY; num_of_column++)
	{
		for (int num_of_row = 0; num_of_row < SIZE_OF_TET_ARRAY; num_of_row++)
		{
			if (rotations[block_nr][rotation_nr][num_of_column][num_of_row] == 1)
			{
				gfx_filledRect(grid_coord[corner_x + num_of_column][corner_y + num_of_row][0], 
							   grid_coord[corner_x + num_of_column][corner_y + num_of_row][1], 
							   grid_coord[corner_x + num_of_column][corner_y + num_of_row][0] + TILE_SIZE, 
							   grid_coord[corner_x + num_of_column][corner_y + num_of_row][1] + TILE_SIZE, CYAN);
				temp_var += 1;
			}
			else if (rotations[block_nr][rotation_nr][num_of_column][num_of_row] == 2)
			{
				gfx_filledRect(grid_coord[corner_x + num_of_column][corner_y + num_of_row][0], 
							   grid_coord[corner_x + num_of_column][corner_y + num_of_row][1], 
							   grid_coord[corner_x + num_of_column][corner_y + num_of_row][0] + TILE_SIZE, 
							   grid_coord[corner_x + num_of_column][corner_y + num_of_row][1] + TILE_SIZE, MAGENTA);
				temp_var += 1;
			}
			if (temp_var == NUMBER_OF_TILES_IN_SINGLE_TETROMINO)
			{
				break;
			}
		}
	}
}

void drawing_board(int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD], 
				   int grid_coord[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD][2], int TILE_SIZE)
{
	for (int num_of_column = 0; num_of_column < WIDTH_OF_GAME_FIELD; num_of_column++)
	{
		for (int num_of_row = 0; num_of_row < HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD; num_of_row++)
		{
			if (board_state[num_of_column][num_of_row] == 1)
			{
				gfx_filledRect(grid_coord[num_of_column][num_of_row][0], grid_coord[num_of_column][num_of_row][1], 
							   grid_coord[num_of_column][num_of_row][0] + TILE_SIZE, grid_coord[num_of_column][num_of_row][1] + TILE_SIZE, RED);
			}
		}
	}
}

int is_line_full(int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD], int checked_row)
{
	for (int num_of_column = 0; num_of_column < WIDTH_OF_GAME_FIELD; num_of_column++)
	{
		if (board_state[num_of_column][checked_row] == 0)
		{
			return false;
		}
	}
	return true;
}

void line_clear(int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD], int cleaned_row)
{
	for (int number_of_column = 0; number_of_column < WIDTH_OF_GAME_FIELD; number_of_column++)
	{
		board_state[number_of_column][cleaned_row] = 0;
	}
	for (int number_of_column = 0; number_of_column < WIDTH_OF_GAME_FIELD; number_of_column++)
	{
		for (int number_of_row = cleaned_row; number_of_row > BLOCKS_ON_TOP_OF_GAME_FIELD; number_of_row--)
		{
			board_state[number_of_column][number_of_row] = board_state[number_of_column][number_of_row - 1];
		}
	}
}

void line_handler(int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD])
{
	for (int number_of_row = 0; number_of_row < HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD; number_of_row++)
	{
		if (is_line_full(board_state, number_of_row))
		{
			line_clear(board_state, number_of_row);
		}
	}
}

int is_game_lost(int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD])
{
	for (int number_of_column = 0; number_of_column < WIDTH_OF_GAME_FIELD; number_of_column++)
	{
		for (int number_of_row = 0; number_of_row < BLOCKS_ON_TOP_OF_GAME_FIELD; number_of_row++)
		{
			if (board_state[number_of_column][number_of_row] > 0)
			{
				return true;
			}
		}
	}
	return false;
}

int tetromino_falling(int game_timer, int *corner_x, int *corner_y, int block_nr, int block_x, int block_y, int rotation_nr, 
					  char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY])
{
	if (game_timer >= TETROMINO_FALLING_SPEED)
	{
		block_y += 1;
		*corner_x = finding_x_border_corner(block_nr, block_x, block_y, rotation_nr, rotations);
		*corner_y = finding_y_border_corner(block_nr, block_x, block_y, rotation_nr, rotations);
		return block_y;
	}
	return block_y;
}

void freeze(int block_nr, int corner_x, int corner_y, int rotation_nr, 
			char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY], 
			int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD])
{
	for (int number_of_column = 0; number_of_column < SIZE_OF_TET_ARRAY; number_of_column++)
	{
		for (int number_of_row = 0; number_of_row < SIZE_OF_TET_ARRAY; number_of_row++)
		{
			if (rotations[block_nr][rotation_nr][number_of_column][number_of_row] > 0)
			{
				board_state[corner_x + number_of_column][corner_y + number_of_row] = 1;
			}
		}
	}
}

int freeze_control(int block_nr, int block_x, int block_y, int corner_x, int corner_y, int rotation_nr, 
				   char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY], 
				   int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD])
{
	if (!colision_detection(block_nr, corner_x, corner_y, rotation_nr, rotations, board_state))
	{
		block_y -= 1;
		corner_y -= 1;
		freeze(block_nr, corner_x, corner_y, rotation_nr, rotations, board_state);
		return true;
	}
	return false;
}

int overflow_control(int game_timer)
{
	if (game_timer >= TETROMINO_FALLING_SPEED)
	{
		return 1;
	}
	return game_timer;
}

void new_tetromino(int *tetromino_x, int *tetromino_y, int *tetromino_nr, int *tetromino_rotation_nr, int *next_tetromino, int *corner_x, int *corner_y, 
				   char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY])
{
	*tetromino_x = WIDTH_OF_GAME_FIELD / 2;
	*tetromino_y = STARTING_TETROMINO_Y;
	*tetromino_nr = *next_tetromino;
	*next_tetromino = rand() % NUMBER_OF_TETROMINOS;
	if (*tetromino_nr == 1)
	{
		*tetromino_rotation_nr = 1;
	}
	else
	{
		*tetromino_rotation_nr = 0;
	}
	*corner_x = finding_x_border_corner(*tetromino_nr, *tetromino_x, *tetromino_y, *tetromino_rotation_nr, rotations);
	*corner_y = finding_y_border_corner(*tetromino_nr, *tetromino_x, *tetromino_y, *tetromino_rotation_nr, rotations);
}

int tetromino_rotation_handler(int tetromino_rotation, int *key_down_last_frame, int block_nr, int *corner_x, int *corner_y, 
							   char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY], 
							   int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD], int block_x, int block_y)
{
	int temp_corner_x = finding_x_border_corner(block_nr, block_x, block_y, (tetromino_rotation + 1) % NUMBER_OF_ROTATIONS, rotations);
	int temp_corner_y = finding_y_border_corner(block_nr, block_x, block_y, (tetromino_rotation + 1) % NUMBER_OF_ROTATIONS, rotations);
	if (gfx_isKeyDown(SDLK_SPACE) && !*key_down_last_frame && colision_detection(block_nr, temp_corner_x, temp_corner_y, 
		(tetromino_rotation + 1) % NUMBER_OF_ROTATIONS, rotations, board_state))
	{
		// finding furtest right x of tetromino after rotation
		int furthest_x = 0;
		for (int number_of_column = 0; number_of_column < SIZE_OF_TET_ARRAY; number_of_column++)
		{
			for (int number_of_row = 0; number_of_row < SIZE_OF_TET_ARRAY; number_of_row++)
			{
				if (rotations[block_nr][(tetromino_rotation + 1) % NUMBER_OF_ROTATIONS][number_of_column][number_of_row] > 0 && number_of_column > furthest_x)
				{
					furthest_x = number_of_column;
				}
			}
		}
		*key_down_last_frame = true;
		if (finding_x_border_corner(block_nr, block_x, block_y, (tetromino_rotation + 1) % NUMBER_OF_ROTATIONS, rotations) < 0 || 
			finding_x_border_corner(block_nr, block_x, block_y, (tetromino_rotation + 1) % NUMBER_OF_ROTATIONS, rotations) + furthest_x > WIDTH_OF_GAME_FIELD)
		{
			return tetromino_rotation;
		}
		tetromino_rotation = (tetromino_rotation + 1) % NUMBER_OF_ROTATIONS;
		*corner_x = finding_x_border_corner(block_nr, block_x, block_y, tetromino_rotation, rotations);
		*corner_y = finding_y_border_corner(block_nr, block_x, block_y, tetromino_rotation, rotations);
	}
	return tetromino_rotation;
}

void reset_game_state(int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD])
{
	for (int num_of_column = 0; num_of_column < WIDTH_OF_GAME_FIELD; num_of_column++)
	{
		for (int num_of_row = 0; num_of_row < HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD; num_of_row++)
		{
			board_state[num_of_column][num_of_row] = 0;
		}
	}
}

void drawing_next_tetromino(int next_tetromino, char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY], 
							int corner_x, int corner_y, int TILE_SIZE)
{
	for (int number_of_column = 0; number_of_column < SIZE_OF_TET_ARRAY; number_of_column++)
	{
		for (int number_of_row = 0; number_of_row < SIZE_OF_TET_ARRAY; number_of_row++)
		{
			if (rotations[next_tetromino][0][number_of_column][number_of_row] == 1)
			{
				gfx_filledRect(corner_x + number_of_column * TILE_SIZE, corner_y + number_of_row * TILE_SIZE, 
							   corner_x + (number_of_column + 1) * TILE_SIZE, corner_y + (number_of_row + 1) * TILE_SIZE, CYAN);
			}

			else if (rotations[next_tetromino][0][number_of_column][number_of_row] == 2)
			{
				gfx_filledRect(corner_x + number_of_column * TILE_SIZE, corner_y + number_of_row * TILE_SIZE, corner_x + (number_of_column + 1) * TILE_SIZE, 
				 			   corner_y + (number_of_row + 1) * TILE_SIZE, MAGENTA);
			}
		}
	}
}

int key_input_handler(int *is_right, int *is_left, int *right_time, int *left_time, int tet_x, int tet_y, int tetromino_rotation, int block_nr, 
					  char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY], 
					  int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD], int *corner_x_f, int *corner_y_f)
{
	*is_right = gfx_isKeyDown(SDLK_RIGHT);
	*is_left = gfx_isKeyDown(SDLK_LEFT);

	if (*is_right)
	{
		*right_time += 1;
	}
	else
	{
		*right_time = 0;
	}

	if (*is_left)
	{
		*left_time += 1;
	}
	else
	{
		*left_time = 0;
	}

	if (*is_right && *is_left)
	{
		return tet_x;
	}
	else if (*is_right && (*right_time == 1 || *right_time > HORIZONTAL_MOVE_DELAY))
	{
		if (colision_detection(block_nr, finding_x_border_corner(block_nr, tet_x + 1, tet_y, tetromino_rotation, rotations),
							   finding_y_border_corner(block_nr, tet_x + 1, tet_y, tetromino_rotation, rotations), tetromino_rotation, rotations, board_state))
		{
			tet_x = tet_x + 1;
		}
	}
	else if (*is_left && (*left_time == 1 || *left_time > HORIZONTAL_MOVE_DELAY))
	{
		if (colision_detection(block_nr, finding_x_border_corner(block_nr, tet_x - 1, tet_y, tetromino_rotation, rotations),
							   finding_y_border_corner(block_nr, tet_x + 1, tet_y, tetromino_rotation, rotations), tetromino_rotation, rotations, board_state))
		{
			tet_x = tet_x - 1;
		}
	}

	*corner_x_f = finding_x_border_corner(block_nr, tet_x, tet_y, tetromino_rotation, rotations);
	*corner_y_f = finding_y_border_corner(block_nr, tet_x, tet_y, tetromino_rotation, rotations);

	return tet_x;
}

void tetromino_instafall_handler(int tetromino_rotation, int *key_down_last_frame, int block_nr, int *corner_x, int *corner_y,
								 char rotations[NUMBER_OF_TETROMINOS][NUMBER_OF_ROTATIONS][SIZE_OF_TET_ARRAY][SIZE_OF_TET_ARRAY],
								 int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD], int block_x, int *block_y)
{

	if (gfx_isKeyDown(SDLK_DOWN) && !*key_down_last_frame)
	{
		*key_down_last_frame = true;
		while (colision_detection(block_nr, *corner_x, *corner_y, tetromino_rotation, rotations, board_state))
		{
			*block_y += 1;
			*corner_x = finding_x_border_corner(block_nr, block_x, *block_y, tetromino_rotation, rotations);
			*corner_y = finding_y_border_corner(block_nr, block_x, *block_y, tetromino_rotation, rotations);
		}
		*block_y -= 1;
		*corner_x = finding_x_border_corner(block_nr, block_x, *block_y, tetromino_rotation, rotations);
		*corner_y = finding_y_border_corner(block_nr, block_x, *block_y, tetromino_rotation, rotations);
	}
}

void setting_values_constants(int *SIZE_OF_BLOCK_F, int *size_of_next_var_f, int *X_COORD_NEXT_TET_F, int *Y_COORD_NEXT_TET_F, 
						      int GRID_COORDINATES_F[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD][2])
{
	if ((gfx_screenWidth() - 2 * SCREEN_MARGIN) / WIDTH_OF_GAME_FIELD > (gfx_screenHeight() - 2 * SCREEN_MARGIN) / 
		 (HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD))
	{
		*SIZE_OF_BLOCK_F = (gfx_screenHeight() - 2 * SCREEN_MARGIN) / (HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD);
		*size_of_next_var_f = *SIZE_OF_BLOCK_F;
		*X_COORD_NEXT_TET_F = gfx_screenWidth() / 2 + WIDTH_OF_GAME_FIELD / 2 * *SIZE_OF_BLOCK_F + (gfx_screenWidth() / 2 - 
							  WIDTH_OF_GAME_FIELD / 2 * *SIZE_OF_BLOCK_F) / 2 - SIZE_OF_TET_ARRAY / 2 * *SIZE_OF_BLOCK_F;
		*Y_COORD_NEXT_TET_F = gfx_screenHeight() / 2 - (SIZE_OF_TET_ARRAY / 2) * *SIZE_OF_BLOCK_F;
		if (*X_COORD_NEXT_TET_F < gfx_screenWidth() / 2 + WIDTH_OF_GAME_FIELD / 2 * *SIZE_OF_BLOCK_F)
		{
			*X_COORD_NEXT_TET_F = gfx_screenWidth() - (SIZE_OF_TET_ARRAY + 1) * SIZE_OF_NEXT;
			*Y_COORD_NEXT_TET_F = SIZE_OF_NEXT;
			*size_of_next_var_f = SIZE_OF_NEXT;
		}
	}
	else
	{
		*SIZE_OF_BLOCK_F = (gfx_screenWidth() - 2 * SCREEN_MARGIN) / WIDTH_OF_GAME_FIELD;
		*size_of_next_var_f = *SIZE_OF_BLOCK_F;
		*X_COORD_NEXT_TET_F = gfx_screenWidth() / 2 - (SIZE_OF_TET_ARRAY / 2) * *SIZE_OF_BLOCK_F;
		*Y_COORD_NEXT_TET_F = gfx_screenHeight() / 2 + HEIGHT_OF_GAME_FIELD / 2 * *SIZE_OF_BLOCK_F + 
							  (gfx_screenHeight() / 2 - HEIGHT_OF_GAME_FIELD / 2 * *SIZE_OF_BLOCK_F) / 2 - SIZE_OF_TET_ARRAY / 2 * *SIZE_OF_BLOCK_F;
		if (*Y_COORD_NEXT_TET_F > gfx_screenHeight() / 2 - (HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD) / 2 * *SIZE_OF_BLOCK_F)
		{
			*X_COORD_NEXT_TET_F = gfx_screenWidth() - (SIZE_OF_TET_ARRAY + 1) * SIZE_OF_NEXT;
			*Y_COORD_NEXT_TET_F = SIZE_OF_NEXT;
			*size_of_next_var_f = SIZE_OF_NEXT;
		}
	}

	/*
	X_OF_TOP_RIGHT_GRID = gfx_screenWidth() / 2 - WIDTH_OF_GAME_FIELD / 2 * SIZE_OF_BLOCK;
	Y_OF_TOP_RIGHT_GRID = gfx_screenHeight() / 2 - (HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD) / 2 * SIZE_OF_BLOCK;
	*/

	for (int num_of_column = 0; num_of_column < WIDTH_OF_GAME_FIELD; num_of_column++)
	{
		for (int num_of_row = 0; num_of_row < (HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD); num_of_row++)
		{
			GRID_COORDINATES_F[num_of_column][num_of_row][0] = gfx_screenWidth() / 2 - WIDTH_OF_GAME_FIELD / 2 * *SIZE_OF_BLOCK_F + *SIZE_OF_BLOCK_F * num_of_column;
			GRID_COORDINATES_F[num_of_column][num_of_row][1] = gfx_screenHeight() / 2 - (HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD) / 
															   2 * *SIZE_OF_BLOCK_F + *SIZE_OF_BLOCK_F * num_of_row;
		}
	}
}

void game_over_screen_handler(int *game_over_screen_f, int *is_main_loop_on_f, int *is_game_on_f, 
							  int board_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD])
{
	gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), WHITE);
	gfx_textout(gfx_screenWidth() / 2, gfx_screenHeight() / 2, "YOU LOST!!!", RED);
	gfx_textout(gfx_screenWidth() / 2, gfx_screenHeight() / 2 + TEXT_GAP, "PRESS SPACE TO TRY AGAIN", GREEN);
	gfx_textout(gfx_screenWidth() / 2, gfx_screenHeight() / 2 + 2 * TEXT_GAP, "PRESS ESCAPE TO EXIT", BLACK);

	gfx_updateScreen();

	int key_input_game_over = 0;
	key_input_game_over = gfx_getkey();

	if (key_input_game_over == SDLK_SPACE)
	{
		*game_over_screen_f = false;
		*is_game_on_f = true;
		reset_game_state(board_state);
	}
	else if (key_input_game_over == SDLK_ESCAPE)
	{
		*game_over_screen_f = false;
		*is_main_loop_on_f = false;
	}
}

void last_frame_handling(int* space_last, int* down_last)
{
	if (!gfx_isKeyDown(SDLK_SPACE))
	{
		*space_last = false;
	}
	if (!gfx_isKeyDown(SDLK_DOWN))
	{
		*down_last = false;
	}
}

int main(int argc, char *argv[])
{

	gfx_init();

	srand(time(NULL));

	clear_screen();

	gfx_updateScreen();

	// array made of 0 and 1. 0 is a empty tile, 1 is a full tile
	int game_state[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD] = {0};

	int GRID_COORDINATES[WIDTH_OF_GAME_FIELD][HEIGHT_OF_GAME_FIELD + BLOCKS_ON_TOP_OF_GAME_FIELD][2];

	// setting the size of a single tile
	int SIZE_OF_BLOCK;

	int X_COORD_NEXT_TET;
	int Y_COORD_NEXT_TET;
	int size_of_next_var;

	setting_values_constants(&SIZE_OF_BLOCK, &size_of_next_var, &X_COORD_NEXT_TET, &Y_COORD_NEXT_TET, GRID_COORDINATES);

	// game handling variables
	int is_game_on = true;
	int game_over_screen = false;
	int is_main_loop_on = true;
	int game_time_tracker = 0;

	int space_last_frame = false;
	int down_last_frame = false;

	// tetromino position variables
	int current_tetromino_x;
	int current_tetromino_y;
	int current_corner_x;
	int current_corner_y;

	// tetrominos shape and rotation variables
	int current_tetromino_shape;
	int next_tetromino_shape = rand() % NUMBER_OF_TETROMINOS;
	int current_tetromino_rotation;

	int is_left_down = false;
	int time_of_left_down = 0;
	int is_right_down = false;
	int time_of_right_down = 0;

	new_tetromino(&current_tetromino_x, &current_tetromino_y, &current_tetromino_shape, &current_tetromino_rotation, &next_tetromino_shape, &current_corner_x,
				  &current_corner_y, pieces);

	while (is_main_loop_on)
	{
		while (is_game_on)
		{
			clear_screen();

			// key handling
			tetromino_instafall_handler(current_tetromino_rotation, &down_last_frame, current_tetromino_shape, &current_corner_x, &current_corner_y,
										pieces, game_state, current_tetromino_x, &current_tetromino_y);
			current_tetromino_rotation = tetromino_rotation_handler(current_tetromino_rotation, &space_last_frame, current_tetromino_shape, &current_corner_x,
																	&current_corner_y, pieces, game_state, current_tetromino_x, current_tetromino_y);
			current_tetromino_x = key_input_handler(&is_right_down, &is_left_down, &time_of_right_down, &time_of_left_down, current_tetromino_x, current_tetromino_y,
													current_tetromino_rotation, current_tetromino_shape, pieces, game_state, &current_corner_x, &current_corner_y);


			last_frame_handling(&space_last_frame, &down_last_frame);

			// time tracker handling
			game_time_tracker += 1;
			current_tetromino_y = tetromino_falling(game_time_tracker, &current_corner_x, &current_corner_y, current_tetromino_shape, current_tetromino_x,
													current_tetromino_y, current_tetromino_rotation, pieces);
			game_time_tracker = overflow_control(game_time_tracker);

			// freeze handling
			if (freeze_control(current_tetromino_shape, current_tetromino_x, current_tetromino_y, current_corner_x, current_corner_y, current_tetromino_rotation,
							   pieces, game_state))
			{
				new_tetromino(&current_tetromino_x, &current_tetromino_y, &current_tetromino_shape, &current_tetromino_rotation,
							  &next_tetromino_shape, &current_corner_x, &current_corner_y, pieces);
			}

			// line handling
			line_handler(game_state);

			// drawing 1/2
			drawing_board(game_state, GRID_COORDINATES, SIZE_OF_BLOCK);

			// handling loosing
			if (is_game_lost(game_state))
			{
				// delay is intended, its a way for the player to see the board state at the moment of loosing
				SDL_Delay(DELAY_AT_LOSING_MOMENT);
				is_game_on = false;
				game_over_screen = true;
				break;
			}

			// drawing 2/2
			drawing_tetromino(current_tetromino_shape, current_corner_x, current_corner_y, current_tetromino_rotation, pieces, GRID_COORDINATES, SIZE_OF_BLOCK);
			drawing_next_tetromino(next_tetromino_shape, pieces, X_COORD_NEXT_TET, Y_COORD_NEXT_TET, size_of_next_var);
			drawing_grid(GRID_COORDINATES, SIZE_OF_BLOCK);
			gfx_updateScreen();

			// delay at the start is set to 16 so the FPS = 60
			SDL_Delay(DELAY_OF_GAME);
		}
		while (game_over_screen)
		{
			game_over_screen_handler(&game_over_screen, &is_main_loop_on, &is_game_on, game_state);
		}
	}

	// drawing goodbye screen

	clear_screen();
	gfx_textout(gfx_screenWidth() / 2, gfx_screenHeight() / 2, "GOODBYE! :D", WHITE);
	gfx_updateScreen();
	SDL_Delay(DELAY_AT_END);

	return 0;
}
