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
    display_clear();                                            // Clear screen

    ball.x = display_w/2-ball_s/2;                              // Sets starting position of ball
    ball.y = (rand()%((display_h/2)-ball_s)+(display_h/4));     
    ball.dx = ball_v;                                           // Sets ball direction dx/dy
    ball.dy = ball_v/(((rand()%22) / 10) + 1.8);

    move_ball((int)(ball.x),(int)(ball.y),0,0);                 // Prints ball on starting position

    for (int i = 0; i < 2; i++) {                               // Sets and prints starting position of paddles
        paddles[i].y = (display_h/2-paddle_h/2);
        move_paddle(i, paddles[i].y);
    }

    print_dotted_line();                                        // Prints the dotted line in the middle of the screen
}

// Checks for inputs and sets values for movement (LEDs for debug)
void update_controls() {
    if (PORTF & 0x2) {      // 0000 0010 (PORTF) BTN 1
        controls.up2 = 1;
        PORTESET = 0x1;     // Activate LED 1
    } else {
        controls.up2 = 0;
        PORTECLR = 0x1;     // Deactivate LED 1
    }

    if (PORTD & 0x20) {     // 0010 0000 (PORTD) BTN 2
        controls.down2 = 1;
        PORTESET = 0x2;     // Activate LED 2
    } else {
        controls.down2 = 0;
        PORTECLR = 0x2;     // Deactivate LED 2
    }

    if (PORTD & 0x40) {     // 0100 0000 (PORTD) BTN 3
        controls.down1 = 1;
        PORTESET = 0x4;     // Activate LED 3
    } else {
        controls.down1 = 0;
        PORTECLR = 0x4;     // Deactivate LED 3
    }

    if (PORTD & 0x80) {     // 1000 0000 (PORTD) BTN 4
        controls.up1 = 1;
        PORTESET = 0x8;     // Activate LED 4
    } else {
        controls.up1 = 0;
        PORTECLR = 0x8;     // Deactivate LED 4
    }
}

// Checks if a button is pressed and returns true or false, used for waiting for button presses
bool check_controls() {
    if (PORTF & 0x2) {      // 0000 0010 (PORTF) BTN 1
        return true;
    }
    if (PORTD & 0x20) {     // 0010 0000 (PORTD) BTN 2
        return true;
    }
    if (PORTD & 0x40) {     // 0100 0000 (PORTD) BTN 3
        return true;
    }
    if (PORTD & 0x80) {     // 1000 0000 (PORTD) BTN 4
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
// Also limits movement so that the paddles can't go outside the screen and 
// disables movement if both up and down are active
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
// top/bottom of the screen or ends the game if the ball misses a paddle
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
    initialize();                           // Initializes game starting point
    sleep(500);
    while (true) {                          // Wait for button press
        if (check_controls()) {
            break;
        }
    }
    while (quit == 0) {
        update_controls();                  // Update control values
        move();                             // Update position of paddles
        print_dotted_line((int)(ball.x));   // Print dotted line to avoid overwrites
        move_ball((int)(ball.x + ball.dx), (int)(ball.y + ball.dy), (int)(ball.x), (int)(ball.y));
        ball.x += ball.dx;
        ball.y += ball.dy;
        check_collision();                  // Handles collision of ball
        sleep(10);                          // Limits the program to update once every 10 ms (100 fps)
    }
    print_end_screen(quit);                 // Prints win/loss of player
    sleep(500);
    while (true) {                          // Wait for button press
        if (check_controls()) {
            quit = 0;
            game();
        }
    }
}

int main() {
    display_init();                 // Initializes display
    intialize_controls();           // Enables controls
    print_start_screen();           // Prints start screen
    sleep(1000);
    game();                         // Enter game-loop
    return 0;
}