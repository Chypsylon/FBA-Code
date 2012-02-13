#ifndef MAIN_HEADER_C
#define MAIN_HEADER_C

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main_uart.h"
#include "i2cmaster.h"
#include "main_header.h"


void io_init (void)
{
  /* Ein-/Ausgänge festlegen*/
  DDR_LCD_BL |= (1<< PIN_LCD_BL); 
  DDR_STATUS_LED |= (1<<PIN_STATUS_LED);
  DDR_US_TRIGGER |= (1<<PIN_US_TRIGGER);
  DDR_IR_PULSE |= (1<<PIN_IR_PULSE);
  
  DDR_BUTTON &= ~(1<<PIN_BUTTON1);
  DDR_BUTTON &= ~(1<<PIN_BUTTON2);
  
  DDR_ENCODER &= ~(1<<PIN_ENCODER_BUTTON);
  DDR_ENCODER &= ~(1<<PIN_ENCODER_A);
  DDR_ENCODER &= ~(1<<PIN_ENCODER_B);
  
  /*Ausgänge ein-/auschalten*/
  PORT_LCD_BL &= ~(1<< PIN_LCD_BL);
  PORT_STATUS_LED &= ~(1<<PIN_STATUS_LED);
  PORT_US_TRIGGER &= ~(1<<PIN_US_TRIGGER);
  PORT_IR_PULSE &= ~(1<<PIN_IR_PULSE);
  
  /*Pullups ein-/auschalten*/
  PORT_BUTTON |= (1<<PIN_BUTTON1);
  PORT_BUTTON |= (1<<PIN_BUTTON2);
  
  PORT_ENCODER |= (1<<PIN_ENCODER_BUTTON);
  PORT_ENCODER |= (1<<PIN_ENCODER_A);
  PORT_ENCODER |= (1<<PIN_ENCODER_B);
}


//LCD Hintergrundbeleuchtung an/aus
//1=an, 0=aus
void lcd_bl (uint8_t state)
{
  if(state)
    PORT_LCD_BL |= (1<<PIN_LCD_BL); //LCD LED an
  else
    PORT_LCD_BL &= ~(1<<PIN_LCD_BL); //LCD LED aus
}


//Status LED an/aus
//1=an, 0=aus
void status_led (uint8_t state)
{
  if(state==1)
    PORT_STATUS_LED |= (1<<PIN_STATUS_LED); //StatusLED an
  else if (state==0)
    PORT_STATUS_LED &= ~(1<<PIN_STATUS_LED); //Status LED aus
}

//IR Entfernung ein/aus
void ir_pulse(uint8_t state)
{
  if(state==1)
    DDR_IR_PULSE |= (1<<PIN_IR_PULSE); //StatusLED an
  else if (state==0)
    DDR_IR_PULSE &= ~(1<<PIN_IR_PULSE); //Status LED aus
}

//start US-measurement
void trig_us(void)
{
  PORT_US_TRIGGER |= (1<<PIN_US_TRIGGER);//Trig high
  _delay_us(12);
  PORT_US_TRIGGER &= ~(1<<PIN_US_TRIGGER);//TRIG auf low
}


/*MOTOR SLAVE*/

//Sends motor speeds for both motors to the motor-slave
uint8_t send_motor12_speed(int16_t motor1_speed, int16_t motor2_speed)
{
  //Slave ready?
  if(!(i2c_start(MOTOR_SLAVE_ADRESSE+I2C_WRITE))) 
  {  
	  i2c_write(0);  //buffer startadress
	  i2c_write( LOW_BYTE(motor1_speed) ); 
	  i2c_write( HIGH_BYTE(motor1_speed) ); 
	  i2c_write( LOW_BYTE(motor2_speed) ); 
	  i2c_write( HIGH_BYTE(motor2_speed) ); 
	  i2c_stop();       //stop
	  
	  return 0;
  }
  else
    return 1;
}

//Sends motor speeds for motor 1 to the motor-slave
uint8_t send_motor1_speed(int16_t motor1_speed)
{
  //Slave ready?
  if(!(i2c_start(MOTOR_SLAVE_ADRESSE+I2C_WRITE))) 
  {  
	  i2c_write(0);  //buffer startadress
	  i2c_write( LOW_BYTE(motor1_speed) ); 
	  i2c_write( HIGH_BYTE(motor1_speed) );  
	  i2c_stop();       //stop
	  
	  return 0;
  }
  else
    return 1;
}

//Sends motor speeds for motor 2 to the motor-slave
uint8_t send_motor2_speed(int16_t motor2_speed)
{
  //Slave ready?
  if(!(i2c_start(MOTOR_SLAVE_ADRESSE+I2C_WRITE))) 
  {  
	  i2c_write(2);  //buffer startadress
	  i2c_write( LOW_BYTE(motor2_speed) ); 
	  i2c_write( HIGH_BYTE(motor2_speed) );  
	  i2c_stop();       //stop
	  
	  return 0;
  }
  else
    return 1;
}


