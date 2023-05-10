/*
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols, Jarrod Bennett
 * Modified by Eve
 */ 

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 8000000L
#include <util/delay.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

/* digits_displayed - 1 if digits are displayed on the seven
** segment display, 0 if not. No digits displayed initially.
*/
volatile uint8_t digits_displayed = 0;
/* Seven segment display digit being displayed.
** 0 = right digit; 1 = left digit.
*/
volatile uint8_t seven_seg_cc = 0;

volatile uint8_t longest_line_1 = 0;

volatile uint8_t longest_line_2 = 0;

/* Seven segment display segment values for 0 to 4 */
uint8_t seven_seg_data[10] = {63,6,91,79,102};

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete
	start_screen();
	
	// Loop forever,
	while(1) {
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	
	init_timer0();
	DDRD |= (1<<DDRD3);
	
	DDRA = 0xFF;
	DDRC = 0x01;

	/* Set up timer/counter 1 so that we get an 
	** interrupt 100 times per second, i.e. every
	** 10 milliseconds.
	*/
	OCR1A = 9999; /* Clock divided by 8 - count for 10000 cycles */
	TCCR1A = 0; /* CTC mode */
	TCCR1B = (1<<WGM12)|(1<<CS11); /* Divide clock by 8 */

	/* Enable interrupt on timer on output compare match 
	*/
	TIMSK1 = (1<<OCIE1A);

	/* Ensure interrupt flag is cleared */
	TIFR1 = (1<<OCF1A);
	
	// Turn on global interrupts
	sei();
}

void start_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	move_terminal_cursor(10,10);
	printf_P(PSTR("Teeko"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("CSSE2010 project by Eve Gath 46966168"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	start_display();
	
	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1) {
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S') {
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED) {
			break;
		}
	}
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the game and display
	initialise_game();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	
	uint32_t last_flash_time, current_time, time_difference;
	uint8_t btn; //the button pushed
	char serial_input;
	uint8_t cursor_x, cursor_y;
	
	last_flash_time = get_current_time();
	print_current_player_display();
	PORTD |= (1 << PORTD3);
	
	// We play the game until it's over
	while(!is_game_over()) {
				
		// We need to check if any button has been pushed, this will be
		// NO_BUTTON_PUSHED if no button has been pushed
		btn = button_pushed();
		serial_input = -1;
		cursor_x = get_cursor_x();
		cursor_y = get_cursor_y();

		
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		
		if (valid_move(cursor_x, cursor_y)) {
			PORTD |= (1 << PORTD3);
		}
		else {
			PORTD = 0x00;
		}
		
		
		if (serial_input == 'P' || serial_input == 'p') {
			current_time = get_current_time();
			time_difference = current_time - last_flash_time;
			while (1)
			{
				// recheck serial
				// if 'p', break
				serial_input = -1;
				if (serial_input_available()) {
					serial_input = fgetc(stdin);
				}
					
				if (serial_input == 'P' || serial_input == 'p') {
					break;
				}
			}
			current_time = get_current_time();
			last_flash_time = current_time - time_difference;
				
		}
		
		if (btn == BUTTON3_PUSHED || serial_input == 'a' || serial_input == 'A') {
			// If button 3 is pushed, move left,
			// i.e decrease x by 1 and leave y the same
			if (cursor_x != 0) {
				move_display_cursor(-1, 0);
				//flash?
				last_flash_time = get_current_time();
			}
			else {
				move_display_cursor(4,0);
			}
			
		}
			
		if (btn == BUTTON2_PUSHED || serial_input == 'd' || serial_input == 'D') {
			// If button 2 is pushed, move right,
			// i.e increase x by 1 and leave y the same
			move_display_cursor(1, 0);
			last_flash_time = get_current_time();
		}
		
		if (btn == BUTTON1_PUSHED || serial_input == 'w' || serial_input == 'W') {
			// If button 1 is pushed, move up,
			// i.e increase y by 1 and leave x the same
			move_display_cursor(0, 1);
			last_flash_time = get_current_time();
		}
			
		if (btn == BUTTON0_PUSHED || serial_input == 's' || serial_input == 'S') {
			// If button 0 is pushed, move down,
			// i.e decrease y by 1 and leave x the same
			if (cursor_y != 0) {
				move_display_cursor(0, -1);
				last_flash_time = get_current_time();
			}
			else {
				move_display_cursor(0, 4);
			}
			
		}
		
		if (serial_input == ' ') {
			piece_placement();
			print_current_player_display();
		}

	
		current_time = get_current_time();
		if(current_time >= last_flash_time + 500) {
			// 500ms (0.5 second) has passed since the last time we
			// flashed the cursor, so flash the cursor
			flash_cursor();
			
			// Update the most recent time the cursor was flashed
			last_flash_time = current_time;
		}
	}
	// We get here if the game is over.
}

void handle_game_over() {
	clear_terminal();
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button to start again"));
	
	while(button_pushed() == NO_BUTTON_PUSHED) {
		; // wait
	}
	if (button_pushed()) {
		new_game();
	}
	
}

