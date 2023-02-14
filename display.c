// Display translation file for chipKIT Uno32 board with a I2C screen
// x=0 and y=0 is in the lower left corner
// Pong specilaced global functions
//     print_dotted_line
//     move_paddle
//     move_ball
//     render_score
// Pong private functions
//     print_solid
//     print_score
#include <stdbool.h>
#include <math.h>
#include <settings.c>

// print_dotted_line prints dotted line startingfrom bottom and cuts of last dot att top of screan.
void print_dotted_line() {
    // Erase doted center line
    Erase ((display_w/2-dot_w/2)-1, (display_w/2+dot_w/2)-1, 0, display_h-1);
    for(int y=0; y<display_h; y += (dot_h + dot_s)) {
        if(y < (display_h-dot_h)) {  //
            print_solid(true, display_w/2-dot_w/2, y, display_w/2+dot_w/2, y+dot_h-1);
        } else() {
            print_solid(true, display_w/2-dot_w/2, y, display_w/2+dot_w/2, display_w-1);
        }
    }
}

// print_solid color(0 black, 1 white), x1,y1(lower-left point), x2,y2(upper-right point).
void print_solid(bool color, int x1, int y1, int x2, int y2) {
    // checks so x1 is smaller then x2
    if (x1 > x2) {      
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }

    // checks so y1 is smaller then y2
    if (y1 > y2) {      
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    // Clears a part of the screan (black)
    if (!color) {
        // Erase
        Erase(x1, y1, x2, y2);
        if(x1 <= display_w/2+dot_w/2 && x2 >= display_w/2-dot_w/2) {
            // Reprint doted center line
            print_dotted_line();
        }
    } 
    else () {
        // Draw
        Draw(x1, y1, x2, y2);
    }
    Update ();

    // Prints on part of the screan (white)
    else() {
        ### // print white over the whole area
    }

    // Refresh screan
    ### // update screan
}

// move_paddle clears old location of paddle and prints new position for paddle.
// paddle_sel(0 left, 1 right), new_pos(height of lower pixel of paddle).
void move_paddle(int paddle_sel, int new_pos) {
    // Erase paddle
    Erase (paddle_s+paddle_sel*(display_w-paddle_w-paddle_s), 0, paddle_s+paddle_w, display_h-1);
    // Draw paddle
    Draw (paddle_s+paddle_sel*(display_w-paddle_w-paddle_s), new_pos, paddle_s+paddle_w, new_pos+paddle_h);
    Update ();
}

// move_ball clears old location of ball and prints new position for ball.
// print_solid new_x,new_y(lower-left pixel of new position of ball)
// old_x,old_y(lower-left pixel of old position of ball).
void move_ball(int new_x, int new_y, int old_x, int old_y) {
    // Erase ball
    Erase (old_x, old_y, old_x+ball_s, old_y+ball_s);
    print_solid(bool color, int x1, int y1, int x2, int y2)
    // Draw ball
    Draw (new_x, new_y, new_x+ball_s, new_y+ball_s);
    print_solid(bool color, int x1, int y1, int x2, int y2)
    Update ();
}

// render_points clears old score and prints new score value for that side.
// player(0 left, 1 right), point(amount of points to add to score).
void render_score(bool player = 0,int point = 1) {
    if (i == 0)
        number_cord = 64 + 2 + width * number_pos
    if (i == 1)
        number_cord = 63 - 2 - width * number-number_pos
    delect number in list and add to wwrite array
}