//Send motor1 float command
uint8_t send_motor1_float(void)
{
  //Slave ready?
  if(!(i2c_start(MOTOR_SLAVE_ADRESSE+I2C_WRITE))) 
  {  
	  i2c_write(0);  //buffer startadress
	  i2c_write( LOW_BYTE(FLOAT) ); 
	  i2c_write( HIGH_BYTE(FLOAT) );  
	  i2c_stop();       //stop
	  
	  return 0;
  }
  else
    return 1;
}


//Send motor2 float command
uint8_t send_motor2_float(void)
{
  //Slave ready?
  if(!(i2c_start(MOTOR_SLAVE_ADRESSE+I2C_WRITE))) 
  {  
	  i2c_write(2);  //buffer startadress
	  i2c_write( LOW_BYTE(FLOAT) ); 
	  i2c_write( HIGH_BYTE(FLOAT) );  
	  i2c_stop();       //stop
	  
	  return 0;
  }
  else
    return 1;
}

/*Line Slave*/

//Set line sensor sampling mode
//mode: 0 stop sampling; 1-8 sample sensor; 10 sample continuesly
uint8_t send_line_mode(uint8_t mode)
{
  //Slave ready?
  if(!(i2c_start(LINE_SLAVE_ADRESSE+I2C_WRITE))) 
  {  
	  i2c_write(16);  //buffer startadress
	  i2c_write(mode);  
	  i2c_stop();       //stop
	  
	  return 0;
  }
  else
    return 1;
}

//get value of specified sensor
uint16_t read_line_sensor(uint8_t sensor)
{
  uint8_t values[2];
  values[0] = 0;
  values[1] = 0;
  
  if(sensor>8) return 0;
  
  if(!(i2c_start(LINE_SLAVE_ADRESSE+I2C_WRITE))) //Slave bereit zum lesen?
  {
    i2c_write((sensor*2)-2); //Buffer Startadresse zum Auslesen
    i2c_rep_start(LINE_SLAVE_ADRESSE+I2C_READ); //Lesen beginnen
    values[0] = i2c_readAck(); // Bytes lesen...
    values[1] = i2c_readNak(); // letztes Byte lesen, darum kein ACK
    i2c_stop();           // Zugriff beenden
  }
  
  return uniq(values[0],values[1]);
}

/* 0    =>no calibration
   1    => start calibration; look for highest and lowest values until 2
   2    => stop fetching values and save max. and min. in eeprom; set to 0 when finished
*/
uint8_t send_calibration_mode(uint8_t mode)
{
  //Slave ready?
  if(!(i2c_start(LINE_SLAVE_ADRESSE+I2C_WRITE))) 
  {  
	  i2c_write(19);  //buffer startadress
	  i2c_write(mode);  
	  i2c_stop();       //stop
	  
	  return 0;
  }
  else
    return 1;
}


//switch ir led on off
//modes defined in line_header.h
uint8_t send_ir_led(uint8_t led, uint8_t state)
{
  if(state>1) return 2; //state must be 1 or 0
  
  if(!(i2c_start(LINE_SLAVE_ADRESSE+I2C_WRITE))) 
  {  
	  i2c_write(17);  //buffer startadress
	  i2c_write(led);
	  i2c_write(state);
	  i2c_stop();       //stop
	  
	  return 0; //transmission sucessful
  }
  else
    return 1;  //slave not ready/responding
}



/*int to string*/

void uart_put_int(int8_t var)
{
  char buffer[20];
  uart_puts(itoa(var, buffer, 10));
}

void uart_put_uint(uint8_t var)
{
  char buffer[20];
  uart_puts(utoa(var, buffer, 10));
}

void uart_put_u16bit( uint16_t value )
{
    unsigned char digit;
    
    digit = '0';

    while( value >= 10000 )                // Still larger than 1000 ?
        {
            digit++;                         // Increment first digit
            value -= 10000;
        }
    	uart_putc( digit );

    while( value >= 1000 )                // Still larger than 100 ?
    {
        digit++;                         // Increment first digit
        value -= 1000;
    }
	uart_putc( digit ); 
	
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

void uart_put_16bit( int16_t value )
{
    unsigned char digit;
    
	if(value<0)
	{
	  uart_putc('-');
	  value *= -1;
	}
    
	digit = '0';
    while( value >= 1000 )                // Still larger than 100 ?
    {
        digit++;                         // Increment first digit
        value -= 1000;
    }
	uart_putc( digit ); 
	
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


/*ENCODER*/

void encode_init( void )
{
  int8_t new;
 
  new = 0;
  if( PHASE_A )
    new = 3;
  if( PHASE_B )
    new ^= 1;                   // convert gray to binary
  last = new;                   // power on state
  enc_delta = 0;

}

int8_t encode_read4( void )         // read four step encoders
{
  int8_t val;
 
  cli();
  val = enc_delta;
  enc_delta = val & 3;
  sei();
  return val >> 2;
}

#endif //MAIN_HEADER_C


