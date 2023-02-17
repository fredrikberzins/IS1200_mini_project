#include <stdbool.h>
#include <math.h>

// Display related declarations:
/*---===---===---===---===---===---===---===---===---===---*/
// Font:
// Declare bitmap for font
extern const uint8_t const font[128*8];
// Declare text buffer
extern char textbuffer[4][16];

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
uint8_t spi_send_recv(uint8_t data),
void display_init();
// Clear/Update
void display_clear();
void display_update();
// Splash screens start/end
void print_start_screen();
void print_end_screen(int player);
// Game related:
// print string/score
void print_string(int line, char *str);
void print_score(int player_sel ,int point);
// print solid/center line
void print_solid(bool white, int x1, int y1, int x2, int y2);
void print_dotted_line();
// Move ball/paddle
void move_ball(int new_x, int new_y, int old_x, int old_y);
void move_paddle(int paddle_sel, int new_pos);
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
	sleep(0.01);
	DISPLAY_ACTIVATE_VDD;
	sleep(1);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	sleep(0.01);
	DISPLAY_DO_NOT_RESET;
	sleep(0.01);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	sleep(1);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}

// Clears display
void display_clear() {
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	sleep(0.01);
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
	display_string( 0, " Pong");
	display_string( 1, " by");
	display_string( 2, " Felix &");
	display_string( 3, " Fredrik" );
    display_update();
}

// Draw end screan
void print_end_screen(int player) {
	display_clear();
	display_string( 0, " Player %d win", player);
	display_string( 1, " ending score [%d:%d]", player1, player2);
	display_string( 3, " Pong by Felix & Fredrik");
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