#include <avr/io.h>

#include "twislave.h"
#include "line_header.h"


void init_io (void)
{
	/* Set In- Outputs*/
	DDR_IR_LED_OBEN_RECHTS2 |= (1<<PIN_IR_LED_OBEN_RECHTS2);
	DDR_IR_LED_UNTEN_VORNE1 |= (1<<PIN_IR_LED_UNTEN_VORNE1);
	DDR_IR_LED_UNTEN_VORNE2 |= (1<<PIN_IR_LED_UNTEN_VORNE2);
	DDR_IR_LED_OBEN_LINKS1  |= (1<<PIN_IR_LED_OBEN_LINKS1);
	DDR_IR_LED_OBEN_VORNE1  |= (1<<PIN_IR_LED_OBEN_VORNE1);
	DDR_IR_LED_OBEN_LINKS2  |= (1<<PIN_IR_LED_OBEN_LINKS2);
	DDR_IR_LED_OBEN_VORNE2  |= (1<<PIN_IR_LED_OBEN_VORNE2);
	DDR_IR_LED_OBEN_RECHTS1 |= (1<<PIN_IR_LED_OBEN_RECHTS1);

	DDR_LINE_MUX |= (1<<PIN_LINE_MUX_A) | (1<<PIN_LINE_MUX_B) | (1<<PIN_LINE_MUX_C);
	DDR_LINE_MUX &= ~(1<<PIN_LINE_MUX_OUT);

	DDR_LINE_LED |= (1<<PIN_LINE_LED_48) |(1<<PIN_LINE_LED_37) |(1<<PIN_LINE_LED_26) |(1<<PIN_LINE_LED_15);

	/*switch Outputs on/off*/
	PORT_IR_LED_OBEN_RECHTS2 &= ~(1<<PIN_IR_LED_OBEN_RECHTS2);
	PORT_IR_LED_UNTEN_VORNE1 &= ~(1<<PIN_IR_LED_UNTEN_VORNE1);
	PORT_IR_LED_UNTEN_VORNE2 &= ~(1<<PIN_IR_LED_UNTEN_VORNE2);
	PORT_IR_LED_OBEN_LINKS1  &= ~(1<<PIN_IR_LED_OBEN_LINKS1);
	PORT_IR_LED_OBEN_VORNE1  &= ~(1<<PIN_IR_LED_OBEN_VORNE1);
	PORT_IR_LED_OBEN_LINKS2  &= ~(1<<PIN_IR_LED_OBEN_LINKS2);
	PORT_IR_LED_OBEN_VORNE2  &= ~(1<<PIN_IR_LED_OBEN_VORNE2);
	PORT_IR_LED_OBEN_RECHTS1 &= ~(1<<PIN_IR_LED_OBEN_RECHTS1);

	PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_A) | (1<<PIN_LINE_MUX_B) | (1<<PIN_LINE_MUX_C);

	PORT_LINE_LED &= ~(1<<PIN_LINE_LED_48) | (1<<PIN_LINE_LED_37) | (1<<PIN_LINE_LED_26) | (1<<PIN_LINE_LED_15);
	/*(de)activate Pullups*/
	PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_OUT);

}


/*ADC*/

void init_adc(void)
{
	uint16_t result;

	//internal reference
	ADMUX = (1<<REFS1) | (1<<REFS0);
	//prescaler
	ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	//activate ADC
	ADCSRA |= (1<<ADEN);

	//make a dummy measurement
	ADCSRA |= (1<<ADSC);  //single conversion
	while (ADCSRA & (1<<ADSC) ); //wait until finished

	result = ADCW;  //read measurement
}


uint16_t read_adc(uint8_t channel)
{
	//select channel
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
	ADCSRA |= (1<<ADSC);            //single conversion
	while (ADCSRA & (1<<ADSC) );  // wait until finished

	return ADCW;                  // read and return the result
}


uint16_t read_adc_avg( uint8_t channel, uint8_t average )
{
	uint16_t result = 0;

	//make samples
	for(uint8_t i = 0; i < average; ++i )
		result += read_adc(channel);

	return ( result / average );  //calculate average
}


//Activate channel in line sensor multiplexer
void set_line_mux_channel(uint8_t channel)
{
	//extract lower(?) three bits from channel-byte NOT WORKING!
	//PORT_LINE_MUX |= ((channel & (1 << 0))<<PIN_LINE_MUX_A) | ((channel & (1 << 1))<<PIN_LINE_MUX_B) | ((channel & (1 << 2))<<PIN_LINE_MUX_C);

	switch (channel)
	{
		case 0:
		{
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_A);
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_B);
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_C);
			break;
		}
		case 1:
		{
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_A);
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_B);
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_C);
			break;
		}
		case 2:
		{
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_A);
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_B);
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_C);
			break;
		}
		case 3:
		{
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_A);
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_B);
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_C);
			break;
		}
		case 4:
		{
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_A);
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_B);
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_C);
			break;
		}
		case 5:
		{
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_A);
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_B);
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_C);
			break;
		}
		case 6:
		{
			PORT_LINE_MUX &= ~(1<<PIN_LINE_MUX_A);
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_B);
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_C);
			break;
		}
		case 7:
		{
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_A);
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_B);
			PORT_LINE_MUX |= (1<<PIN_LINE_MUX_C);
			break;
		}
	}


}


