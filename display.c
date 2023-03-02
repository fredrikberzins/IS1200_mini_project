/* Written by Felix Bergqvist Widström and Fredrik Berzins (2023) */

/*
	Some functions written 2015 by Axel Isaksson
	This copyright notice added 2015 by F Lundevall
	For copyright and licensing, see file COPYING 

	Code copied:
		spi_send_recv()
		display_init()		(Modified to include pin setups from main function in mipsmain.c)
		display_update()
		display_image() 	(Modified, to handle 128px wide image instead of 32px, removed one argument)
		print_string()		(display_string renamed to fit nameing scheme)
*/

#include "header.h"

// spi_send_recv (from mipslabfunc.c)
uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

// display_init (from mipslabfunc.c)
// Initialize the display by defining
// output pins, clock speed, SPI Master
void display_init(void) {
	/*
		This will set the peripheral bus clock to the same frequency
		as the sysclock. That means 80 MHz, when the microcontroller
		is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;

  	DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

// display_update (from mipslabfunc.c)
// Updates display if you have added any text in textbuffer[][]
void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

// display_clear
// Clears display from all pixels and text (Black screen)
void display_clear(){
	// Loops through rows and sets text buffer to "" by using print_string()
	for(int i = 0; i < 4; i++) {
		print_string( i, "");
	}
	display_update();
	// Loops through all bytes in display array and sets all bits to 1 (for all pixels to be off)
	for(int i = 0; i < 512; i++) {
		display[i] = 0xff;
	}
	display_image(display);		// Print out empty display array/image (Updates screen based on display[])
}

// display_image (from mipslabfunc.c)
// Prints a Image on the screen (modified to print a 128px wide img instead of 32px)
// Arguments: 
// const uint8_t *data: an array of 8-bit values to represent 8 pixels vertically on the display
void display_image(const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x00);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 128; j++)
			spi_send_recv(~data[i*128 + j]);
	}
}
// print_start_screen
// Clears screen and then prints start screen:
//	PONG
//	BY
//	FELIX &
//	FREDRIK
void print_start_screen() {
	display_clear();			// Clear to ensure no text is in the way
	print_string( 0, " PONG");
	print_string( 1, " BY");
	print_string( 2, " FELIX &");
	print_string( 3, " FREDRIK");
    display_update();			// Update to print out strings in textbuffer 
}

// print_end_screen
// Clears screen and then prints end screen:
//	Player NR:#
//	WON PONG
// Argument: 
//	int player: 1(Player 1 won(Left side)), any other(Player 2 won(Right side))
void print_end_screen(int player) {
	display_clear(); 		// Clear display from game related textures
	// Check if player is 1 then print "player 1 won" else "player 2 won"
	if (player == 1) {
		print_string( 0, " PLAYER NR:1");
	} else {
		print_string( 0, " PLAYER NR:2");
	}
	print_string( 1, " WON PONG");
    display_update();		// Update to print out strings in textbuffer
}

// print_string (from mipsfunc.c) with font from misc.c (mipslabdata.c)
// Arguments:
//	int line: int between 0 and 3 (defines what row(a row is 8px high) of the screen to print on)
//	char *str: String with text max of 16 characters
void print_string(int line, char *str) {
	int i;
	if(line >= 0 || line < 4 || str) {
		for(i = 0; i < 16; i++) {
			if(*str) {
				textbuffer[line][i] = *str;
				str++;
			}
			else {
				textbuffer[line][i] = ' ';
			}
		}
	}
}

// print_solid
// Prints a solid block (black or white)
// Arguments:
//	int color: 0 (black), any other (white)
// 	int x1:	int between 0 and display_w-1
//	int y1: int between 0 and display_h-1
//		x1,y1 is the upper left corner of the block
//	int x2: int between 0 and display_w-1
//	int y2: int between 0 and display_h-1
//		x2,y2 is the lower right corner of the block
void print_solid(int color, int x1, int y1, int x2, int y2) {
    // Checks that x1 is smaller then x2
	// If not swap them
    if (x1 > x2) {      
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    // Checks that y1 is smaller then y2
	// If not swap them
    if (y1 > y2) {      
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }
    // Loops through all pixels in specified area.
	for (int x = x1; x <= x2; x++) {
		for(int y = y1; y <= y2; y++) {
			int row = lower_8(y);	// row is the row number, it's used to get the right offset in the array
			int rest_y = y%8;		// rest_y is to get the right bit in the byte
			if (color == 0) {
				// Copies the byte in the array and changes the specific bit to 1.
				// By doing bitwise or 
				display[x+row*128] = display[x+row*128] | power(2, rest_y); 	// Turn off all pixels within x1,y1,x2,y2
			} else {
				// Copies the byte in the array and changes the specific bit to 0.
				// By doing bitwise and
				display[x+row*128] = display[x+row*128] & ~power(2, rest_y); 	// Turn on all pixels within x1,y1,x2,y2
			}
		}
	}
	display_image(display);
}

// print_dotted_line prints dotted line starting from top and cuts of last dot att bottom of screen.
// Uses screen width to get center and dot_w, dot_h and dot_s, to get the right dot size and placement
void print_dotted_line() {
	// Loop through all dots depending on dot_h, dot_s and display_h
    for(int y=0; y<display_h; y += dot_h+dot_s) {
		// Print out dots on center line
        if(y < (display_h-dot_h)) {
			// Print dot if it wont get cut of by the display edge
            print_solid(1, (display_w/2)-(dot_w/2), y, (display_w/2)+(dot_w/2)-1, y+dot_h-1);
        }
		// Print part of the dot if it will get cut of by the display edge
        else {
            print_solid(1, (display_w/2)-(dot_w/2), y, (display_w/2)+(dot_w/2)-1, display_h-1);
        }
    }
}

// move_ball
// Erases the last ball positon and the reprints the ball in the new position
// Argumetns:
// 	int new_x: int between 0 and display_w-ball_s-1
//	int new_y: int between 0 and display_h-ball_s-1
//		new_x,new_y is the upper left corner of the ball
//	int old_x: int between 0 and display_w-ball_s-1
//	int old_y: int between 0 and display_h-ball_s-1
//		old_x,old_y is the upper left corner of the ball
void move_ball(int new_x, int new_y, int old_x, int old_y) {
    // Erase ball by using old_x, old_y and ball_s
    print_solid(0, old_x, old_y, old_x+ball_s-1, old_y+ball_s-1);
    // Draw ball by using new_x, new_y and ball_s
    print_solid(1, new_x, new_y, new_x+ball_s-1, new_y+ball_s-1);
}

// move_paddle
// Erases the paddles 
// Arguments:
// 	int paddle_sel:	0 (left paddle) or 1 (right paddle)
//	int new_pos: int between 0 and display_h-paddle_h-1 (height of paddle from the top)
void move_paddle(int paddle_sel, int new_pos) {
    // Erase all pixels where paddle can be
    print_solid(0, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2), 0, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2)+paddle_w-1, display_h-1);
    // Draw paddle depending on new_pos
    print_solid(1, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2), new_pos, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2)+paddle_w-1, new_pos+paddle_h-1);
}