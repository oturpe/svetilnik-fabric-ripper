// This is the configuration file. Ideally, any configuration and calibration
// of the device should be done by changing this file only.

// Needed by util/delay.h
#define F_CPU 1000000UL

// Delay between two executions of main loop, given in millisecond.
#define LOOP_DELAY 100

// Sets 1 MHz / 256 / 64 = 61 Hz pwm
#define PRESCALER_VALUE Atmega8::PSV_64

// Half length of one complete indicator period. In other words, the time the
// indicator spends on and off. Given in units of LOOP_DELAY.
#define INDICATOR_HALF_PERIOD 20

// Duty cycle of the whole device. Given in units of LOOP_DELAY
// 600 = 1 minute
#define ENABLE_ON_PERIOD 2 * 600
#define ENABLE_OFF_PERIOD 10 * 600

// Speeds and accelerations of motors, in range 0 .. 255 where 255 is the
// maximum.
#define SPOOL_SPEED 0x40
#define SPOOL_ACCELERATION 0x40
#define GUIDE_SPEED 0x80
#define GUIDE_ACCELERATION 0x80

// Duty cycle of guide running motor 2. Given in units of LOOP_DELAY
#define GUIDE_ON_PERIOD 10
#define GUIDE_OFF_PERIOD 30

// Definition of indicator pin data direction and data ports and pins
#define INDICATOR_DATA_DIR DDRB
#define INDICATOR_DATA_DIR_PIN DDB0
#define INDICATOR_DATA PORTB
#define INDICATOR_DATA_PIN PORTB0

// Definitions of switch data direction and data ports and pins
#define MOTOR_1_FORWARD_DATA_DIR DDRB
#define MOTOR_1_FORWARD_DATA_DIR_PIN DDB2
#define MOTOR_1_FORWARD_DATA PORTB
#define MOTOR_1_FORWARD_DATA_PIN PORTB2
#define MOTOR_1_FORWARD_OUTPUT_COMPARE OCR1BL

#define MOTOR_1_REVERSE_DATA_DIR DDRB
#define MOTOR_1_REVERSE_DATA_DIR_PIN DDB1
#define MOTOR_1_REVERSE_DATA PORTB
#define MOTOR_1_REVERSE_DATA_PIN PORTB1
#define MOTOR_1_REVERSE_OUTPUT_COMPARE OCR1AL

#define MOTOR_2_FORWARD_DATA_DIR DDRB
#define MOTOR_2_FORWARD_DATA_DIR_PIN DDB3
#define MOTOR_2_FORWARD_DATA PORTB
#define MOTOR_2_FORWARD_DATA_PIN PORTB3
#define MOTOR_2_FORWARD_OUTPUT_COMPARE OCR2A

#define MOTOR_2_REVERSE_DATA_DIR DDRD
#define MOTOR_2_REVERSE_DATA_DIR_PIN DDD3
#define MOTOR_2_REVERSE_DATA PORTD
#define MOTOR_2_REVERSE_DATA_PIN PORTD3
#define MOTOR_2_REVERSE_OUTPUT_COMPARE OCR2B
