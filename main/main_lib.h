#ifndef MAIN_LIB_H
#define MAIN_LIB_H

void io_init (void);

void lcd_bl (uint8_t state);

void status_led (uint8_t state);

void trig_us(void);

/*IR Sensors*/
void ir_pulse(uint8_t state);
uint8_t send_ir_led(uint8_t led, uint8_t state);

/*Line Slave*/
uint8_t send_line_mode(uint8_t mode);
uint16_t read_line_sensor(uint8_t sensor);
uint8_t send_calibration_mode(uint8_t mode);

/*MOTOR CONTROL*/
uint8_t send_motor12_speed(int16_t motor1_speed, int16_t motor2_speed);
uint8_t send_motor1_speed(int16_t motor1_speed);
uint8_t send_motor2_speed(int16_t motor2_speed);

uint8_t send_motor1_float(void);
uint8_t send_motor2_float(void);

/*INT TO STRING*/
void uart_put_int(int8_t var);
void uart_put_uint(uint8_t var);
void uart_put_u16bit( uint16_t value );
void uart_put_16bit( int16_t value );

/*LCD*/
//Displays a variable at cursor position
void lcd_put_int8(int8_t var);
void lcd_put_uint8(uint8_t var);
void lcd_put_int16(int16_t var);
void lcd_put_uint16(uint16_t var);


/*ENCODER*/
void encode_init(void);
int8_t encode_read4(void);

#endif //MAIN_LIB_H