//Switch specified IR led on/off
void set_ir_led(uint8_t led,uint8_t state)
{
	switch (led)
	{
		case IR_LED_ALL:
		{

			PORT_IR_LED_UNTEN_VORNE1 |= (state << PIN_IR_LED_UNTEN_VORNE1);
			PORT_IR_LED_UNTEN_VORNE2 |= (state << PIN_IR_LED_UNTEN_VORNE2);
			PORT_IR_LED_OBEN_LINKS1  |= (state << PIN_IR_LED_OBEN_LINKS1);
			PORT_IR_LED_OBEN_LINKS2  |= (state << PIN_IR_LED_OBEN_LINKS2);
			PORT_IR_LED_OBEN_VORNE1  |= (state << PIN_IR_LED_OBEN_VORNE1);
			PORT_IR_LED_OBEN_VORNE2  |= (state << PIN_IR_LED_OBEN_VORNE2);
			PORT_IR_LED_OBEN_RECHTS1 |= (state << PIN_IR_LED_OBEN_RECHTS1);
			PORT_IR_LED_OBEN_RECHTS2 |= (state << PIN_IR_LED_OBEN_RECHTS2);
			break;
		}

		case IR_LED_UNTEN_VORNE1:
		{
			PORT_IR_LED_UNTEN_VORNE1 |= (state << PIN_IR_LED_UNTEN_VORNE1);
			break;
		}

		case IR_LED_UNTEN_VORNE2:
		{
			PORT_IR_LED_UNTEN_VORNE2 |= (state << PIN_IR_LED_UNTEN_VORNE2);
			break;
		}

		case IR_LED_OBEN_LINKS1:
		{
			PORT_IR_LED_OBEN_LINKS1 |= (state << PIN_IR_LED_OBEN_LINKS1);
			break;
		}

		case IR_LED_OBEN_LINKS2:
		{
			PORT_IR_LED_OBEN_LINKS2 |= (state << PIN_IR_LED_OBEN_LINKS2);
			break;
		}

		case IR_LED_OBEN_VORNE1:
		{
			PORT_IR_LED_OBEN_VORNE1 |= (state << PIN_IR_LED_OBEN_VORNE1);
			break;
		}

		case IR_LED_OBEN_VORNE2:
		{
			PORT_IR_LED_OBEN_VORNE2 |= (state << PIN_IR_LED_OBEN_VORNE2);
			break;
		}

		case IR_LED_OBEN_RECHTS1:
		{
			PORT_IR_LED_OBEN_RECHTS1 |= (state << PIN_IR_LED_OBEN_RECHTS1);
			break;
		}

		case IR_LED_OBEN_RECHTS2:
		{
			PORT_IR_LED_OBEN_RECHTS2 |= (state << PIN_IR_LED_OBEN_RECHTS2);
			break;
		}
	}
}

//switch line led
void set_line_led(uint8_t led, uint8_t state)
{
	if(state==ON)
	{
		switch (led)
		{
			case LINE_LED_ALL:
			{
				PORT_LINE_LED |= (1 << PIN_LINE_LED_48) | (1 << PIN_LINE_LED_37) | (1 << PIN_LINE_LED_26) | (1 << PIN_LINE_LED_15);
				break;
			}

			case LINE_LED_48:
			{
				PORT_LINE_LED |= (1 << PIN_LINE_LED_48);
				break;
			}

			case LINE_LED_37:
			{
				PORT_LINE_LED |= (1 << PIN_LINE_LED_37);
				break;
			}

			case LINE_LED_26:
			{
				PORT_LINE_LED |= (1 << PIN_LINE_LED_26);
				break;
			}

			case LINE_LED_15:
			{
				PORT_LINE_LED |= (1 << PIN_LINE_LED_15);
				break;
			}
		}
	}
	else
	{
		switch (led)
		{
			case LINE_LED_ALL:
			{
				PORT_LINE_LED &= ~(1 << PIN_LINE_LED_48) & ~(1 << PIN_LINE_LED_37) & ~(1 << PIN_LINE_LED_26) & ~(1 << PIN_LINE_LED_15);
				break;
			}

			case LINE_LED_48:
			{
				PORT_LINE_LED &= ~(1 << PIN_LINE_LED_48);
				break;
			}

			case LINE_LED_37:
			{
				PORT_LINE_LED &= ~(1 << PIN_LINE_LED_37);
				break;
			}

			case LINE_LED_26:
			{
				PORT_LINE_LED &= ~(1 << PIN_LINE_LED_26);
				break;
			}

			case LINE_LED_15:
			{
				PORT_LINE_LED &= ~(1 << PIN_LINE_LED_15);
				break;
			}
		}
	}


}
