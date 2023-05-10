# Teeko_C-
Teeko 5x5 game made with C++ code and AVR Atmega324A

Teeko Description
This project involves creating a replica of the board game ‘Teeko’. Teeko is a turn-based game
played between two players on a 5x5 board. Each player has a set of 4 coloured pieces (green for
player 1, red for player 2) they must take turns placing on the board. A player wins the game if
they manage to place all of their pieces in a line (which may be a diagonal, vertical or horizontal
line). Note that these lines do not wrap around the edges of the board.
The game consists of two phases, referred to in this document as game phase 1 and game phase
2.
1. In game phase 1, the two players take turns placing their 4 pieces on the board. These
pieces may be placed on any empty space on the board. This phase ends when all 8 pieces
(4 green pieces + 4 red pieces) have been placed on the board.
2. In game phase 2, players take turns picking up one of their pieces and moving it to one of
the adjacent empty spaces. An adjacent space is one of the 8 surrounding spaces but does
not wrap around the edges of the board. A piece cannot be placed in the same space it was
picked up.
Note: In proper Teeko, players can also win by placing each of their pieces in a square of 4
adjacent spaces, however for this assignment you DO NOT need to consider this.
These rules are also described well on the Teeko Wikipedia page. Another useful resource is
teeko.cc, which is an online version of the game you may wish to compare your implementation
against. NOTE: these two resources allow players to win by placing pieces in a square as in the
note above. Once again, you do not need to implement this behaviour.


Initial Operation
The provided program has very limited functionality. It will display a start screen which detects
the rising edge on the push buttons B0, B1, B2 and B3, and also the input terminal character ‘s’.
Pressing of any of these will start a game with the start configuration and a flashing cursor.
Once started, the program detects a rising edge on the button B3, but no action is taken on this
input.

Minimum Performance (Level 0 – Pass/Fail)
Your program must have at least the features present in the code supplied to you, i.e., it must build
and run, show the start screen, and display the initial board when a button or ‘s’ is pressed. No
marks can be earned for other features unless this requirement is met, i.e., your project mark will
be zero.

Start Screen (Level 1 – 4 marks)
Modify the program so that when it starts (i.e. the AVR microcontroller is reset) it outputs your
name and student number to the serial terminal. Do this by modifying the function
start_screen() in file project.c.

Move Cursor with Buttons (Level 1 – 12 marks)
The provided program does not allow for the cursor to be moved. Modify the program so that
button B3 (connected to pin B3 for IN students, connected to pin C3 for EX students) moves the
cursor to the left, button B2 moves the cursor right, button B1 moves the cursor up and button B0
moves the cursor down. (B3 = left, B2 = right, B1 = up, B0 = down). If the cursor moves off the
display, it should wrap around to the opposite side. For example, if the cursor is at space (3,4) and
attempts to move upwards, it should wrap to the bottom to (3,0). When the cursor moves, it must
be visible immediately and should restart its flashing cycle i.e. the cursor should not flash until
500 ms after the cursor has been moved. You may need to modify/relocate some of the existing
cursor flashing code seen in project.c.
In the play_game()function in the file project.c, when button 3 is pressed, the function
move_display_cursor(dx,dy) in the file game.c is called. This function is currently
empty, start by filling in the move_display_cursor function, there are some hints to get
you started. Then update play_game()to detect the other buttons as well.

Move Cursor with Terminal Input (Level 1 – 5 marks)
The provided program does not register any terminal inputs once the game has started. Modify
the program such that pressing ‘w’/‘W’ moves the cursor upwards, ‘a’/‘A’ moves the cursor to
the left, ‘s’/‘S’ moves the cursor downwards and ‘d’/‘D’ moves the cursor to the right. Both the
lower case and upper case of each letter must move the cursor.
On the start screen, the game can be started by pressing ‘s’ or ‘S’, looking at the function
start_screen() should give you an idea of how to read serial input from the terminal.
Note that if you press the directional arrows, they might also move your cursor in an unexpected
direction, this is because they use escape characters that contain ‘A’ and ‘D’. We will not be considering 
escape characters when testing and will only use the letters of the alphabet, digits and spacebar as potential inputs.

Game Phase 1 (Level 1 – 8 marks)
(This assumes that you have implemented “Move Cursor with Buttons/terminal input” above.)
Modify the program so that a piece can be placed at the current location of the cursor when the
spacebar is pressed. For this level, the piece played does not have to be a legal move as per game
phase 2, however you are not allowed to place a piece on top of another piece. When a piece is
successfully placed, the current player should switch (it starts as player 1/green). If a player tries
to place a piece on top of another piece, this move should be rejected and the current player should
not change.

