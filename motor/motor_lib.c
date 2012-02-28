#include <avr/io.h>

#include "twislave.h"
#include "motor_header.h"


void io_init (void)
{
	/* Ein-/Ausgänge festlegen*/
	//Greifermotor 1
	DDR_GRIPPER1_A |= (1<< PIN_GRIPPER1_A);
	DDR_GRIPPER1_B |= (1<< PIN_GRIPPER1_B);
	DDR_GRIPPER1_EN |= (1<< PIN_GRIPPER1_EN);
	//Greifermotor 2
	DDR_GRIPPER2_A |= (1<< PIN_GRIPPER2_A);
	DDR_GRIPPER2_B |= (1<< PIN_GRIPPER2_B);
	DDR_GRIPPER2_EN |= (1<< PIN_GRIPPER2_EN);
	//Motor 1
	DDR_MOTOR1_A |= (1<< PIN_MOTOR1_A);
	DDR_MOTOR1_B |= (1<< PIN_MOTOR1_B);
	DDR_MOTOR1_EN |= (1<< PIN_MOTOR1_EN);
	//Motor 2
	DDR_MOTOR2_A |= (1<< PIN_MOTOR2_A);
	DDR_MOTOR2_B |= (1<< PIN_MOTOR2_B);
	DDR_MOTOR2_EN |= (1<< PIN_MOTOR2_EN);

	//Batterie Spannung
	DDR_BATTERY_U &= ~(1<< PIN_BATTERY_U);
	//Motorenencoder
	DDR_ENCODER1 &= ~(1<< PIN_ENCODER1);
	DDR_ENCODER2 &= ~(1<< PIN_ENCODER2);

	/*Ausgänge ein-/auschalten*/

	PORT_GRIPPER1_A &= ~(1<< PIN_GRIPPER1_A);
	PORT_GRIPPER1_B &= ~(1<< PIN_GRIPPER1_B);
	PORT_GRIPPER1_EN &= ~(1<< PIN_GRIPPER1_EN);

	PORT_GRIPPER2_A &= ~(1<< PIN_GRIPPER2_A);
	PORT_GRIPPER2_B &= ~(1<< PIN_GRIPPER2_B);
	PORT_GRIPPER2_EN &= ~(1<< PIN_GRIPPER2_EN);

	PORT_MOTOR1_A &= ~(1<< PIN_MOTOR1_A);
	PORT_MOTOR1_B &= ~(1<< PIN_MOTOR1_B);
	PORT_MOTOR1_EN &= ~(1<< PIN_MOTOR1_EN);

	PORT_MOTOR2_A &= ~(1<< PIN_MOTOR2_A);
	PORT_MOTOR2_B &= ~(1<< PIN_MOTOR2_B);
	PORT_MOTOR2_EN &= ~(1<< PIN_MOTOR2_EN);

	/*Eingänge Pullups ein/aus*/

	PORT_BATTERY_U &= ~(1<< PIN_BATTERY_U);
	PORT_ENCODER1 |= (1<< PIN_ENCODER1);
	PORT_ENCODER2 |= (1<< PIN_ENCODER2);


}

//PWM initialisieren
void pwm_init(void)
{ 
	//nicht invertierender pwm modus
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
	//10-bit "phase corrected" pwm
	TCCR1A |= (1 << WGM11) | (1 << WGM10);
	//set prescaler to 8
	TCCR1B |= (1 << CS11);

	//beide Motoren ausschalten
	OCR_MOTOR1 = 0;
	OCR_MOTOR2 = 0;
}

//"Falsche" PWM_Eingaben korrigieren
int16_t correct_pwm(int16_t value)
{
	if(value> MAX_PWM_VALUE)
		return MAX_PWM_VALUE;
	else if(value<(-MAX_PWM_VALUE))
		return -MAX_PWM_VALUE;
	else
		return value;
}

//Motor 1 steurern
//-1023 <= speed <= 1023
//speed 0 => Brake
void motor1_speed (int16_t speed)
{
	speed = correct_pwm(speed);

	if(speed>0)
	{
		PORT_MOTOR1_A &= ~(1<< PIN_MOTOR1_A);
		PORT_MOTOR1_B |= (1<< PIN_MOTOR1_B);
		OCR_MOTOR1 = speed;
	}
	else if(speed==0) //BRAKE
	{
		PORT_MOTOR1_A |= ~(1<< PIN_MOTOR1_A);
		PORT_MOTOR1_B |= (1<< PIN_MOTOR1_B);
		OCR_MOTOR1 = speed;
	}
	else if(speed<0)
	{
		PORT_MOTOR1_A |= (1<< PIN_MOTOR1_A);
		PORT_MOTOR1_B &= ~(1<< PIN_MOTOR1_B);

		OCR_MOTOR1 = speed *-1;
	}
}


//Motor 2 steurern
//-1023 <= speed <= 1023
//speed 0 => Brake
void motor2_speed (int16_t speed)
{
	speed = correct_pwm(speed);

	if(speed>0)
	{
		PORT_MOTOR2_A &= ~(1<< PIN_MOTOR2_A);
		PORT_MOTOR2_B |= (1<< PIN_MOTOR2_B);
		OCR_MOTOR2 = speed;
	}
	else if(speed==0) //BRAKE
	{
		PORT_MOTOR2_A |= (1<< PIN_MOTOR2_A);
		PORT_MOTOR2_B |= (1<< PIN_MOTOR2_B);
		OCR_MOTOR2 = speed;
	}
	else if(speed<0)
	{
		PORT_MOTOR2_A |= (1<< PIN_MOTOR2_A);
		PORT_MOTOR2_B &= ~(1<< PIN_MOTOR2_B);
		OCR_MOTOR2 = speed *-1;
	}
}

//Disconnect motor1 
void motor1_float(void)
{
	PORT_MOTOR1_A &= ~(1<< PIN_MOTOR1_A);
	PORT_MOTOR1_B &= ~(1<< PIN_MOTOR1_B);
	OCR_MOTOR1 = 0;
}

//Disconnect motor2 
void motor2_float(void)
{
	PORT_MOTOR2_A &= ~(1<< PIN_MOTOR2_A);
	PORT_MOTOR2_B &= ~(1<< PIN_MOTOR2_B);
	OCR_MOTOR2 = 0;
}


//read motor commands from i2c-buffer and drive motors
void drive_motors(void)
{
	//get the motor speed values from I2C-Buffer
	//convert 2 bytes to 16bit value
	int16_t _motor1_speed = uniq(i2cdata[0],i2cdata[1]);
	int16_t _motor2_speed = uniq(i2cdata[2],i2cdata[3]);

	//Float Motor?
	if(_motor1_speed == FLOAT)  
		motor1_float();
	else  
		motor1_speed(correct_pwm(_motor1_speed));  //Drive motor1

	//Float Motor?
	if(_motor2_speed == FLOAT)  
		motor2_float();
	else  
		motor2_speed(correct_pwm(_motor2_speed));  //Drive motor2
}



