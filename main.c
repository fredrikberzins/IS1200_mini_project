#include <stdio.h>
#include <stdlib.h>
#include <pic32mx.h>
#include "settings.c"
#include "display.c"

typedef struct Ball {
    int w,h;
    int x,y;
    int dx,dy;
} ball_t;

typedef struct Paddle {
    int w,h;
    int x,y;
} paddle_t;

typedef struct Controls {
    int up1, down1;
    int up2, down2;
} controls_t;

static ball_t ball;
static paddle_t paddles[2];
static controls_t controls;

void initialize() {     //Set necessary values
    ball.w = ball_s;
    ball.h = ball_s;
    ball.x = display_w/2;
    ball.y = display_h/2;
    ball.dx = ball_v;
    ball.dy = ball_v/(rand()%3 + 1);

    for (int i = 0; i < 2; i++) {
        paddles[i].w = paddle_w;
        paddles[i].h = paddle_h;
        move_paddle(i, (display_h-paddle_h)/2);
    }

    print_dotted_line();
}

void reset() {      //Reset everything to starting position and wait 3 sec
    ball.x = display_w/2;
    ball.y = display_h/2;
    ball.dy = ball_v/(rand()%3 + 1);
    for (int i = 0; i < 2; i++) {
        move_paddle(i, (display_h-paddle_h)/2);
    }
    sleep(3);
}

void check_controls() {
    if (PORTF & 0x1) {      // 0000 0001 (PORTF)    BTN1
        controls.up2 = 1;
    } else {
        controls.up2 = 0;
    }

    if (PORTE & 0x20) {     // 0010 0000 (PORTE)    BTN2
        controls.down2 = 1;
    } else {
        controls.down2 = 0;
    }

    if (PORTE & 0x40) {     // 0100 0000 (PORTE)    BTN3
        controls.down1 = 1;
    } else {
        controls.down1 = 0;
    }

    if (PORTE & 0x80) {     // 1000 0000 (PORTE)    BTN4
        controls.up1 = 1;
    } else {
        controls.up1 = 0;
    }
}

void move() {
    if (controls.up1 == 1 && controls.down1 != 1) {
        move_paddle(0, paddles[0].y + paddle_v);
    }
    if (controls.up2 == 1 && controls.down2 != 1) {
        move_paddle(1, paddles[1].y + paddle_v);
    }
    if (controls.down1 == 1 && controls.up1 != 1) {
        move_paddle(0, paddles[0].y - paddle_v);
    }
    if (controls.down2 == 1 && controls.up2 != 1) {
        move_paddle(1, paddles[1].y - paddle_v);
    }
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
        && ball.x + (ball_s/2) == paddles[i].x + (paddle_w/2)) {
            ball.dx = -ball.dx;
        }
    }
}
void check_score() {
    if (player1 == limit) {
        // player 1 wins       
    }
    if (player2 == limit) {
        // player 2 wins       
    }
}

int main() {
    initialize();
    //display start screen
    //start when one player pushes a button
    int quit = 0;
    while (quit == 0) {
        check_controls();
        move();
        move_ball(ball.x + ball.dx, ball.y + ball.dy, ball.x, ball.y);
        check_collision();
        check_score();
        sleep(0.03); // limits program to update once every 3 ms
    }
    //display end screen
    return 0;
}