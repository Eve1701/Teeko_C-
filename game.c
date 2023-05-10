/*
 * game.c
 *
 * Contains functions relating to the play of the game Teeko
 *
 * Authors: Luke Kamols, Jarrod Bennett
 * Modified by Eve
 */ 

#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "display.h"
#include "terminalio.h"

// Start pieces in the middle of the board
#define CURSOR_X_START ((int)(WIDTH/2))
#define CURSOR_Y_START ((int)(HEIGHT/2))
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define PICKEDUP 63

uint8_t board[WIDTH][HEIGHT];
uint8_t validmoveboard[WIDTH][HEIGHT];
// cursor coordinates should be /* SIGNED */ to allow left and down movement.
// All other positions should be unsigned as there are no negative coordinates.
int8_t cursor_x;
int8_t cursor_y;
uint8_t cursor_visible;
uint8_t current_player;
uint8_t player_pieces_1;
uint8_t player_pieces_2;
int8_t previous_position_x;
int8_t previous_position_y;

void initialise_game(void) {
	
	// initialise the display we are using
	initialise_display();
	
	// initialise the board to be all empty
	for (uint8_t x = 0; x < WIDTH; x++) {
		for (uint8_t y = 0; y < HEIGHT; y++) {
			board[x][y] = EMPTY_SQUARE;
		}
	}
	
	// set the starting player
	current_player = PLAYER_1;

	// also set where the cursor starts
	cursor_x = CURSOR_X_START;
	cursor_y = CURSOR_Y_START;
	cursor_visible = 0;
	player_pieces_1 = 0;
	player_pieces_2 = 0;
	previous_position_x = PICKEDUP;
	previous_position_y = PICKEDUP;
	
}

uint8_t get_piece_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the bounds
	// will be considered empty
	if (x < 0 || x >= WIDTH || y < 0 || y >= WIDTH) {
		return EMPTY_SQUARE;
	} else {
		//if in the bounds, just index into the array
		return board[x][y];
	}
}

uint8_t get_valid_piece_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the bounds
	// will be considered empty
	if (x < 0 || x >= WIDTH || y < 0 || y >= WIDTH) {
		return EMPTY_SQUARE;
		} else {
		//if in the bounds, just index into the array
		return validmoveboard[x][y];
	}
}

void flash_cursor(void) {
	uint8_t valid_at_cursor = validmoveboard[cursor_x][cursor_y];
	if (cursor_visible) {
		// we need to flash the cursor off, it should be replaced by
		// the colour of the piece which is at that location
		uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
		if (valid_at_cursor) {
			update_square_colour(cursor_x, cursor_y, piece_at_cursor);
		} else {
			update_square_colour(cursor_x, cursor_y, piece_at_cursor);
		}
		
	} else {
		// we need to flash the cursor on
		//picked up?
		//use other colour?
		//not? this one:
		if (previous_position_x == PICKEDUP && previous_position_y == PICKEDUP){
			update_square_colour(cursor_x, cursor_y, CURSOR);
		}
		else{// if (previous_position_x != PICKEDUP && previous_position_y != PICKEDUP) 
			update_square_colour(cursor_x, cursor_y, PICKEDUPCURSOR);
		}
	}
	cursor_visible = 1 - cursor_visible; //alternate between 0 and 1
}