Turn Indicator (Level 1 – 6 marks)
(This assumes that you have implemented “Game Phase 1” above.)
Display the current players turn and the corresponding colour on the terminal in a fixed position.
Add appropriate text (e.g. “Current player: 1/2, (green/red)”) to the terminal display so it is clear
which players turn it is. As the first player to place a piece is player 1, the terminal should show
“Current player: 1 (green)” (or similar) when the game begins. This text should be updated only
when it changes i.e. you should not be continuously printing the current player to the terminal.

Game Phase 2 (Level 1 – 10 marks)
(This assumes that you have implemented “Game Phase 1” above.)
After each player places down their 4 pieces, pick up a piece by pressing the space bar while the
cursor is above a piece. You should only be able to pick up the current player’s pieces. Once a
piece has been picked up, you can place it again with the space bar after moving the cursor to a
legal space (any one of the 8 space around where the piece was picked up that is empty). Note:
you cannot place a piece at the same location the piece was picked up. These 8 spaces do not
wrap around the board like the cursor does (only the cursor wraps around the board). The cursor
should flash a different colour while a piece is picked up. Several (unused) colours have been
provided in pixel_colour.h (IN) /terminalio.h (EX).

Valid Move Detection on LED (Level 1 – 7 marks)
(Assumes “Game Phase 1/2” is implemented) Display on an LED whether placing a piece in the
current cursor location is a valid move or not. This LED should be on when the move is valid,
otherwise the LED should be off. This must work for both game phase 1 and 2. In phase 1, any
move is valid as long as it is not on top of another piece. In phase 2, both selecting and placing a
held piece must be displayed on the LED. When picking up a piece the LED should be on when
the cursor is above one of the current player’s pieces. When holding a piece, the LED should be
on when the cursor is above one of the 8 empty spaces around the space the piece was picked up.
This LED must update as soon as a move is made.

Game Over (Level 1 – 12 marks)
(Assumes “Game Phase 1/2” is implemented)
Add functionality to detect if a player has won the game i.e. successfully placed 4 pieces in a row,
column or diagonal. If a win is detected then a message should be displayed on the terminal
indicating which player has won the game. A new game should be able to be started after the game
has ended. Some of this code has already been provided for you in project.c.
Hint: To detect a win has occurred, the following steps may be taken:
1. Locate each of the current player’s pieces.
2. For each piece, check if a line of 3 other pieces exists in each of the 8-possible directions.
Some more advice:
• Ensure you only check positions that are within the game board.
• Loops can be used to check each direction methodically. One possible solution is to define
arrays of directions to search. Directions can be defined in terms of the x and y directions
as in move_display_cursor(dx,dy).
• You must not only search around the piece just placed. Consider the situation where the
final move is placing a piece between the other 3 pieces.

Visual Display of Legal Moves (Level 2 – 7 marks)
In addition to displaying whether a move is valid on the LED, also show all of the legal moves on
the display (LED matrix (IN)/terminal (EX)) when holding a piece. You do not need to show legal
moves in phase 1 of the game or when no piece has been picked up. The legal moves must be
shown in a new colour. Several (unused) colours have been provided in pixel_colour.h (IN)
/terminalio.h (EX).

Longest Line Display (Level 2 – 7 marks)
(This assumes that you have implemented “Game Phase 1” above.)
Display on the seven segment display the current longest line of pieces each player has. The
longest line of player 1 (green) should be shown on the left digit and the longest line of player 2
(red) should be shown on the right digit. No display flickering should be apparent. Both digits
(when displayed) should be of equal brightness. Include any connections required on your
submitted feature summary form. At the start of the game ‘00’ should be displayed. You should
be able to extend the functionality implemented to detect game over for this.

Game Pause (Level 2 – 7 marks)
Modify the program so that if the ‘p’ or ‘P’ key on the serial terminal is pressed then the game
will pause. When ‘p’ or ‘P’ key on the serial terminal is pressed again, the game recommences.
(All other button/key presses/inputs should be discarded whilst the game is paused – i.e. the cursor
cannot be moved, and no pieces can be placed.) The cursor flashing must be unaffected – e.g. if
the pause happens 200ms before the cursor is going to flash off, then the cursor should not flash
off until 200ms after the game is resumed, not immediately upon resume. Other functionality such
as the seven segment display should be unaffected i.e. both digits of the seven segment display
should still be shown (without ghosting etc.) if implemented.
