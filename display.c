#include <stdio.h>
#include <stdlib.h>
#include <pic32mx.h>
#include <stdbool.h>
#include "header.h"

void display_main() {
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
}

// Clears display
void display_clear(){
	for (int i = 0; i < 512; i++) {
		display[i] = 0xff;
	}
	display_image(0, display);
}

// Draw Start screan
void print_start_screen() {
	display_clear();
	print_string( 0, " PONG");
	print_string( 1, " BY");
	print_string( 2, " FELIX &");
	print_string( 3, " FREDRIK");
    display_update();
}

// Draw end screan
void print_end_screen(int player) {
	display_clear();
	//" Player "+ player + " won";
	//" Ending score [" + player1 + ":" + player2 + "]";
	char row_0 = " PLAYER " + (player+0x30);
	char row_3 = " " + (player1+0x30) + " : " + (player2+0x30);
	print_string( 0, row_0);
	print_string( 1, "WON PONG");
	print_string( 2, " SCORE");
	print_string( 3, row_3);
    display_update();
}

// print string with font from font_data.c
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

// render_points clears old score and prints new score value for that side.
// player(0 left, 1 right), point(amount of points to add to score).
void print_score(int player_sel ,int point) {
    int score_pos = ((display_w/2)-(dot_w/2))-2-4+player_sel*(dot_w+2+4*2);
    // Erase old score
    print_solid(false, score_pos, display_h-1-7, score_pos+4, display_h-1);
    // Print out new score

    display_update();
}

// print_solid color(0 black, anything else white), x1,y1(lower-left point), x2,y2(upper-right point).
void print_solid(int color, int x1, int y1, int x2, int y2) {
    // Checks so x1 is smaller then x2
	// if not swap them
    if (x1 > x2) {      
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    // Checks so y1 is smaller then y2
	// if not swap them
    if (y1 > y2) {      
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }
    // loops thru all pixels in specified area.

	for (int x = x1; x <= x2; x++) {
		for(int y = y1; y <= y2; y++) {
			int row = lower_8(y);
			int rest_y = y%8;
			PORTESET = 0x80;
			if (color == 0) {
				PORTESET = 0x40;
				// Erase
				display[x+row*128] = display[x+row*128] | ~power(2, rest_y); // turn off all pixels within x1,y1,x2,y2
			}
			else {
				PORTESET = 0x20;
				// Print
				display[x+row*128] = display[x+row*128] & ~power(2, rest_y); // turn on all pixels within x1,y1,x2,y2
			}
		}
	}
	PORTECLR=0xE0;
	display_image(0, display);
}

// print_dotted_line prints dotted line startingfrom bottom and cuts of last dot att top of screan.
void print_dotted_line() {
    for(int y=0; y<display_h; y += dot_h+dot_s) {
		// print out dots on center line
        if(y < (display_h-dot_h)) {
            print_solid(1, (display_w/2)-(dot_w/2)-1, y, (display_w/2)+(dot_w/2)-1, y+dot_h-1);
        }
		// print out last dot on center line
        else {
            print_solid(1, (display_w/2)-(dot_w/2)-1, y, (display_w/2)+(dot_w/2)-1, display_h-1);
        }
    }
}

void reprint_dotted_line_score(int x) {
	if (x < ((display_w/2)-(dot_w/2)-1) || x > ((display_w/2)+(dot_w/2))) {
		return;
	}
	else {
		print_dotted_line();
	}
}

// move_ball clears old location of ball and prints new position for ball.
// print_solid new_x,new_y(lower-left pixel of new position of ball)
// old_x,old_y(lower-left pixel of old position of ball).
void move_ball(int new_x, int new_y, int old_x, int old_y) {
    // Erase ball
    print_solid(0, old_x, old_y, old_x+ball_s-1, old_y+ball_s-1);
    // Draw ball
    print_solid(1, new_x, new_y, new_x+ball_s-1, new_y+ball_s-1);
}

// move_paddle clears old location of paddle and prints new position for paddle.
// paddle_sel(0 left, 1 right), new_pos(height of lower pixel of paddle).
void move_paddle(int paddle_sel, int new_pos) {
    // Erase paddle
    print_solid(0, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2), 0, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2)+paddle_w-1, display_h-1);
    // Draw paddle
    print_solid(1, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2), new_pos, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2)+paddle_w-1, new_pos+paddle_h-1);
}