/*INCLUDES*/

//Standard C-Libs
#include <stdlib.h>
#include <stdint.h>

//Standard AVR-Libs
#include <util/twi.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>


//Libs
#include "line_header.h"
#include "line_lib.h"
#include "twislave.h"



int main(void)
{
	/*Variables*/
	uint16_t ambient_light = 0, light_value = 0;

	struct light_sensor
	{
		uint16_t cal_min;
		uint16_t cal_max;
	} light_sensors[NUM_SENSORS];


	init_io();  //Set up IO-Ports
	init_adc(); //Set up ADC
	init_twi_slave(SLAVE_ADRESSE);	//I2C-Slave

	set_line_led(LINE_LED_ALL,ON);

	for(uint8_t i=0;i<buffer_size;i++)
	{
		//fill up I2C-Buffers
		rxbuffer[i]=0;
		txbuffer[i]=0;
	}

	for(uint8_t i=0;i<NUM_SENSORS;i++)
	{
		light_sensors[i].cal_min = 1023;
		light_sensors[i].cal_max = 0;
	}

	sei();  //activate global interrupts

	//TODO: read calibration min max from eeprom

	while(1)
	{
		/*Calibration*/
		while(rxbuffer[LINE_SENSOR_CALIBRATION_COMMAND]==1)  //start looking for calibration values
		{
			for(uint8_t i=0;i<NUM_SENSORS;i++)  //get max. and min. values for each sensor
			{
				set_line_mux_channel(i);
				_delay_ms(10);  //allow multiplexer enough time to switch channel, can probably be reduced/omitted

				//look for max. and min. value and store
				light_value = read_adc_avg(0,10);

				if(light_value>light_sensors[i].cal_max)
					light_sensors[i].cal_max = light_value;

				if(light_value<light_sensors[i].cal_min)
					light_sensors[i].cal_min = light_value;
			}
		}

		if(rxbuffer[LINE_SENSOR_CALIBRATION_COMMAND]==2)  //save calibration values to eeprom
		{
			//TODO
			rxbuffer[LINE_SENSOR_CALIBRATION_COMMAND]=0;
		}

		if(rxbuffer[LINE_SENSOR_CALIBRATION_COMMAND]==3)  //prepare min. calibration values for transmission
		{
			//fill txbuffer with min. calibration values
			for(uint8_t i=0;i<NUM_SENSORS;i++)
			{
				txbuffer[2*i] = LOW_BYTE(light_sensors[i].cal_min);
				txbuffer[2*i+1] = HIGH_BYTE(light_sensors[i].cal_min);
			}
			rxbuffer[LINE_SENSOR_CALIBRATION_COMMAND]=0;
		}

		if(rxbuffer[LINE_SENSOR_CALIBRATION_COMMAND]==4)  //prepare max. calibration values for transmission
		{
			//fill txbuffer with max. calibration values
			for(uint8_t i=0;i<NUM_SENSORS;i++)
			{
				txbuffer[2*i] = LOW_BYTE(light_sensors[i].cal_max);
				txbuffer[2*i+1] = HIGH_BYTE(light_sensors[i].cal_max);
			}
			rxbuffer[LINE_SENSOR_CALIBRATION_COMMAND]=0;
		}

		/*Calibration END*/

		/*Sampling and processing*/

		if(rxbuffer[LINE_SENSOR_COMMAND]!=0) //0 -> no sampling
		{
			for(uint8_t i=0; i<NUM_SENSORS; i++) //sweep through sensors
			{
				set_line_mux_channel(i);
				_delay_ms(10); //TODO: reduce/omit delay

				if(rxbuffer[LINE_SENSOR_COMMAND]==10) //no calibration
				{
					/*switch on respective leds*/
					if((i==0)||(i==4))
						set_line_led(LINE_LED_15,ON);

					if((i==1)||(i==5))
						set_line_led(LINE_LED_26,ON);

					if((i==2)||(i==6))
						set_line_led(LINE_LED_37,ON);

					if((i==3)||(i==7))
						set_line_led(LINE_LED_48,ON);

					light_value = read_adc_avg(0,10);
				}
				else if(rxbuffer[LINE_SENSOR_COMMAND]==11)  //sample with ambient light filtering
				{
					ambient_light = read_adc_avg(0,10);  //take measurement without led activated -> get ambient light

					/*switch on respective leds*/
					if((i==0)||(i==4))
						set_line_led(LINE_LED_15,ON);

					if((i==1)||(i==5))
						set_line_led(LINE_LED_26,ON);

					if((i==2)||(i==6))
						set_line_led(LINE_LED_37,ON);

					if((i==3)||(i==7))
						set_line_led(LINE_LED_48,ON);

					//_delay_ms(10);

					light_value = read_adc_avg(0,10);

					if(ambient_light < light_value)
						light_value=0;  //prevent underflow
					else
						light_value = ambient_light - light_value;  //eliminate ambient light
				}
				else if(rxbuffer[LINE_SENSOR_COMMAND]==12)  //normalize values
				{
					/*switch on respective leds*/
					if((i==0)||(i==4))
						set_line_led(LINE_LED_15,ON);

					if((i==1)||(i==5))
						set_line_led(LINE_LED_26,ON);

					if((i==2)||(i==6))
						set_line_led(LINE_LED_37,ON);

					if((i==3)||(i==7))
						set_line_led(LINE_LED_48,ON);

					light_value = read_adc_avg(0,10);

					int16_t temp;

					temp = ((light_value - light_sensors[i].cal_min) * 1023 )/ ( light_sensors[i].cal_max - light_sensors[i].cal_min );  //normalize line values

					if(temp<0)  //prevent underflow
						temp=0;

					if(temp>1023)  //prevent overflow
						temp=1023;

					light_value = temp;
				}

				//fill  transmission buffer with light values
				txbuffer[2*i] = LOW_BYTE(light_value);
				txbuffer[2*i+1] = HIGH_BYTE(light_value);

				set_line_led(LINE_LED_ALL,OFF);
			}
		}
		/*Sampling and processing END*/

		set_ir_led(rxbuffer[17],rxbuffer[18]);  //switch Distance IR-Leds
	}

	return 0;
}
