
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

//line estimation
//int8_t sensor_position[NUM_SENSORS] = {-32, -15, -9, -3, 3, 9, 15, 32 };



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
  /*int32_t wa_numerator = 0;
  uint16_t wa_denominator = 0;*/

  uint16_t line_values[8];
  
  flags.calibration = 0;

  for(uint8_t i=0;i<8;i++)
  		line_values[i] = 0;

  io_init();
  

  lcd_init(LCD_DISP_ON);

  lcd_bl(1);

  lcd_puts_P("Test");

  
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
	encoder_rotation += encode_read4() ;
	if(encoder_rotation != encoder_rotation_old)
	{
	  uart_puts_P("Encoder: ");
	  uart_put_16bit(encoder_rotation);
	  uart_puts_P(" \r\n");
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
	
	
	for(uint8_t i=0;i<NUM_SENSORS;i++)
	{
		line_values[i] = read_line_sensor(i+1);
		if(line_values[i]>LINE_TRESHOLD)  {
			line_position |= (1<<(7-i));
		}
		else  {
			line_position &= ~(1<<(7-i));
		}
	}



	//estimate line position
//	/* value0 * position0 + value1 * position1 + value2 * position2 ...
//	 * ----------------------------------------------------------------
//	 * value0 + value1 + value2 ...
//	 */
//	for(uint8_t i=0; i<NUM_SENSORS; i++)
//	{
//		//wa_numerator   += (long)(line_values[i] * sensor_position[i]);
//		wa_numerator   += (long)line_values[i] * (i * 1000);
//		wa_denominator +=  line_values[i];
//	}
//
//	line_position = wa_numerator / wa_denominator;
//
//	/*line_position = (line_values[0] * sensor_position[0] +
//			         line_values[1] * sensor_position[1] +
//			         line_values[2] * sensor_position[2] +
//			         line_values[3] * sensor_position[3] +
//			         line_values[4] * sensor_position[4] +
//			         line_values[5] * sensor_position[5] +
//			         line_values[6] * sensor_position[6] +
//			         line_values[7] * sensor_position[7])/
//			         (line_values[0] +
//			          line_values[1] +
//			          line_values[2] +
//			          line_values[3] +
//			          line_values[4] +
//			          line_values[5] +
//			          line_values[6] +
//			          line_values[7]);*/



	lcd_gotoxy(0,1);
	lcd_puts_P("          ");
	lcd_gotoxy(0,1);

	char buffer[20];

	lcd_puts(itoa(line_position, buffer, 2));


	//_delay_ms(100);
	
	
	
	
	
	signed char tast = taster;
	
	
	switch(tast)
	{
	  case NO_TASTER:
		break;
		
	  case BUTTON_1:
		//flags.start = NOT_READY;
		  if(flags.cal_values == 0)
		  {
			  uart_puts_P(" Stop sampling\r\n");
			  send_line_mode(0);
			  uart_puts_P(" MIN prepared for transmission");
			  send_calibration_mode(3);
			  flags.cal_values = 1;
		  }
		  else
		  {
			  uart_puts_P(" Stop sampling\r\n");
			  send_line_mode(0);
			  uart_puts_P(" MAX prepared for transmission");
			  send_calibration_mode(4);
			  flags.cal_values = 0;
		  }


		break;
		
	  case BUTTON_1 + TASTER_LONG:
	  	uart_puts_P("Transmitting normalized values");
	  	send_line_mode(12);
		break;	
		
	  case BUTTON_2:
		//flags.start = READY;
		if(flags.calibration==0)
		{
			uart_puts_P(" STARTED Calibration\r\n");
			send_calibration_mode(1);
			flags.calibration = 1;
		}
		else
		{
			uart_puts_P(" SAVE and STOP CALIBRATION \r\n");
			send_calibration_mode(2);
			flags.calibration = 0;
		}

		break;
		
	  case BUTTON_2 + TASTER_LONG:
		break;
		
	  case BUTTON_ENCODER:
	    for(uint8_t i=0;i<NUM_SENSORS;i++)
		{
		  uart_puts_P("Sensor ");
		  uart_put_uint(i);
		  uart_puts_P(": ");
	      uart_put_16bit(line_values[i]);
	      uart_puts_P(" \r\n");
		}
		uart_puts_P("-------------------------\r\n\r\n");
		/*uart_puts_P("Displacement: ");
		uart_put_int(line_position);
		uart_puts_P(" \r\n");
		uart_puts_P("wa_numerator: ");
		uart_put_16bit(wa_numerator);
		uart_puts_P(" \r\n");
		uart_puts_P("wa_denominator: ");
		uart_put_u16bit(wa_denominator);
		uart_puts_P(" \r\n");*/
		break;
		
	  case BUTTON_ENCODER + TASTER_LONG:		
		break;
	}//switch.end
	
	if (tast != NO_TASTER)
	  taster = NO_TASTER;
	  
	  


	motor1_speed = 0;
	motor2_speed = 0;

	send_motor1_speed(motor1_speed);
	send_motor2_speed(motor2_speed);
	 
   }//while.end
  
  return 0;
}//main.end
