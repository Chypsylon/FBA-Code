

void io_init (void);
void pwm_init(void);

int16_t correct_pwm(int16_t value);

void motor1_speed (int16_t value);
void motor2_speed (int16_t value);

void motor1_float(void);
void motor2_float(void);

void drive_motors(void);