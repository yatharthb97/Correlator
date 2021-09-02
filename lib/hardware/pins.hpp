#pragma once


//Available LEDs → Assigned Flex PWM Pins
//Pin 13 → LED_BUILTIN (On Teensy 4.1)
const int LED_RED = 41;
const int LED_BLUE = 42;
const int LED_YELLOW = 22;
const int LED_WHITE = 23;
const int LED_GREEN = 24;


//Initalization Pins → Must be unsued pins
const int SAFE_INPUT_DUMP_PIN = 0;
const int SAFE_OUTPUT_DUMP_PIN = 0;

//TTL_c Pins
const int TTL_C_PULSE_INPUT_PIN = 10; //TMR1CH0


//Error Pins
const int ER_PRECISION_PIN = LED_BLUE;
const int ER_OVERFLOW_PIN = LED_RED;
const int ER_INPUT_VALIDATION = LED_YELLOW;


// LED State Indicators
const int SETUP_LED = LED_BUILTIN;
const int LOOP_LED = LED_GREEN;


/*
FlexPWM1 Module0 - Controls PWM pins 1, 44, 45.
FlexPWM1 Module1 - Controls PWM pins 0, 42, 43.
FlexPWM1 Module2 - Controls PWM pins 24, 46, 47.
FlexPWM1 Module3 - Controls PWM pins 7, 8, 25.
FlexPWM2 Module0 - Controls PWM pins 4, 33.
FlexPWM2 Module1 - Controls PWM pin 5.
FlexPWM2 Module2 - Controls PWM pins 6, 9.
FlexPWM2 Module3 - Controls PWM pins 36, 37.
FlexPWM3 Module0 - Controls PWM pin 53.
FlexPWM3 Module1 - Controls PWM pins 28, 29.
FlexPWM3 Module2 - No pins accessible.
FlexPWM3 Module3 - Controls PWM pin 41.
FlexPWM4 Module0 - Controls PWM pin 22.
FlexPWM4 Module1 - Controls PWM pin 23.
FlexPWM4 Module2 - Controls PWM pins 2, 3.
FlexPWM4 Module3 - No pins accessible.*/