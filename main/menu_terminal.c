#include <avr/pgmspace.h>

#include "main_uart.h" 
#include "menu_terminal.h"


#define SENDESC uart_puts("\033[");

//***************************
// Convert byte to 3 ASCII digits and send
//***************************
void term_send_value_as_digits( unsigned char value )
{
    unsigned char digit;
    
    digit = '0';
    while( value >= 100 )                // Still larger than 100 ?
    {
        digit++;                         // Increment first digit
        value -= 100;
    }
    
   uart_putc( digit );                  // Send first digit
    
    digit = '0';
    while( value >= 10 )                 // Still larger than 10 ?
    {
        digit++;                         // Increment second digit
        value -= 10;
    }
    
    uart_putc( digit );                  // Send second digit
    
    uart_putc( '0' + value );            // Send third digit
}


//***************************
// Decode incoming ESC sequence
//***************************
unsigned int term_get_sequence(void)
{
    unsigned char val;
    unsigned int ret;
    
    val = uart_getc();
    if( val != 27 )                      // Received ESC ?
    {
        ret = val;                       // If not, return char
    }
    else
    {
        val = uart_getc();
        if( val != '[' )                 // Received '[' ?
        {
            ret = val;                   // If not, return char
        }
        else
        {
            val = uart_getc();
            
            if( val == 'A' ||
                val == 'B' ||
                val == 'C' ||
                val == 'D' )             // Arrow keys ?
            {
                ret = val << 8;          // Put code in upper byte
            }
            else
            {
                ret = val;               // If not, return char
            }
        }
    }
    return ret;
}


//***************************
// Send 'clear bottom of screen' sequence
//***************************
void term_erase_screen_bottom(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_putc( 'J' );
}


//***************************
// Send 'clear top of screen' sequence
//***************************
void term_erase_screen_top(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_puts("1J");
}


//***************************
// Send 'clear screen' sequence
//***************************
void term_erase_screen(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_puts("2J");
}


//***************************
// Send 'clear end of line' sequence
//***************************
void term_erase_to_end_of_line(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_putc('K');
}


//***************************
// Send 'clear start of line' sequence
//***************************
void term_erase_to_start_of_line(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_puts("1K");
}


//***************************
// Send 'clear line' sequence
//***************************
void term_erase_line(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_puts("2K");
}


//***************************
// Set text mode
//***************************
void term_set_display_attribute_mode( unsigned char mode )
{
    SENDESC                             // Send escape sequence start
    
    uart_putc( mode );
    uart_putc( 'm' );
}


//***************************
// Set text colour
//***************************
void term_set_display_colour( unsigned char fg_bg, unsigned char colour )
{
    SENDESC                             // Send escape sequence start
    
    uart_putc( fg_bg );                 // Select foreground/background
    uart_putc( colour );
    uart_putc( 'm' );
}


//***************************
// Set cursor position
//
// Top-left is (1,1)
//***************************
void term_set_cursor_position( unsigned char row, unsigned char column )
{
    SENDESC                                        // Send escape sequence start
    
    term_send_value_as_digits( row );              // Convert row byte
    uart_putc( ';' );
    term_send_value_as_digits( column );           // Convert column byte
    uart_putc( 'H' );
}


//***************************
// Move cursor
//***************************
void term_move_cursor( unsigned char distance, unsigned char direction )
{
    SENDESC                             // Send escape sequence start
    
    term_send_value_as_digits( distance );         // Convert distance byte

    uart_putc( direction );
}



//***************************
// Save cursor position
//***************************
void term_save_cursor_position(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_putc( 's' );
}


//***************************
// Restore cursor position
//***************************
void term_restore_cursor_position(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_putc( 'u' );
}


//***************************
// Enable scroll for entire screen
//***************************
void term_set_scroll_mode_all(void)
{
    SENDESC                             // Send escape sequence start
    
    uart_putc( 'r' );
}


//***************************
// Limit screen scrolling to some lines only
//***************************
void term_set_scroll_mode_limit( unsigned char start, unsigned char end )
{
    SENDESC                             // Send escape sequence start
    
    term_send_value_as_digits( start );            // Convert start line byte
    uart_putc( ';' );
    term_send_value_as_digits( end );              // Convert end line byte
    uart_putc( 'r' );
}


//***************************
// Send 'print screen' command sequence
//***************************
void term_print_screen(void)
{
    SENDESC                             // Send escape sequence start
   
    uart_putc( 'i' );
}



//***************************
//Initizalize Terminal
//**************************

