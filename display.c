/* Written by Felix Bergqvist Widström and Fredrik Berzins (2023) */

#include <stdio.h>
#include <stdlib.h>
#include <pic32mx.h>
#include <stdbool.h>
#include "header.h"

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
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

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 128; j++)
			spi_send_recv(~data[i*128 + j]);
	}
}

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
	for(int i = 0; i < 512; i++) {
		display[i] = 0xff;
	}
	display_image(0, display);
	for(int i = 0; i < 4; i++) {
		print_string( i, "");
	}
	display_update();
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
	if(player == 1){
		print_string( 0, " PLAYER NR:1");
	}
	else{
		print_string( 0, " PLAYER NR:2");
	}
	print_string( 1, " WON PONG");
    display_update();
}

// print string with font from mipslabdata.c
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
			if (color == 0) {
				// Erase
				display[x+row*128] = display[x+row*128] | power(2, rest_y); // turn off all pixels within x1,y1,x2,y2
			} else {
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
            print_solid(1, (display_w/2)-(dot_w/2), y, (display_w/2)+(dot_w/2)-1, y+dot_h-1);
        }
		// print out last dot on center line
        else {
            print_solid(1, (display_w/2)-(dot_w/2), y, (display_w/2)+(dot_w/2)-1, display_h-1);
        }
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