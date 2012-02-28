
/*INCLUDES*/

//Standard C-Libs
#include <stdlib.h>


//Standard AVR-Libs
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>


//Libs
#include "main_taster.h"
#include "main_uart.h" 
#include "i2cmaster.h"  
#include "main_lcd.h"

#include "main_header.h"
#include "main_lib.h"
#include "menu_terminal.h"




int16_t motor1_speed = 0;
int16_t motor2_speed = 0;

volatile uint16_t timestamp_last=0;
volatile uint16_t runtime=0;




//called every 1,024ms
ISR(TIMER0_OVF_vect)
{
   static uint8_t millisec_count;
   
   
   millisec_count++;
   
   //led toggle
   if((millisec_count % 100) == 0)
   {
     //switch LED on/off
	 status_led(flags.led_state);
	 //invert LED state
	 if(flags.led_state)
	   flags.led_state = 0;
	 else
	   flags.led_state = 1;
   }
   
   //Button debounce
   if((millisec_count % 10) == 0)
   {
     get_taster (BUTTON_1, (PINC & (1<<PIN_BUTTON1)));
     get_taster (BUTTON_2, (PINC & (1<<PIN_BUTTON2)));
	 get_taster (BUTTON_ENCODER, (PINB & (1<<PIN_ENCODER_BUTTON)));
   }
   
   //Encoder
   if((millisec_count % 2) == 0)
   {
     int8_t new, diff;
     new = 0;
     if( PHASE_A )
       new = 3;
     if( PHASE_B )
       new ^= 1;                   // convert gray to binary
     diff = last - new;                // difference last - new
     if( diff & 1 )  // bit 0 = value (1)
     {               
       last = new;                 // store new as next last
       enc_delta += (diff & 2) - 1;        // bit 1 = direction (+/-)
     }
   }
   
   if((millisec_count % 50) == 0)
   {
     trig_us();
   }
   
}

/*US Messung*/
ISR(TIMER1_CAPT_vect)
{
  //Wenn steigende Flanke
  if(TCCR1B & (1<<ICES1))
  {
    //Flankenerkennung auf fallend
	TCCR1B ^= (1<<ICES1);
	//Aktuellen timer-wert speichern
	timestamp_last = ICR1;
  }
  //fallende Flanke
  else
  {
    //Flankenerkennung auf steigend
	TCCR1B ^= (1<<ICES1);
	//Laufzeit = aktueller timerwert - vorheriger timerwert
	runtime = ICR1 - timestamp_last;
	flags.us_state = READY;
  }
  
}