void init_term(void)
{
  term_set_display_attribute_mode( MODE_NONE ); // Disable all previous modes
  term_erase_screen();                          // Clear screen
  term_set_cursor_position( 1, 1 );             // Move to top-left corner
}


//***************************
// Draw frame on terminal client
//***************************
void term_draw_frame( unsigned char top, unsigned char left, 
                      unsigned char height, unsigned char width,
                      unsigned char doubleFrame )
{
    /*** Defines egde character codes for both single and double frames ***/
    static unsigned char edges[] PROGMEM = { 0xda, 0xbf, 0xc0, 0xd9, 0xc4, 0xb3,
                                             0xc9, 0xbb, 0xc8, 0xbc, 0xcd, 0xba };

    unsigned char i = 0;
    height++;
    width++;    
    
    /*** Draw frame ***/
    term_set_cursor_position( top, left );                // Corners first
    uart_putc( pgm_read_byte(&edges[ doubleFrame * 6 + 0 ] ));
    
    term_set_cursor_position( top, left + width );
    uart_putc( pgm_read_byte(&edges[ doubleFrame * 6 + 1 ] ));
    
    term_set_cursor_position( top + height, left );
    uart_putc( pgm_read_byte(&edges[ doubleFrame * 6 + 2 ] ));
    
    term_set_cursor_position( top + height, left + width );
    uart_putc( pgm_read_byte(&edges[ doubleFrame * 6 + 3 ] ));
    
    for( i = left + 1; i < left + width; i++ )           // Top and bottom edges
    {
        term_set_cursor_position( top, i );
        uart_putc( pgm_read_byte(&edges[ doubleFrame * 6 + 4 ] ));
        
        term_set_cursor_position( top + height, i );
        uart_putc( pgm_read_byte(&edges[ doubleFrame * 6 + 4 ] ));
    }
    
    for( i = top + 1; i < top + height; i++ )            // Left and right edges
    {
        term_set_cursor_position( i, left );
        uart_putc( pgm_read_byte(&edges[ doubleFrame * 6 + 5 ] ));
        
        term_set_cursor_position( i, left + width );
        uart_putc( pgm_read_byte(&edges[ doubleFrame * 6 + 5 ] ));
    }
}



//***************************
// Draw menu on terminal client
//***************************
unsigned char term_draw_menu( unsigned char *menu, unsigned char selectPos,
                              unsigned char top, unsigned char left, unsigned char doubleFrame )
{
    unsigned char i, width, height;
    unsigned char *ptr = menu;
    
    width = height = 0;
    
    while( *ptr != 0 )                          // Scan through menu string
    {
        height++;                               // Keep track of item count

        if( selectPos == height )
        {
            term_set_display_attribute_mode( MODE_REVERSED );  // Reverse selected item
        }
        else
        {
            term_set_display_attribute_mode( MODE_NONE );
        }
    
        term_set_cursor_position( top + height, left + 1 );   // Start of item text
        
        i = 0;
        
        while( *ptr != '\n' )                   // Scan through item text
        {
            uart_putc( *ptr );                  // Print item character
            i++;                                // Item character count
            ptr++;
        }
        
        ptr++;                                  // Skip newline character
        
        if( i > width )
            width = i;                          // Always keep max width
    }
    
    term_set_display_attribute_mode( MODE_NONE );
    if( selectPos == 0 )                        // Draw frame only if selectPos == 0
    {
        term_draw_frame( top, left, height, width, doubleFrame );        
    }
    
    term_set_cursor_position( top + selectPos, left + 1 );    // Postition at start of selected item
    return height;
}


//***************************
// Handle menu
//***************************
unsigned char term_handle_menu( unsigned char *menu, unsigned char selectPos,
                                unsigned char top, unsigned char left, unsigned char doubleFrame )
{
    unsigned char height;
    unsigned int ret;
        
    /* Print menu frame */
    height = term_draw_menu( menu, 0, top, left, doubleFrame );
    
    while(1)
    {
        /* Print menu text with selected item reversed */
        term_draw_menu( menu, selectPos, top, left, doubleFrame );
        
        ret = term_get_sequence();             // Decode ESC sequence
        
        if( ret == 13 )                        // Exit on ENTER
            return selectPos;
            
        if( (ret & 0xff) == 0 )                // Arrow key ?
        {
            ret >>= 8;
            
            if( ret == MOVE_UP )
            {
                selectPos--;                   // Move selection up, with wrap
                if( selectPos < 1 )
                    selectPos = height;
            } else if( ret == MOVE_DOWN )
            {
                selectPos++;                   // Move selection down, with wrap
                if( selectPos > height )
                    selectPos = 1;
            }
        }
    }
}