// determines whether the move wanted to make is a valid move or not.
// first considers when there is 4 pieces on the board, but a piece hasn't been picked
// up yet, then considers of there is less than 4 pieces on the board, then finally
// considers whether a piece has been picked up and if it is within the 8 surrounding 
// squares. 
uint8_t valid_move(int8_t x, int8_t y) {
	int8_t valid_x;
	int8_t valid_y;
	uint8_t player;
	int8_t player_pieces;
	
	
	valid_x = 0;
	valid_y = 0;
	player = get_player();
	player_pieces = get_player_pieces(player);
	
	valid_x = previous_position_x - x;
	valid_y = previous_position_y - y;
	
	if (previous_position_x == PICKEDUP && previous_position_y == PICKEDUP && board[x][y] == player) {
		if (player_pieces == 4) {
			return 1;
		}
	}	
	else if (previous_position_x == PICKEDUP && previous_position_y == PICKEDUP && board[x][y] == EMPTY_SQUARE) {
		if (player_pieces != 4) {
			return 1;
		}
	}
	else if (previous_position_x != PICKEDUP && previous_position_y != PICKEDUP && board[x][y] == EMPTY_SQUARE) {
		if (ABS(valid_x) <= 1 && ABS(valid_y) <= 1) {
			if (valid_x == 0 && valid_y == 0)
			{
				// not valid move
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	return 0;
}

//check the header file game.h for a description of what this function should do
// (it may contain some hints as to how to move the cursor)
void move_display_cursor(int8_t dx, int8_t dy) {
	uint8_t piece_at_cursor = get_piece_at(cursor_x, cursor_y);
	uint8_t valid_at_cursor = validmoveboard[cursor_x][cursor_y];
	if (valid_at_cursor)
	{
		update_square_colour(cursor_x, cursor_y, MOVESQUARE);
	}
	else
	{
		update_square_colour(cursor_x, cursor_y, piece_at_cursor);
	}
	
	cursor_x = (cursor_x + dx) % WIDTH;
	cursor_y = (cursor_y + dy) % HEIGHT;
	
	if (previous_position_x == PICKEDUP && previous_position_y == PICKEDUP)
	{
		update_square_colour(cursor_x, cursor_y, CURSOR);
	}
	else {
		update_square_colour(cursor_x, cursor_y, PICKEDUPCURSOR);	
	}
	cursor_visible = 1;
	
	//print_current_player_display();
	/*suggestions for implementation:
	 * 1: remove the display of the cursor at the current location
	 *		(and replace it with whatever piece is at that location)
	 * 2: update the positional knowledge of the cursor, this will include
	 *		variables cursor_x, cursor_y and cursor_visible. Make sure you
	 *		consider what should happen if the cursor moves off the board.
	 * 3: display the cursor at the new location
	 * 4: reset the cursor flashing cycle. See project.c for how the cursor
	 *		is flashed.
	 */
	
}

// changes the current player.
void toggle_player(void) {
	if (current_player == PLAYER_1) {
		current_player = PLAYER_2;
	}
	else if (current_player == PLAYER_2) {
		current_player = PLAYER_1;
	}
}

void valid_move_leds(void) {
	int8_t valid_x, valid_y;

	if (previous_position_x != PICKEDUP && previous_position_y != PICKEDUP) {
		for (int8_t i = 0; i < WIDTH; i++) {
			for (int8_t j = 0; j < HEIGHT; j++) {
				valid_x = previous_position_x - i;
				valid_y = previous_position_y - j;
				if (ABS(valid_x) <= 1 && ABS(valid_y) <= 1) {
					if (valid_x == 0 && valid_y == 0) {
						//
					}
					else {
						if (get_piece_at(i,j) == EMPTY_SQUARE) {
							update_square_colour(i, j, MOVESQUARE);
							validmoveboard[i][j] = MOVESQUARE;
						}
					}
				}
			}
		}
	}
	
}

void piece_placement(void) {
	// make it display on board, put piece there in specific colour on the board
	// move global
	
	if (current_player == PLAYER_1) {	
		if (player_pieces_1 == 4 && board[cursor_x][cursor_y] == PLAYER_1) { //pickup
			previous_position_x = cursor_x;
			previous_position_y = cursor_y;
			valid_move_leds();
			board[cursor_x][cursor_y] = EMPTY_SQUARE;
			update_square_colour(cursor_x, cursor_y, EMPTY_SQUARE);
			player_pieces_1 -= 1;
			
		}
		else if (player_pieces_1 < 4) { //place
			if (valid_move(cursor_x, cursor_y)) {
				for (int8_t i = 0; i < WIDTH; i++) {
					for (int8_t j = 0; j < HEIGHT; j++) {
						// Function to check if 1 set in valid move board
						if (get_valid_piece_at(i, j) == MOVESQUARE) {
							update_square_colour(i, j, EMPTY_SQUARE);
							validmoveboard[i][j]= EMPTY_SQUARE;
						}
					}
				}
				board[cursor_x][cursor_y] = PLAYER_1;
				update_square_colour(cursor_x, cursor_y, PLAYER_1);
				player_pieces_1 += 1;
				toggle_player();
				previous_position_x = PICKEDUP;
				previous_position_y = PICKEDUP;
			}
							
		} 
	}
	else if (current_player == PLAYER_2) {
		if (player_pieces_2 == 4 && board[cursor_x][cursor_y] == PLAYER_2) { //pickup
			previous_position_x = cursor_x;
			previous_position_y = cursor_y;
			valid_move_leds();
			board[cursor_x][cursor_y] = EMPTY_SQUARE;
			update_square_colour(cursor_x, cursor_y, EMPTY_SQUARE);
			player_pieces_2 -= 1;
		}
		else if (player_pieces_2 < 4) { //place
			if (valid_move(cursor_x, cursor_y)) {
				for (int8_t i = 0; i < WIDTH; i++) {
					for (int8_t j = 0; j < HEIGHT; j++) {
						// Function to check if 1 set in valid move board
						if (get_valid_piece_at(i, j) == MOVESQUARE) {
							update_square_colour(i, j, EMPTY_SQUARE);
							validmoveboard[i][j]= EMPTY_SQUARE;
						}
					}
				}
				board[cursor_x][cursor_y] = PLAYER_2;
				update_square_colour(cursor_x, cursor_y, PLAYER_2);
				player_pieces_2 += 1;
				toggle_player();
				previous_position_x = PICKEDUP;
				previous_position_y = PICKEDUP;
			}
			
		}
		
	} 
	
}

// returns the current player
uint8_t get_player(void) {
	return current_player;
}

int8_t get_player_pieces(uint8_t player) {
	if (player == PLAYER_1) {
		return player_pieces_1;
	} 
	else if (player == PLAYER_2) {
		return player_pieces_2;
	}
	return 0;
}

uint8_t get_cursor_x(void) {
	return cursor_x;
}

uint8_t get_cursor_y(void) {
	return cursor_y;
}

uint8_t is_game_over(void) {
	uint8_t player = PLAYER_1 + PLAYER_2 - get_player();
	for (uint8_t i = 0; i < WIDTH; i++) {
		for (uint8_t j = 0; j < HEIGHT; j++) {
			if (get_piece_at(i, j) == player) {
				if (get_piece_at(i+1, j) == player) {
					if (get_piece_at(i+2, j) == player) {
						if (get_piece_at(i+3, j) == player) {
							return 1;
						}
					}
				}
				else if (get_piece_at(i, j+1) == player) {
					if (get_piece_at(i, j+2) == player) {
						if (get_piece_at(i, j+3) == player) {
							return 1; 
						}
					}
				} else if (get_piece_at(i+1, j+1) == player) {
					if (get_piece_at(i+2, j+2) == player) {
						if (get_piece_at(i+3, j+3) == player) {
							return 1; 
						}
					}
				} else if (get_piece_at(i-1, j) == player) {
					if (get_piece_at(i-2, j) == player) {
						if (get_piece_at(i-3, j) == player) {
							return 1; 
						}
					}
				}
			}
		}
	}
			
	// Detect if the game is over i.e. if a player has won.
	return 0;
}