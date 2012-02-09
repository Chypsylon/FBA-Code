/***********************
FILE: motor_header.h
DATE: 2.11.2011
***********************/
/* DEFINES */
#define MAX_PWM_VALUE 		1023
#define SLAVE_ADRESSE 		0x50  //I2C Slave Adresse

#define FLOAT 2000

/*MACROS*/
#define uniq(LOW,HEIGHT)	((HEIGHT << 8)|LOW)			// 2x 8Bit to 16Bit
#define LOW_BYTE(x)        	(x & 0xff)					// 16Bit to 8Bit
#define HIGH_BYTE(x)       	((x >> 8) & 0xff)			// 16Bit to 8Bit




/* PINBELEGUNG MOTOR

PORTB
  PB0 - Greifer 2 A
  PB1 - Motor 1 EN
  PB2 - Motor 2 EN
  PB3 - MOSI
  PB4 - MISO
  PB5 - SCK

PORTC
  PC0 - Batterie Spannung
  PC1 - Greifer 1 EN
  PC2 - Greifer 1 B
  PC3 - Greifer 2 EN
  PC4 - SDA
  PC5 - SCL
  PC6 - Reset
  
PORTD
  PD0 - Motor 1 A
  PD1 - Motor 1 B
  PD2 - Encoder 1
  PD3 - Encoder 2
  PD4 - Motor 2 B
  PD5 - Motor 2 A
  PD6 - Greifer 1 A
  PD7 - Greifer 2 B

*/



/*Ports*/
#define PORT_GRIPPER2_A 	PORTB
#define PORT_MOTOR1_EN 		PORTB
#define PORT_MOTOR2_EN 		PORTB

#define PORT_BATTERY_U 		PORTC
#define PORT_GRIPPER1_EN 	PORTC
#define PORT_GRIPPER1_B 	PORTC
#define PORT_GRIPPER2_EN 	PORTC

#define PORT_MOTOR1_A 		PORTD
#define PORT_MOTOR1_B 		PORTD
#define PORT_ENCODER1 		PORTD
#define PORT_ENCODER2 		PORTD
#define PORT_MOTOR2_B 		PORTD
#define PORT_MOTOR2_A 		PORTD
#define PORT_GRIPPER1_A 	PORTD
#define PORT_GRIPPER2_B 	PORTD


/*DDRx*/
#define DDR_GRIPPER2_A 		DDRB
#define DDR_MOTOR1_EN 		DDRB
#define DDR_MOTOR2_EN 		DDRB

#define DDR_BATTERY_U 		DDRC
#define DDR_GRIPPER1_EN 	DDRC
#define DDR_GRIPPER1_B 		DDRC
#define DDR_GRIPPER2_EN 	DDRC

#define DDR_MOTOR1_A 		DDRD
#define DDR_MOTOR1_B		DDRD
#define DDR_ENCODER1 		DDRD
#define DDR_ENCODER2 		DDRD
#define DDR_MOTOR2_B 		DDRD
#define DDR_MOTOR2_A 		DDRD
#define DDR_GRIPPER1_A 		DDRD
#define DDR_GRIPPER2_B 		DDRD


/*Pins*/
#define PIN_GRIPPER2_A 		PB0
#define PIN_MOTOR1_EN 		PB1
#define PIN_MOTOR2_EN 		PB2

#define PIN_BATTERY_U 		PC0
#define PIN_GRIPPER1_EN 	PC1
#define PIN_GRIPPER1_B 		PC2
#define PIN_GRIPPER2_EN 	PC3

#define PIN_MOTOR1_A		PD0
#define PIN_MOTOR1_B 		PD1
#define PIN_ENCODER1 		PD2
#define PIN_ENCODER2 		PD3
#define PIN_MOTOR2_B 		PD4
#define PIN_MOTOR2_A 		PD5
#define PIN_GRIPPER1_A 		PD6
#define PIN_GRIPPER2_B 		PD7


/************/
#define OCR_MOTOR1 			OCR1A
#define OCR_MOTOR2 			OCR1B