int main(void)
{  


  int16_t encoder_rotation = 0;
  int16_t encoder_rotation_old = 0;
  
  uint8_t line_position = 0;
  uint16_t line_estimate = 0;
  uint32_t wa_numerator = 0;
  uint16_t wa_denominator = 0;

  uint16_t line_values[8];
  
  //PID stuff
  //int16_t proportional = 0;
  int16_t last_proportional = 0;
  int16_t integral = 0;
  //int16_t derivative = 0;
  int16_t error_value = 0;
  uint16_t Kp = 0;
  uint16_t Kd = 0;
  uint16_t Ki = 0;

  //Menu
  uint16_t menu=0;


  for(uint8_t i=0;i<8;i++)
  		line_values[i] = 0;

  io_init();
  

  lcd_init(LCD_DISP_ON);

  lcd_bl(1);

  
  /*TIMER CONFIG*/
  
  /*System Tick*/
  // Prescaler auf 64
  TCCR0|=(1<<CS01)|(1<<CS00);

  //Overflow Interrupt aktivieren
  TIMSK|=(1<<TOIE0);
  
  /*US Timer*/
  //Counter initialisieren
  TCNT0=0;
  
  //Timer konfigurieren
  TCCR1A = 0;                      // normal mode, keine PWM Ausgänge
  //Noise Canceler aktivieren, Flankenerkennung auf steigende, Prescaler auf 64
  TCCR1B |= (1<<ICNC1) |       (1<<ICES1) |                     (1<<CS11) |(1<<CS10);
  
  //ICP Interrupt aktivieren
  TIMSK |= (1<<TICIE1);
  
  /*IR Timer*/
  OCR2 = 240;
  
  TCCR2 |= (1<<CS20) | (1<<WGM21) | (1<<COM20);
  
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
  
  _delay_ms(1000);  //wait for slaves to initialize properly
  
  i2c_init();         // init I2C interface
  encode_init();
  
  tasten[0].mode = TM_LONG;
  tasten[1].mode = TM_LONG;
  tasten[2].mode = TM_LONG;
  
  sei();
  
  
  send_line_mode(10);  //start sampling
  
  
  while(1)
  {
	
	/*read encoder and display if changed*/
	encoder_rotation = 0;
	encoder_rotation += encode_read4() ;
	if(encoder_rotation != encoder_rotation_old)  //has changed since last time
	{
		switch (menu) {
			case 2:
				Kp += encoder_rotation*10;
				break;
			case 3:
				Kd += encoder_rotation;
				break;
			case 4:
				Ki += encoder_rotation;
				break;
			case 7:
				motor1_speed += (encoder_rotation * 10);
				motor2_speed = motor1_speed;
				break;
			default:
				break;
		}
	}
	encoder_rotation_old = encoder_rotation;
	//encoder.end
	
	
	//check whether new us time is available for calculation
	if(flags.us_state == READY)
	{
	  //calculate distance from travel time
	  //*4 => scale to milliseconds
	  //runtime = (runtime*4)/58;
	  runtime /= 15;
	  flags.us_state = NOT_READY;
	}
	
	//Read line sensors values from slave
	//binary line detection
	for(uint8_t i=0;i<NUM_SENSORS;i++)
	{
		line_values[i] = read_line_sensor(i+1);

		if(line_values[i]>LINE_TRESHOLD)
		{
			line_position |= (1<<(7-i));

			//values for line estimation via weighted mean
			//only values above threshold (e.g. black) are considered in calculation
			wa_numerator += (long)(line_values[i]) *((i+1) *100);
			wa_denominator += line_values[i];
		}
		else  {
			line_position &= ~(1<<(7-i));
		}
	}

	//prevent division through zero
	if(line_position==0)  {  //not on line
		if(line_estimate < 450)  {
			line_estimate = 100;
		}
		else  {
			line_estimate = 800;
		}

		}
	else  {
		line_estimate = wa_numerator/wa_denominator;
	}

	//reset values
	wa_numerator = 0;
	wa_denominator = 0;

	/*
	for(uint8_t i=0;i<NUM_SENSORS;i++)
	{
		if(line_position & (1<<(7-i)))  {
			lcd_putc('1');
		}
		else  {
			lcd_putc('0');
		}
	}*/
	

	//PID
	int16_t proportional = ((int)line_estimate) - SET_POINT;  //should be 0 when over line
	int16_t derivative = proportional - last_proportional;
	integral += proportional;
	last_proportional = proportional;
	//Difference between motor speeds. if positive -> turn right, if negative turn left
	error_value = ((proportional * Kp)/100) + integral * Ki + derivative * Kd;

	//drive motors

	const int max = 900;
	if(error_value > max)  {
		error_value = max;
	}
	if(error_value < -max)  {
		error_value = -max;
	}

	if(error_value < 0)  {
		motor1_speed = max + error_value;
		motor2_speed = max;
	}
	else  {
		motor1_speed = max;
		motor2_speed = max - error_value;
	}


	signed char tast = taster;
	
	switch(tast)
	{
	  case NO_TASTER:
		break;
		
	  case BUTTON_1:
		  menu++;
		break;
		
	  case BUTTON_1 + TASTER_LONG:
		break;	
		
	  case BUTTON_2:
		  if(menu)
			  menu--;
		break;
		
	  case BUTTON_2 + TASTER_LONG:
		break;
		
	  case BUTTON_ENCODER:
		  if(menu==5)  {
			if (!flags.run_start) {
				flags.run_start = 1;
			}
			else  {
				flags.run_start = 0;
			}
		  }
		  if(menu==7)  {
			  if (flags.motor_start) {
				  flags.motor_start = 0;
			  }
			  else  {
				  flags.motor_start = 1;
			  }
		  }
		break;
		
	  case BUTTON_ENCODER + TASTER_LONG:		
		break;
	}
	if (tast != NO_TASTER)
		taster = NO_TASTER;

	switch (menu) {
		case 0:
			lcd_clrscr();
			lcd_gotoxy(0,LCD_LINE1);
			lcd_puts_P("Alexander Kargl");
			lcd_gotoxy(0,LCD_LINE2);
			lcd_puts_P("Btn1++  Btn2--");
			break;
		case 1:
			lcd_clrscr();
			lcd_gotoxy(0,LCD_LINE1);
			lcd_puts_P("Line Position");
			lcd_gotoxy(0,LCD_LINE2);
			lcd_put_uint16(line_estimate);
			break;
		case 2:
			lcd_clrscr();
			lcd_gotoxy(0,LCD_LINE1);
			lcd_puts_P("Kp");
			lcd_gotoxy(0,LCD_LINE2);
			lcd_put_uint16(Kp);
			break;
		case 3:
			lcd_clrscr();
			lcd_gotoxy(0,LCD_LINE1);
			lcd_puts_P("Kd");
			lcd_gotoxy(0,LCD_LINE2);
			lcd_put_uint16(Kd);
			break;
		case 4:
			lcd_clrscr();
			lcd_gotoxy(0,LCD_LINE1);
			lcd_puts_P("Ki");
			lcd_gotoxy(0,LCD_LINE2);
			lcd_put_uint16(Ki);
			break;
		case 5:
			lcd_clrscr();
			lcd_gotoxy(0,LCD_LINE1);

			if (!flags.run_start) {
				lcd_puts_P("Start Program");
			}
			else  {
				lcd_puts_P("Stop Program");
			}
			lcd_gotoxy(0,LCD_LINE2);
			lcd_put_uint8(flags.run_start);
			break;
		case 6:
			lcd_clrscr();
			lcd_gotoxy(0,LCD_LINE1);
			lcd_puts_P("P");
			lcd_put_int16(proportional);
			lcd_gotoxy(9,LCD_LINE1);
			lcd_puts_P("D");
			lcd_put_int16(derivative);
			lcd_gotoxy(0,LCD_LINE2);
			lcd_puts_P("I");
			lcd_put_int16(integral);
			lcd_gotoxy(9,LCD_LINE2);
			lcd_puts_P("E");
			lcd_put_int16(error_value);
			break;
		case 7:
			lcd_clrscr();
			lcd_gotoxy(0,LCD_LINE1);
			lcd_puts_P("M1 ");
			lcd_put_int16(motor1_speed);
			lcd_gotoxy(15,LCD_LINE1);
			lcd_put_uint8(flags.run_start);
			lcd_gotoxy(0,LCD_LINE2);
			lcd_puts_P("M2 ");
			lcd_put_int16(motor2_speed);
			break;
		default:
			break;
	}
	  
	  

	if(flags.motor_start)
	{
		send_motor1_speed(motor1_speed);
		send_motor2_speed(motor2_speed);
	}
	else
	{
		send_motor1_speed(0);
		send_motor2_speed(0);
	}

	 
   }//while.end
  
  return 0;
}//main.end
