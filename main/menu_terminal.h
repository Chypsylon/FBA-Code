

//***************************
// Misc definitions
//***************************

/*** Text modes ***/
#define MODE_NONE         '0'
#define MODE_BOLD         '1'
#define MODE_DIM          '2'
#define MODE_UNDERLINE    '4'
#define MODE_BLINK        '5'
#define MODE_REVERSED     '7'
#define MODE_CONCEALED    '8'


/*** Text colours ***/
#define COL_FOREGROUND    '3'
#define COL_BACKGROUND    '4'

#define COL_BLACK         '0'
#define COL_RED           '1'
#define COL_GREEN         '2'
#define COL_YELLOW        '3'
#define COL_BLUE          '4'
#define COL_MAGENTA       '5'
#define COL_CYAN          '6'
#define COL_WHITE         '7'


/*** Cursor move direction ***/
#define MOVE_UP           'A'
#define MOVE_DOWN         'B'
#define MOVE_RIGHT        'C'
#define MOVE_LEFT         'D'



//***************************
// Function prototypes
//***************************


void term_send_value_as_digits( unsigned char value );
unsigned int term_get_sequence(void);
void term_erase_screen_bottom(void);
void term_erase_screen_top(void);
void term_erase_screen(void);
void term_erase_to_end_of_line(void);
void term_erase_to_start_of_line(void);
void term_erase_line(void);
void term_set_display_attribute_mode( unsigned char mode );
void term_set_display_colour( unsigned char fg_bg, unsigned char colour );
void term_set_cursor_position( unsigned char row, unsigned char column );
void term_move_cursor( unsigned char distance, unsigned char direction );
void term_save_cursor_position(void);
void term_restore_cursor_position(void);
void term_set_scroll_mode_all(void);
void term_set_scroll_mode_limit( unsigned char start, unsigned char end );
void term_print_screen(void);
void init_term(void);
void term_draw_frame( unsigned char top, unsigned char left, unsigned char height, unsigned char width, unsigned char doubleFrame );
unsigned char term_draw_menu( unsigned char *menu, unsigned char selectPos, unsigned char top, unsigned char left, unsigned char doubleFrame );
unsigned char term_handle_menu( unsigned char *menu, unsigned char selectPos, unsigned char top, unsigned char left, unsigned char doubleFrame );

