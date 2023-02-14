#include <stdio.h>
#include <stdlib.h>
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

void initialize() {
    //Set necessary values
    ball.w = ball_s;
    ball.h = ball_s;
    ball.x = display_w/2;
    ball.y = display_h/2;
    ball.dx = ball_v;
    ball.dy = ball_v;

    for (int i = 0; i < 2; i++) {
        paddles[i].w = paddle_w;
        paddles[i].h = paddle_h;
        move_paddle(i, (display_h-paddle_h)/2);
    }

    print_dotted_line();
}

void checkCollision() {
    if (ball.x + ball_s >= display_w) {
        player1++;
        //Reset ball to middle
    }
    if (ball.x <= 0) {
        player2++;
        //Reset ball to middle
    }
    if (ball.y + ball_s >= display_h || ball.y <= 0) {
        ball.dy = -ball.dy;
    }
    for (int i = 0; i < 2; i++) {
        if (ball.y + (ball_s/2) >= paddles[i].y && ball.y + (ball_s/2) <= paddles[i].y + paddle_h && ball.x + (ball_s/2) == paddles[i].x) {
            ball.dx = -ball.dx;
        }
    }
}
void checkScore() {
    if (player1 == limit) {
        // player 1 wins       
    }
    if (player2 == limit) {
        // player 2 wins       
    }
}

int main() {
    int quit = 0;
    while (quit == 0) {
        //initialize();
        //display start screen
        //start when one player pushes a button
        //move paddles
        //move ball
        //checkCollision();
        //check score
        //display end screen
        sleep(0.05); // limits program to 20 updates per second => 20fps
    }

    return 0;
}