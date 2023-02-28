#include <stdio.h>
#include <stdlib.h>
#include <pic32mx.h>
#include "header.h"

// Global varibles/settings varibles:
/*---===---===---===---===---===---===---===---===---===---*/
// Display
int display_w = 128;    // display width = pixels
int display_h = 32;     // display height = pixels
// Ball
int ball_s = 2;         // ball size = pixels (it is a rectangle NOT ball(rounded rectangle))
double ball_v = 1;    // ball speed
// Player paddle
int paddle_w = 3;       // paddle width = pixels
int paddle_h = 10;      // paddle height = pixels
int paddle_s = 8;       // paddle spacing to edge = pixels
int paddle_v = 1;       // paddle move speed
// Center dot/dot line
int dot_w = 1;          // dot width = pixels (must be even)
int dot_h = 2;          // dot height = pixels
int dot_s = 5;          // dot spacing to edge = pixels
/*---===---===---===---===---===---===---===---===---===---*/

// Structs
typedef struct Ball {
    int w,h;
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

// Field variables
static ball_t ball;
static paddle_t paddles[2];
static controls_t controls;

int quit = 0;

//Functions
void initialize() {         //Set necessary values and display items
    display_clear();
    ball.w = ball_s;
    ball.h = ball_s;
    ball.x = display_w/2;
    ball.y = display_h/2;
    ball.dx = ball_v;
    ball.dy = ball_v/3;

    move_ball((int)(ball.x),(int)(ball.y),0,0);


    for (int i = 0; i < 2; i++) {
        paddles[i].y = (display_h/2-paddle_h/2);
        move_paddle(i, paddles[i].y);
    }

    print_dotted_line();
    display_image(0, display);
}

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

void intialize_controls() {
    PORTFSET = 0x2;
    PORTDSET = 0xE0;
}

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

void game() {
    print_start_screen();
    while (true) {      // Wait for button press
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
        sleep(10); // limits program to update once every 30 ms
    }
    print_end_screen(quit);
    sleep(1000);
    while (true) {      // Wait for button press
        if (check_controls()) {
            quit = 0;
            game();
        }
    }
}

int main() {
    display_main();
    display_init();
    intialize_controls();
    game();
    return 0;
}