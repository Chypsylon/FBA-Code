#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H

/***********************
FILE: main_header.h
DATE: 2.11.2011
***********************/

/* DEFINES */
#define NUM_SENSORS 		8

//LCD
#define LCD_LINE1 0
#define LCD_LINE2 1

//line detection
#define LINE_TRESHOLD 150
#define SET_POINT 450

#define UART_BAUD_RATE      9600
//i2c adresses
#define MOTOR_SLAVE_ADRESSE 0x50
#define LINE_SLAVE_ADRESSE 0x40

//motor float
#define FLOAT 2000

//encoders
#define PHASE_A     (PINB & 1<<PIN_ENCODER_A) //Encoder
#define PHASE_B     (PINB & 1<<PIN_ENCODER_B)

//buttons
#define BUTTON_1 		0
#define BUTTON_2 		1
#define BUTTON_ENCODER	2

#define READY 		1
#define NOT_READY	0

/*IR Leds*/
#define IR_LED_UNTEN_VORNE1 	1
#define IR_LED_UNTEN_VORNE2 	2
#define IR_LED_OBEN_LINKS2		3
#define IR_LED_OBEN_LINKS1		4
#define IR_LED_OBEN_VORNE1		5
#define IR_LED_OBEN_VORNE2		6
#define IR_LED_OBEN_RECHTS1		7
#define IR_LED_OBEN_RECHTS2		8

#define IR_LED_ALL 				0
#define IR_LED_UNTEN_VORNE_ALL 	12
#define IR_LED_OBEN_VORNE_ALL 	56
#define IR_LED_OBEN_LINKS_ALL 	34
#define IR_LED_OBEN_RECHTS_ALL 	78

/*MACROS*/
#define uniq(LOW,HEIGHT)	((HEIGHT << 8)|LOW)			// 2x 8Bit 	--> 16Bit
#define LOW_BYTE(x)        	(x & 0xff)					// 16Bit 	--> 8Bit
#define HIGH_BYTE(x)       	((x >> 8) & 0xff)			// 16Bit 	--> 8Bit



/*Global Variables*/
//Flags
struct {
   unsigned led_state:1;
   volatile unsigned us_state:1;
   unsigned run_start:1;
   unsigned motor_start:1;
} flags;




//Encoder
volatile int8_t enc_delta;          // -128 ... 127
static int8_t last;






/* PINBELEGUNG MAIN

PORTA
  PA0 - IR Receiver vorne unten
  PA1 - IR Receiver links oben
  PA2 - IR Receiver vorne oben
  PA3 - IR Receiver rechts oben
  PA4 - Switch 4
  PA5 - Switch 3
  PA6 - Switch 2
  PA7 - Switch 1

PORTB
  PB0 - Tilt Sensor
  PB1 - Status Led
  PB2 - Encoder Button
  PB3 - Encoder B
  PB4 - Encoder A
  PB5 - LCD Backlight / ISP MOSI
  PB6 - ISP MISO
  PB7 - ISP SCK

PORTC
  PC0 - I2C SCL
  PC1 - I2C SDA
  PC2 - LCD Enable
  PC3 - LCD R/W
  PC4 - LCD RS
  PC5 - Button 1
  PC6 - Button 2
  PC7 - US Trigger
  
PORTD
  PD0 - UART RX
  PD1 - UART TX
  PD2 - LCD DB7
  PD3 - LCD DB6
  PD4 - LCD DB5
  PD5 - LCD DB4
  PD6 - US Echo
  PD7 - IR Pulse

*/



/*Ports*/
#define PORT_IR_RECEIVER  PORTA
#define PORT_SWITCH       PORTA

#define PORT_TILT_SENSOR  PORTB
#define PORT_STATUS_LED   PORTB
#define PORT_ENCODER      PORTB
#define PORT_LCD_BL       PORTB

#define PORT_LCD_E        PORTC
#define PORT_LCD_RW       PORTC
#define PORT_LCD_RS       PORTC
#define PORT_BUTTON       PORTC
#define PORT_US_TRIGGER   PORTC

#define PORT_LCD_DATA     PORTD
#define PORT_US_ECHO      PORTD
#define PORT_IR_PULSE     PORTD

/*DDRx*/

#define DDR_IR_RECEIVER  DDRA
#define DDR_SWITCH       DDRA

#define DDR_TILT_SENSOR  DDRB
#define DDR_STATUS_LED   DDRB
#define DDR_ENCODER      DDRB
#define DDR_LCD_BL       DDRB

#define DDR_LCD_E        DDRC
#define DDR_LCD_RW       DDRC
#define DDR_LCD_RS       DDRC
#define DDR_BUTTON       DDRC
#define DDR_US_TRIGGER   DDRC

#define DDR_LCD_DATA     DDRD
#define DDR_US_ECHO      DDRD
#define DDR_IR_PULSE     DDRD

/*Pins*/
#define PIN_IR_RECEIVER_VORNE_UNTEN PA0
#define PIN_IR_RECEIVER_LINKS_OBEN  PA1
#define PIN_IR_RECEIVER_VORNE_OBEN  PA2
#define PIN_IR_RECEIVER_RECHTS_OBEN PA3
#define PIN_SWITCH4                 PA4
#define PIN_SWITCH3                 PA5
#define PIN_SWITCH2                 PA6
#define PIN_SWITCH1                 PA7

#define PIN_TILT_SENSOR             PB0
#define PIN_STATUS_LED              PB1
#define PIN_ENCODER_BUTTON          PB2
#define PIN_ENCODER_B               PB3
#define PIN_ENCODER_A               PB4
#define PIN_LCD_BL                  PB5

#define PIN_LCD_E 					PC2
#define PIN_LCD_RW					PC3
#define PIN_LCD_RS					PC4
#define PIN_BUTTON1					PC5
#define PIN_BUTTON2 				PC6
#define PIN_US_TRIGGER				PC7

#define PIN_LCD_DB7					PD2
#define PIN_LCD_DB6					PD3
#define PIN_LCD_DB5					PD4
#define PIN_LCD_DB4					PD5
#define PIN_US_ECHO					PD6
#define PIN_IR_PULSE				PD7



#endif //MAIN_HEADER_H
