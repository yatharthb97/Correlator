#pragma once

#define LEDSET_ALLOW_ANALOG == 1 //! Allow or restrict analog (PWM) dimmin operations.

/** @Provides an interface for accessing the LEDs on the dashboard and the BUILTIN LED. Theobject serves primarily as a bookeeping object, and hence, enables state conservation and switching. */
class LEDSet
{
public:
	
	constexpr static int LEDs[6] = {BUILTLIN, LED_RED, LED_BLUE, LED_YELLOW, LED_WHITE, LED_GREEN}; //All color LEDs
	constexpr static bool LEDStatus[6] = {false};
	
	/** @brief Returns the size of the led sets. */
	static unsigned int size() __attribute__((always_inline))
	{
		return 6;
	}


	/**@brief  Sets up the pin mode of all LEDs. */
	static inline void init()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			pinMode(LEDs[i], OUTPUT);
		}
	}

	/** @brief Sets the LEDs digitally to max brighnes, based on their switch state. */
	static inline void state_reload()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			digitalWrite(LEDs[i], LEDStatus[i]);
		}
	}

	/** @brief Toggles the state of all the LEDs. */
	static inline void toggle_all()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			LEDStatus[i] = !LEDStatus[i];
		}
		state_reload();
	}

	/** @brief Toggles all the LEDs twice, with some delay.
	 * \label Animation */
	static inline toggle_all_routine(int delay_ms)
	{
		toggle_all();
		delay(delay_ms);
		toggle_all();
	}



#if LEDSET_ALLOW_ANALOG == 1
	/**@brief Dims the ON-LEDs with the provided analog voltage. Only affects LEDs that are ON. */
	static inline void dim(unsigned int analog_val)
	{
		for(int i = 1; i < size(); i++)
		{
			if(LEDStatus[i] == true)
			{
				analogWrite(LEDs[i], analog_val);
			}
		}
	}

	/** @brief Slowly dims the ON-LEDs with a routine that lasts 1s.
	 * \label Animation */
	static inline void dim_routine(unsigned int end_analog_val)
	{
		int cycles = 1000/50;
		int steps = int(double(255 - end_analog_val)/cycles); //Each dimming cycle will be 50ms, The whole routing is 1s long.

		int analog_val = 255;
		for(int j = 0; j < cycles-1; j++)
		{
			analog_val -= steps;
			dim(analog_val);
			delay(50);
		}
		dim(end_analog_val);
	}

	/** @brief Sets maximum brightness for all ON-LEDs. */
	static inline void min_brighness()
	{
		this->dim(255);
	}

	/** Sets the minimum brighness for all ON-LEDs. */
	static inline void min_brightness()
	{
		this->dim(1);
	}
#endif


/** \defgroup dig_set_unset "Digital Set-Unset LED functions" */
//{@

	/** @brief Turn on all available LEDs. */
	static set_all() __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			digitalWrite(LEDs[i], HIGH);
			LEDStatus[i] = true;
		}
	}

	/** Turn off all available LEDs. */
	static unset_all() __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			digitalWrite(LEDs[i], LOW);
			LEDStatus[i] = false;
		}
	}

	/** @brief Turn-ON a particular LED Pin. */
	static void set(int led_pin) __attribute__((always_inline))
	{
		int index = fetch_index(led_pin);
		if(index != -1)
		{
			digitalWrite(led_pin, HIGH);
			LEDStatus[index] = true;
		}
	}

	/** @brief Turn-OFF a particular LED Pin. */
	static void unset(int led_pin) __attribute__((always_inline))
	{
		int index = fetch_index(led_pin);
		if(index != -1)
		{
			digitalWrite(led_pin, LOW);
			LEDStatus[index] = false;
		}
	}

	/** @brief Turn-ON two LED Pins simultaneously. */
	static void set(int led_pin1, int led_pin2) __attribute__((always_inline))
	{
		int index1 = fetch_index(led_pin1);
		int index2 = fetch_index(led_pin2);

		if(index1 != -1 && index2 != -1)
		{
			digitalWrite(led_pin1, HIGH);
			digitalWrite(led_pin2, HIGH);

			LEDStatus[index1] = true;
			LEDStatus[index2] = true;

		}
	}

	/** @brief Turn-OFF two LED Pins simultaneously. */
	static void unset(int led_pin1, int led_pin2) __attribute__((always_inline))
	{
		int index1 = fetch_index(led_pin1);
		int index2 = fetch_index(led_pin2);

		if(index1 != -1 && index2 != -1)
		{
			digitalWrite(led_pin1, LOW);
			digitalWrite(led_pin2, LOW);

			LEDStatus[index1] = false;
			LEDStatus[index2] = false;

		}
	}

	/** @brief Turn-ON three LED Pins simultaneously. */
	static void set(int led_pin1, int led_pin2, int led_pin3) __attribute__((always_inline))
	{	
		int index1 = fetch_index(led_pin1);
		int index2 = fetch_index(led_pin2);
		int index3 = fetch_index(led_pin3);

		if(index1 != -1 && index2 != -1 && index3 != -1)
		{
			digitalWrite(led_pin1, HIGH);
			digitalWrite(led_pin2, HIGH);
			digitalWrite(led_pin3, HIGH);

			LEDStatus[index1] = true;
			LEDStatus[index2] = true;
			LEDStatus[index3] = true;
		}
	}

	/** @brief Turn-FF three LED Pins simultaneously. */
	static void unset(int led_pin1, int led_pin2, int led_pin3) __attribute__((always_inline))
	{	
		int index1 = fetch_index(led_pin1);
		int index2 = fetch_index(led_pin2);
		int index3 = fetch_index(led_pin3);

		if(index1 != -1 && index2 != -1 && index3 != -1)
		{
			digitalWrite(led_pin1, LOW);
			digitalWrite(led_pin2, LOW);
			digitalWrite(led_pin3, LOW);

			LEDStatus[index1] = false;
			LEDStatus[index2] = false;
			LEDStatus[index3] = false;
		}
	}

//}@ //"Digital Set-Unset LED functions"

private:
	/** @brief Retrive data structure index based on the pin number*/
	int fetch_index(int pin) __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			if(pin == LEDs[i])
			{
				return i;
			}
		}

		return -1;
	}

};