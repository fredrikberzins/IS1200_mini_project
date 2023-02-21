#include <stdio.h>
#include <stdlib.h>
#include <pic32mx.h>
#include <stdbool.h>
#include <math.h>

// Display related declarations:
/*---===---===---===---===---===---===---===---===---===---*/
// Font:
// Declare bitmap for font
extern const uint8_t const font[128*8];
// Declare text buffer
extern char textbuffer[4][16];

// Global varibles/settings import from main
/*---===---===---===---===---===---===---===---===---===---*/
// Display
extern int display_w;      // display width = pixels
extern int display_h;      // display height = pixels
// Ball
extern int ball_s;         // ball size = pixels (it is a rectangle NOT ROUNDED)
extern int ball_v;         // ball speed
// Player paddle
extern int paddle_w;       // paddle width = pixels
extern int paddle_h;       // paddle height = pixels
extern int paddle_s;       // paddle spacing to edge = pixels
extern int paddle_v;       // paddle move speed
// Center dot/dot line
extern int dot_w;          // dot width = pixels (must be even)
extern int dot_h;          // dot height = pixels
extern int dot_s;          // dot spacing to edge = pixels
// Score
extern int player1;        // score for player 1
extern int player2;        // score for player 2
extern int limit;          // score required to win(max 10, other wise the score printing will fail), i.e first to x wins
/*---===---===---===---===---===---===---===---===---===---*/

// Display mode:
// Command/data mode
#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)
// Display reset/not reset
#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)
// Power controll
#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)
#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

// Functions:
// Forward declartion of all disply functions
uint8_t spi_send_recv(uint8_t);
void display_init();
// Clear/Update
void display_clear();
void display_update();
// Splash screens start/end
void print_start_screen();
void print_end_screen(int);
// Game related:
// print string/score
void print_string(int, char *);
void print_score(int ,int);
// print solid/center line
void print_solid(bool, int, int, int, int);
void print_dotted_line();
// Move ball/paddle
void move_ball(int, int, int, int);
void move_paddle(int, int);
/*---===---===---===---===---===---===---===---===---===---*/

// SPI send and recive
uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

// Initiate display
void display_init() {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
	sleep(10);
	DISPLAY_ACTIVATE_VDD;
	sleep(1000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	sleep(10);
	DISPLAY_DO_NOT_RESET;
	sleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	sleep(1000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
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
void display_clear() {
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	sleep(10);
	DISPLAY_DO_NOT_RESET;
	display_update();
}

// Update display
void display_update() {
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
				spi_send_recv(font[c*8+k]);
		}
	}
}

// Draw Start screan
void print_start_screen() {
	display_clear();
	print_string( 0, " Pong");
	print_string( 1, " by");
	print_string( 2, " Felix &");
	print_string( 3, " Fredrik" );
    display_update();
}

// Draw end screan
void print_end_screen(int player) {
	display_clear();
	//" Player "+ player + " won";
	//" Ending score [" + player1 + ":" + player2 + "]";
	print_string( 0, "PLayer");
	print_string( 1, (char*)&player+30);
	print_string( 2, "won");
	print_string( 3, " Pong by Felix & Fredrik");
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

// print_solid color(false black, true white), x1,y1(lower-left point), x2,y2(upper-right point).
void print_solid(bool white, int x1, int y1, int x2, int y2) {
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
    // Clears a part of the screan (black)
    if (!white) {
        // Erase function
		
        // Reprint doted center line if it is erased
        if(x1 <= ((display_w/2)+(dot_w/2)) && x2 >= ((display_w/2)-(dot_w/2))) {
            print_dotted_line();
        }
		// Reprint score if it is erased
        if(y2 >= display_h-1-7 && x1 <= ((display_w/2)+(dot_w/2))+2+4  && x2 >= ((display_w/2)-(dot_w/2)-1-2-4)) {
            print_score(false, player1);
            print_score(true, player2);
        }
    }
	// Draw function
    else {
        // Draw function
    }
    display_update();
}

// print_dotted_line prints dotted line startingfrom bottom and cuts of last dot att top of screan.
void print_dotted_line() {
    for(int y=0; y<display_h; y+= (dot_h+dot_s)) {
		// print out dots on center line
        if(y < (display_h-dot_h)) {
            print_solid(true, (display_w/2)-(dot_w/2)-1, y, (display_w/2)+(dot_w/2), y+dot_h-1);
        }
		// print out last dot on center line
        else {
            print_solid(true, (display_w/2)-(dot_w/2)-1, y, (display_w/2)+(dot_w/2), display_h-1);
        }
    }
}

// move_ball clears old location of ball and prints new position for ball.
// print_solid new_x,new_y(lower-left pixel of new position of ball)
// old_x,old_y(lower-left pixel of old position of ball).
void move_ball(int new_x, int new_y, int old_x, int old_y) {
    // Erase ball
    print_solid(false, old_x, old_y, old_x+ball_s, old_y+ball_s);
    // Draw ball
    print_solid(true, new_x, new_y, new_x+ball_s, new_y+ball_s);
}

// move_paddle clears old location of paddle and prints new position for paddle.
// paddle_sel(0 left, 1 right), new_pos(height of lower pixel of paddle).
void move_paddle(int paddle_sel, int new_pos) {
    // Erase paddle
    print_solid(false, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2), 0, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2)+paddle_w, display_h-1);
    // Draw paddle
    print_solid(true, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2), new_pos, paddle_s+paddle_sel*(display_w-paddle_w-paddle_s*2)+paddle_w, new_pos+paddle_h);
}