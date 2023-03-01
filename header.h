/* Written by Felix Bergqvist Widström and Fredrik Berzins (2023) */

#include <math.h>
#include <stdbool.h>

// Global varibles/settings import from main
   // Display
      extern int display_w;      // display width = pixels
      extern int display_h;      // display height = pixels
   // Ball
      extern int ball_s;         // ball size = pixels (it is a rectangle NOT ROUNDED)
   // Player paddle
      extern int paddle_w;       // paddle width = pixels
      extern int paddle_h;       // paddle height = pixels
      extern int paddle_s;       // paddle spacing to edge = pixels
   // Center dot/dot line
      extern int dot_w;          // dot width = pixels (must be even)
      extern int dot_h;          // dot height = pixels
      extern int dot_s;          // dot spacing to edge = pixels
   // Display/Font/Text
      // Declare bitmap array for display
      extern uint8_t display[512];
      // Declare bitmap array containing font
      extern const uint8_t const font[128*8];
      // Declare text buffer for display output
      extern char textbuffer[4][16];
      
// Screan related
   // Display mode defintions:
      // Command/data mode
         #define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
         #define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)
      // Display reset/not reset
         #define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
         #define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)
      // Power controll
         #define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
         #define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)
         #define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
         #define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

// Forward declared functions
   // SPI
      uint8_t spi_send_recv(uint8_t );
   // Display effects a larger area
      void display_init();
      void display_main();
      void display_update();
      void display_string(int , char *);
      void display_image(int , const uint8_t *);
      void display_clear();
   // Print effects specific aresas
      void print_start_screen();
      void print_end_screen(int player);
      void print_string(int, char *);
      void print_solid(int, int, int, int, int);
      void print_dotted_line();
      void move_paddle(int, int);
      void move_ball(int, int, int, int);
   // Game related
      void initialize();
      void update_controls();
      bool check_controls();
      void initialize_controls();
      void move();
      void check_collision();
   // Misc
      void quicksleep(int);
      int power(int, int);
      int lower_8(int);
      void sleep(int);