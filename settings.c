// Global varibles/settings varibles

// Display
int display_w = 128;    // display width = pixels
int display_h = 32;     // display height = pixels

// Ball
int ball_s = 2;         // ball size = pixels (it is a rectangle NOT ROUNDED)
int ball_v = 1;         // ball speed

// Player paddle
int paddle_w = 2;       // paddle width = pixels
int paddle_h = 10;      // paddle height = pixels
int paddle_s = 8;       // paddle spacing to edge = pixels
int paddle_v = 1;       // paddle move speed

// Center dot/dot line
int dot_w = 2;          // dot width = pixels (must be even)
int dot_h = 2;          // dot height = pixels
int dot_s = 2;          // dot spacing to edge = pixels

// Score
int player1 = 0;        // score for player 1
int player2 = 0;        // score for player 2
int limit = 5;          // score required to win(max 10, other wise the score printing will fail), i.e first to x wins