ISR(TIMER1_COMPA_vect) {
	uint8_t player = get_player();
	/* Change which digit will be displayed. If last time was
	** left, now display right. If last time was right, now 
	** display left.
	*/
	if (!is_game_over()) {
		if (player == PLAYER_1) {
			for (uint8_t i = 0; i < WIDTH; i++) {
				for (uint8_t j = 0; j < HEIGHT; j++) {
					int8_t direction_1 = 0;
					int8_t direction_2 = 0;
					int8_t direction_3 = 0;
					int8_t direction_4 = 0;
					if (get_piece_at(i, j) == PLAYER_1) {
						direction_1 += 1; 
						direction_2 += 1;
						direction_3 += 1;
						direction_4 += 1;
						if (get_piece_at(i+1, j) == PLAYER_1) {
							direction_1 += 1;
							if (get_piece_at(i+2, j) == PLAYER_1) {
								direction_1 += 1;
								if (get_piece_at(i+3, j) == PLAYER_1) {
									direction_1 += 1;
								}
							}
						}
						else if (get_piece_at(i, j+1) == PLAYER_1) {
							direction_2 += 1;
							if (get_piece_at(i, j+2) == PLAYER_1) {
								direction_2 += 1;
								if (get_piece_at(i, j+3) == PLAYER_1) {
									direction_2 += 1;
								}
							}
						} 
						else if (get_piece_at(i+1, j+1) == PLAYER_1) {
							direction_3 += 1;
							if (get_piece_at(i+2, j+2) == PLAYER_1) {
								direction_3 += 1;
								if (get_piece_at(i+3, j+3) == PLAYER_1) {
									direction_3 += 1;
								}
							}
						} 
						else if (get_piece_at(i-1, j) == PLAYER_1) {
							direction_4 += 1;
							if (get_piece_at(i-2, j) == PLAYER_1) {
								direction_4 += 1;
								if (get_piece_at(i-3, j) == PLAYER_1) {
									direction_4 += 1;
								}
							}
						}
						
						if (longest_line_1 < direction_1) {
							longest_line_1 = direction_1;	
						}
						
						if (longest_line_1 < direction_2) {
							longest_line_1 = direction_2;
						}
						
						if (longest_line_1 < direction_3) {
							longest_line_1 = direction_3;
						}
						
						if (longest_line_1 < direction_4) {
							longest_line_1 = direction_4;
						}
					}
				}
			}
		}
			
		if (player == PLAYER_2) {
			for (uint8_t i = 0; i < WIDTH; i++) {
				for (uint8_t j = 0; j < HEIGHT; j++) {
					int8_t direction_1 = 0;
					int8_t direction_2 = 0;
					int8_t direction_3 = 0;
					int8_t direction_4 = 0;
					if (get_piece_at(i, j) == PLAYER_2) {
						direction_1 += 1;
						direction_2 += 1;
						direction_3 += 1;
						direction_4 += 1;
						if (get_piece_at(i+1, j) == PLAYER_2) {
							direction_1 += 1;
							if (get_piece_at(i+2, j) == PLAYER_2) {
								direction_1 += 1;
								if (get_piece_at(i+3, j) == PLAYER_2) {
									direction_1 += 1;
								}
							}
						}
						else if (get_piece_at(i, j+1) == PLAYER_2) {
							direction_2 += 1;
							if (get_piece_at(i, j+2) == PLAYER_2) {
								direction_2 += 1;
								if (get_piece_at(i, j+3) == PLAYER_2) {
									direction_2 += 1;
								}
							}
						} 
						else if (get_piece_at(i+1, j+1) == PLAYER_2) {
							direction_3 += 1;
							if (get_piece_at(i+2, j+2) == PLAYER_2) {
								direction_3 += 1;
								if (get_piece_at(i+3, j+3) == PLAYER_2) {
									direction_3 += 1;
								}
							}
						} 
						else if (get_piece_at(i-1, j) == PLAYER_2) {
							direction_4 += 1;
							if (get_piece_at(i-2, j) == PLAYER_2) {
								direction_4 += 1;
								if (get_piece_at(i-3, j) == PLAYER_2) {
									direction_4 += 1;
								}
							}
						}
						if (longest_line_2 < direction_1) {
							longest_line_2 = direction_1;
						}
							
						if (longest_line_2 < direction_2) {
							longest_line_2 = direction_2;
						}
							
						if (longest_line_2 < direction_3) {
							longest_line_2 = direction_3;
						}
							
						if (longest_line_2 < direction_4) {
							longest_line_2 = direction_4;
						}
					}
				}
			}				
		}
		seven_seg_cc = 1 ^ seven_seg_cc;
		
		if(digits_displayed) {
			/* Display a digit */
			if(seven_seg_cc == 0) {
				/* Display rightmost digit - player 2 */
				PORTA = seven_seg_data[longest_line_2];
				} else {
				/* Display leftmost digit - player 1 */
				PORTA = seven_seg_data[longest_line_1];
			}
			/* Output the digit selection (CC) bit */
			PORTC = seven_seg_cc;
			} else {
			/* No digits displayed -  display is blank */
			PORTA = 0;
		}
	}
}
