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
    int 
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

int main() {
    int quit = 0;
    while (quit == 0) {
        //display start screen
        //start when one player pushes a button
        //move ball
        //move paddles
        //check collisions
        //check score
        //display end screen
    }

    return 0;
}