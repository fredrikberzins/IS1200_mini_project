#include <stdio.h>
#include <stdlib.h>
#include <settings.c>

typedef struct Ball {
    int w,h;
    int x,y;
    int dx,dy;
} ball_t;

typedef struct Paddle {
    int w,h;
    int x,y;
} paddle_t;

static ball_t ball;
static paddle_t paddles[2];

void initialize() {
    ball.w = ball_size;
    ball.h = ball_size;
    //ball.x = screen_width/2
    //ball.y = screen_height/2
    ball.dx = 1;
    ball.dy = 1;

    for (int i = 0; i < 2; i++) {
        paddles[i].w = paddle_w;
        paddles[i].h = paddle_h;
    }
}

void move_ball() {
    //ball.dx += ball_speed;
    //ball.dy += ball_speed;
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