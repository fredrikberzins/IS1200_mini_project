/* Written by Felix Bergqvist Widström and Fredrik Berzins (2023) */

#include "header.h"

/* === Global varibles/settings varibles === */
/*---===---===---===---===---===---===---===---===---===---*/
// Display
int display_w = 128;    // Display width = pixels
int display_h = 32;     // Display height = pixels
// Ball
int ball_s = 2;         // Ball size = pixels (it is a rectangle NOT ball(rounded rectangle))
double ball_v = 1;      // Ball speed
// Player paddle
int paddle_w = 3;       // Paddle width = pixels
int paddle_h = 10;      // Paddle height = pixels
int paddle_s = 8;       // Paddle spacing to edge = pixels
int paddle_v = 1;       // Paddle move speed
// Center dot/dot line
int dot_w = 1;          // Dot width = pixels (must be even)
int dot_h = 2;          // Dot height = pixels
int dot_s = 5;          // Dot spacing to edge = pixels
/*---===---===---===---===---===---===---===---===---===---*/

/* === Structs === */
typedef struct Ball {
    double x,y;
    double dx,dy;
} ball_t;

typedef struct Paddle {
    int y;
} paddle_t;

typedef struct Controls {
    int up1, down1;
    int up2, down2;
} controls_t;

/* === Field variables === */
static ball_t ball;
static paddle_t paddles[2];
static controls_t controls;

int quit = 0;

/* === Functions === */
// Set necessary values and display necessary items
void initialize() {
    display_clear();
    ball.x = display_w/2-ball_s/2;
    ball.y = (rand()%((display_h/2)-ball_s)+(display_h/4));
    ball.dx = ball_v;
    ball.dy = ball_v/(((rand()%22) / 10) + 1.8);

    move_ball((int)(ball.x),(int)(ball.y),0,0);

    for (int i = 0; i < 2; i++) {
        paddles[i].y = (display_h/2-paddle_h/2);
        move_paddle(i, paddles[i].y);
    }

    print_dotted_line();
}

// Checks for inputs and sets values for movement (LEDs for debug)
void update_controls() {
    if (PORTF & 0x2) {      // 0000 0010 (PORTF)    BTN1
        controls.up2 = 1;
        PORTESET = 0x1;
    } else {
        controls.up2 = 0;
        PORTECLR = 0x1;
    }

    if (PORTD & 0x20) {     // 0010 0000 (PORTD)    BTN2
        controls.down2 = 1;
        PORTESET = 0x2;
    } else {
        controls.down2 = 0;
        PORTECLR = 0x2;
    }

    if (PORTD & 0x40) {     // 0100 0000 (PORTD)    BTN3
        controls.down1 = 1;
        PORTESET = 0x4;
    } else {
        controls.down1 = 0;
        PORTECLR = 0x4;
    }

    if (PORTD & 0x80) {     // 1000 0000 (PORTD)    BTN4
        controls.up1 = 1;
        PORTESET = 0x8;
    } else {
        controls.up1 = 0;
        PORTECLR = 0x8;
    }
}

// Checks if a button is pressed and returns true or false
bool check_controls() {
    if (PORTF & 0x2) {
        return true;
    }
    if (PORTD & 0x20) {
        return true;
    }

    if (PORTD & 0x40) {
        return true;
    }

    if (PORTD & 0x80) {
        return true;
    }
    return false;
}

// Sets pins as inputs or outputs for buttons and LEDs
void intialize_controls() {
    TRISECLR = 0xFF;        // Sets LEDs as outputs
    PORTFSET = 0x02;        // Sets button 1 as input
    PORTDSET = 0xE0;        // Sets button 2,3,4 as inputs
}

// Updates position values for paddles depending on currently active controls
void move() {
    if (controls.up1 == 1 && controls.down1 != 1) {
        paddles[0].y -= paddle_v;
        if (paddles[0].y <= 0){
            paddles[0].y = 0;
        } else {
            move_paddle(0, (paddles[0].y));    
        }
    }
    if (controls.down1 == 1 && controls.up1 != 1) {
        paddles[0].y += paddle_v;
        if (paddles[0].y + paddle_h >= (display_h)) {
            paddles[0].y = (display_h-paddle_h);
        } else {
            move_paddle(0, (paddles[0].y));    
        }
    }
    if (controls.up2 == 1 && controls.down2 != 1) {
        paddles[1].y -= paddle_v;
        if (paddles[1].y <= 0){
            paddles[1].y = 0;
        } else {
            move_paddle(1, (paddles[1].y));    
        }
    }
    if (controls.down2 == 1 && controls.up2 != 1) {
        paddles[1].y += paddle_v;
        if (paddles[1].y + paddle_h >= (display_h)) {
            paddles[1].y = (display_h-paddle_h);
        } else {
            move_paddle(1, (paddles[1].y));    
        }
    }
}

// Changes direction of the ball on collision with paddles and the 
// Top/bottom of the screen or ends the game if the ball misses a paddle
void check_collision() {
    if ((int)(ball.x) + ball_s >= display_w-1) {
        quit = 1;
    }
    if ((int)(ball.x) <= 0) {
        quit = 2;
    }
    if ((int)(ball.y) + ball_s-1 >= display_h - 1 || (int)(ball.y) <= 0) {
        ball.dy = -ball.dy;
    }
    for (int i = 0; i < 2; i++) {
        if((int)(ball.x) + i*ball_s == (paddle_s + (paddle_w)) + i*(display_w-(paddle_s*2)-(paddle_w*2))) {
            if((int)(ball.y) <= (paddles[i].y + paddle_h - 1) && ((int)(ball.y) + ball_s - 1) >= paddles[i].y) {
                ball.dx = -ball.dx;
            }
        }
    }
}

// Game loop for restarting on win/loss
void game() {
    print_start_screen();
    while (true) {          // Wait for button press
        if (check_controls()) {
            break;
        }
    }
    initialize();
    sleep(500);
    while (quit == 0) {
        update_controls();
        move();
        print_dotted_line((int)(ball.x));
        move_ball((int)(ball.x + ball.dx), (int)(ball.y + ball.dy), (int)(ball.x), (int)(ball.y));
        ball.x += ball.dx;
        ball.y += ball.dy;
        check_collision();
        sleep(10);          // Limits program to update once every 10 ms (100 fps)
    }
    print_end_screen(quit);
    sleep(500);
    while (true) {          // Wait for button press
        if (check_controls()) {
            quit = 0;
            game();
        }
    }
}

int main() {
    display_init();
    intialize_controls();
    game();
    return 0;
}