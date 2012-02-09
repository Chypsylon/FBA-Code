

void init_io (void);

void init_adc(void);
uint16_t read_adc( uint8_t channel );
uint16_t read_adc_avg( uint8_t channel, uint8_t average );

void set_line_mux_channel(uint8_t channel);

void set_ir_led(uint8_t led,uint8_t state);
void set_line_led(uint8_t led, uint8_t state);