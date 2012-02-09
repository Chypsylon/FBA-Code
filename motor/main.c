
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


//Libs
#include "motor_header.h"
#include "motor_lib.h"
#include "twislave.h"


int main(void)
{  
  //_delay_ms(500);	//boot up delay
  io_init();		//set in/outputs, pullups etc.
  pwm_init();		//set pwm-mode, frequency etc.
  init_twi_slave(SLAVE_ADRESSE);	//I2C-Slave
  
  for(uint8_t i=0;i<i2c_buffer_size;i++)  i2cdata[i]=0;  //fill up I2C-Buffer
  
  sei();  //activate global Interrupts
  
  while(1)
  {
    drive_motors();
  }
  
  return 0;
}