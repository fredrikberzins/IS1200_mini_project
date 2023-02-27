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
int ball_s = 2;         // ball size = pixels (it is a rectangle NOT ROUNDED)
int ball_v = 1;         // ball speed
// Player paddle
int paddle_w = 3;       // paddle width = pixels
int paddle_h = 8;      // paddle height = pixels
int paddle_s = 6;       // paddle spacing to edge = pixels
int paddle_v = 1;       // paddle move speed
// Center dot/dot line
int dot_w = 2;          // dot width = pixels (must be even)
int dot_h = 2;          // dot height = pixels
int dot_s = 7;          // dot spacing to edge = pixels
// Score
int player1 = 0;        // score for player 1
int player2 = 0;        // score for player 2
int limit = 1;          // score required to win(max 10, other wise the score printing will fail), i.e first to x wins
/*---===---===---===---===---===---===---===---===---===---*/

// Structs
typedef struct Ball {
    int w,h;
    int x,y;
    int dx,dy;
} ball_t;

typedef struct Paddle {
    int w,h;
    int y;
} paddle_t;

typedef struct Controls {
    int up1, down1;
    int up2, down2;
} controls_t;

static ball_t ball;
static paddle_t paddles[2];
static controls_t controls;

int quit = 0;

void initialize() {         //Set necessary values and display items
    display_clear();
    ball.w = ball_s;
    ball.h = ball_s;
    ball.x = display_w/2;
    ball.y = display_h/2;
    ball.dx = ball_v;
    ball.dy = ball_v/2;
    // ball.dy = ball_v/(rand()%3 + 1);

    move_ball(ball.x,ball.y,0,0);


    for (int i = 0; i < 2; i++) {
        paddles[i].w = paddle_w;
        paddles[i].h = paddle_h;
        paddles[i].y = display_h/2;
        move_paddle(i, paddles[i].y);
    }

    print_dotted_line();
    display_image(0, display);
}

void reset() {              //Reset everything to starting position and wait 3 sec
    ball.x = display_w/2;
    ball.y = display_h/2;
    ball.dy = ball_v/2;
    //ball.dy = ball_v/( rand() % 3 + 1);
    for (int i = 0; i < 2; i++) {
        move_paddle(i, (display_h-paddle_h)/2);
    }
    sleep(3000);
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
    if (controls.up1 == 1) {
        paddles[0].y -= paddle_v;
        move_paddle(0, (paddles[0].y));
    }
    else if (controls.down1 == 1) {
        paddles[0].y += paddle_v;
        move_paddle(0, (paddles[0].y));
    }
    if (controls.up2 == 1) {
        paddles[1].y -= paddle_v;
        move_paddle(1, (paddles[1].y));
    }
    else if (controls.down2 == 1) {
        paddles[1].y += paddle_v;
        move_paddle(1, (paddles[1].y));
    }
    if (paddles[0].y <= 1){paddles[0].y = 1;}
    if (paddles[0].y >= (display_h-paddle_h-1)){paddles[0].y = (display_h-paddle_h-1);}
    if (paddles[1].y <= 1){paddles[1].y = 1;}
    if (paddles[1].y >= (display_h-paddle_h-1)){paddles[1].y = (display_h-paddle_h-1);}
}

void check_collision() {
    if (ball.x + ball_s >= display_w-1) {
        player1++;
        reset();
    }
    if (ball.x <= 0) {
        player2++;
        reset();
    }
    if (ball.y + ball_s >= display_h || ball.y <= 0) {
        ball.dy = -ball.dy;
    }
    for (int i = 0; i < 2; i++) {
        if (ball.y + (ball_s/2) >= paddles[i].y && ball.y + (ball_s/2) <= paddles[i].y + paddle_h 
        && ball.x + (ball_s/2) == (paddle_s + (paddle_w)) + i*(display_w-(paddle_s*2)-(paddle_w*2))) {
            ball.dx = -ball.dx;
        }
    }
}

void check_score() {
    if (player1 == limit) {
        // player 1 wins   
        quit = 1;   
    }
    else if (player2 == limit) {
        // player 2 wins 
        quit = 2;     
    }
}

int main() {
    display_main();
    display_init();
    print_start_screen();
    intialize_controls();
    while (true) {      // Wait for button press
        if (check_controls()) {
            break;
        }
    }
    initialize();
    sleep(1000);
    while (quit == 0) {
        update_controls();
        move();
        move_ball(ball.x + ball.dx, ball.y + ball.dy, ball.x, ball.y);
        reprint_dotted_line_score(ball.x);
        ball.x += ball.dx;
        ball.y += ball.dy;
        check_collision();
        check_score();
        sleep(30); // limits program to update once every 30 ms
    }
    print_end_screen(quit);
    return 0;
}