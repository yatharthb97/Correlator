#pragma once



//Available LEDs
const int BUILTLIN = LED_BUILTIN;
const int RED = ;
const int BLUE = ;
const int YELLOW = ;
const int WHITE = ;
const int GREEN = ;


//Initalization Pins → Must be unsued pins
const int SAFE_OUTPUT_DUMP_PIN = 0;
const int SAFE_OUTPUT_DUMP_PIN = 0;

//TTL_c Pins
const int TTL_C_Capture_Pin = 10;
const int TTL_C_Output_Pin = 11;
const int TTL_C_Output_Pin = 0;


//Error Pins
const int PRECISION_PIN;
const int OU_Flow_PIN;


// LED State Indicators
const int SETUP_LED 4
const int RUN_LED 5



/** @Provides an interface for accessing the LEDs on the dashboard and the BUILTIN LED. */
namespace LEDSet{

	static int LEDs[6] = {BUILTLIN, RED, BLUE, YELLOW, WHITE, GREEN}; //All color LEDs

	static unsigned int size()
	{
		return 6;
	}


	/** Sets up the pin mode of all LEDs. */
	static void init()
	{
		for(unsigned int i = 0; i < LEDSet::size(); i++)
		{
			pinMode(LEDSet::LEDs, OUTPUT);
		}
	}


	static void set(int led_pin) __attribute__((always_inline))
	{
		digitalWrite(led_pin, HIGH);
	}

	static void unset(int led_pin) __attribute__((always_inline))
	{
		digitalWrite(led_pin, LOW);
	}

	static void set(int led_pin1, int led_pin2) __attribute__((always_inline))
	{
		digitalWrite(led_pin1, HIGH);
		digitalWrite(led_pin2, HIGH);
	}

	static void unset(int led_pin1, int led_pin2) __attribute__((always_inline))
	{
		digitalWrite(led_pin1, LOW);
		digitalWrite(led_pin2, LOW);
	}

	static void set(int led_pin1, int led_pin2, int led_pin3) __attribute__((always_inline))
	{
		digitalWrite(led_pin1, HIGH);
		digitalWrite(led_pin2, HIGH);
		digitalWrite(led_pin3, HIGH);
	}

	static void unset(int led_pin1, int led_pin2, int led_pin3) __attribute__((always_inline))
	{
		digitalWrite(led_pin1, LOW);
		digitalWrite(led_pin2, LOW);
		digitalWrite(led_pin3, LOW);
	}

	static set_all() __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < LEDSet::size(); i++)
		{
			digitalWrite(LEDSet::LEDs[i], HIGH);
		}
	}

	static unset_all() __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < LEDSet::size(); i++)
		{
			digitalWrite(LEDSet::LEDs[i], LOW);
		}
	}

};