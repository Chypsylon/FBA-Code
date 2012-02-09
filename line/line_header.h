/***********************
FILE: motor_header.h
DATE: 2.11.2011
***********************/
/* DEFINES */
#define SLAVE_ADRESSE 		0x40  //I2C Slave Adresse

#define ON  1
#define OFF 0

#define NUM_SENSORS 		8

/*I2C-Protocol*/
#define LINE_SENSOR_COMMAND		16
#define IR_LED_COMMAND			17
#define LINE_SENSOR_CALIBRATION_COMMAND 19


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

/*Line Leds*/
#define LINE_LED_ALL 			0
#define LINE_LED_48 			48
#define LINE_LED_37 			37
#define LINE_LED_26 			26
#define LINE_LED_15 			15



/*MACROS*/
#define uniq(LOW,HEIGHT)	((HEIGHT << 8)|LOW)			// 2x 8Bit to 16Bit
#define LOW_BYTE(x)        	(x & 0xff)					// 16Bit to 8Bit
#define HIGH_BYTE(x)       	((x >> 8) & 0xff)			// 16Bit to 8Bit




/* PINBELEGUNG MOTOR

PORTB
  PB0 - IR_LED_OBEN_RECHTS2
  PB1 - IR_LED_UNTEN_VORNE1
  PB2 - IR_LED_OBEN_LINKS1
  PB3 - MOSI
  PB4 - MISO
  PB5 - SCK/ IR_LED_UNTEN_VORNE2

PORTC
  PC0 - Liniensensor Multiplexer Output
  PC1 - Liniensensor Multiplexer A
  PC2 - Liniensensor Multiplexer B
  PC3 - Liniensensor Multiplexer C
  PC4 - SDA
  PC5 - SCL
  PC6 - Reset
  
PORTD
  PD0 - Liniensensor LED 4 und 8 Ein/Aus
  PD1 - Liniensensor LED 3 und 7 Ein/Aus
  PD2 - Liniensensor LED 2 und 6 Ein/Aus
  PD3 - Liniensensor LED 1 und 5 Ein/Aus
  PD4 - IR_LED_OBEN_VORNE2
  PD5 - IR_LED_OBEN_LINKS2
  PD6 - IR_LED_OBEN_VORNE1
  PD7 - IR_LED_OBEN_RECHTS1

*/



/*Ports*/
#define PORT_IR_LED_OBEN_RECHTS2	PORTB
#define PORT_IR_LED_UNTEN_VORNE1	PORTB
#define PORT_IR_LED_UNTEN_VORNE2	PORTB
#define PORT_IR_LED_OBEN_LINKS1		PORTB

#define PORT_LINE_MUX 				PORTC

#define PORT_LINE_LED 				PORTD
#define PORT_IR_LED_OBEN_VORNE1		PORTD
#define PORT_IR_LED_OBEN_LINKS2		PORTD
#define PORT_IR_LED_OBEN_VORNE2		PORTD
#define PORT_IR_LED_OBEN_RECHTS1	PORTD



/*DDRx*/
#define DDR_IR_LED_OBEN_RECHTS2		DDRB
#define DDR_IR_LED_UNTEN_VORNE1		DDRB
#define DDR_IR_LED_UNTEN_VORNE2		DDRB
#define DDR_IR_LED_OBEN_LINKS1		DDRB

#define DDR_LINE_MUX 				DDRC

#define DDR_LINE_LED 				DDRD
#define DDR_IR_LED_OBEN_VORNE1		DDRD
#define DDR_IR_LED_OBEN_LINKS2		DDRD
#define DDR_IR_LED_OBEN_VORNE2		DDRD
#define DDR_IR_LED_OBEN_RECHTS1		DDRD


/*Pins*/
#define PIN_IR_LED_OBEN_RECHTS2 	PB0
#define PIN_IR_LED_UNTEN_VORNE1 	PB1
#define PIN_IR_LED_OBEN_LINKS1	 	PB2
#define PIN_IR_LED_UNTEN_VORNE2	 	PB5

#define PIN_LINE_MUX_OUT			PC0
#define PIN_LINE_MUX_A				PC1
#define PIN_LINE_MUX_B				PC2
#define PIN_LINE_MUX_C				PC3

#define PIN_LINE_LED_15				PD0
#define PIN_LINE_LED_26				PD1
#define PIN_LINE_LED_37				PD2
#define PIN_LINE_LED_48				PD3
#define PIN_IR_LED_OBEN_VORNE2		PD4
#define PIN_IR_LED_OBEN_LINKS2		PD5
#define PIN_IR_LED_OBEN_VORNE1		PD6
#define PIN_IR_LED_OBEN_RECHTS1		PD7






