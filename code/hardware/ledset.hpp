#pragma once
#include <Arduino.h>

#include <algorithm>
#include <array> //For std::begin and std::end

#include "pins.hpp"
#define LEDSET_ALLOW_ANALOG  1 //!< Allow or restrict analog (PWM) dimming operations.


/** @Provides an interface for accessing the LEDs on the dashboard and the BUILTIN LED. The object serves primarily as a bookeeping object, and hence, enables state conservation and switching. */
class LEDSet
{
public:
	
	const int LEDs[6] = {LED_BUILTIN, LED_RED, LED_BLUE, LED_YELLOW, LED_WHITE, LED_GREEN}; //!< All color LEDs
	bool LEDStatus[6]; //!< ON-OFF states for all LEDs
	bool Error_State; //!< Whether the LED Panel is in an error state

	/** @brief Constructor for LEDSet class. */
	LEDSet(): Error_State(false)
	{
		for(unsigned int i = 0; i < this->size(); i++)
		{
			LEDStatus[i] = false;
		}
	}
	
	/** @brief Returns the size of the led sets. */
	unsigned int size() __attribute__((always_inline))
	{
		return std::distance(std::begin(LEDs), std::end(LEDs));
	}


	/** @brief  Sets up the pin mode of all LEDs. */
	inline void init()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			pinMode(LEDs[i], OUTPUT);
		}
		asm volatile ("dsb");
	}



	/** @brief Sets the LEDs digitally to max brighnes, based on their switch state. */
	inline void state_reload()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			digitalWrite(LEDs[i], LEDStatus[i]);
		}
	}

	/** @brief Toggles the state of all the LEDs. */
	inline void toggle_all()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			LEDStatus[i] = !LEDStatus[i];
		}
		state_reload();
	}

	/** @brief Toggles all the LEDs twice, with some delay.
	 * \property(Animation) */
	inline void toggle_all_routine(int delay_ms)
	{
		toggle_all();
		delay(delay_ms);
		toggle_all();
	}



#if LEDSET_ALLOW_ANALOG == 1
	/**@brief Dims the ON-LEDs with the provided analog voltage. Only affects LEDs that are ON. */
	inline void dim(unsigned int analog_val)
	{
		for(unsigned int i = 1; i < size(); i++)
		{
			if(LEDStatus[i] == true)
			{
				analogWrite(LEDs[i], analog_val);
			}
		}
		this->digital_mode(); //Set back to digitalmode
	}

	/** @brief Slowly dims the ON-LEDs with a routine that lasts 1s.
	 * \property(Animation) */
	inline void dim_routine(unsigned int end_analog_val, double time_s)
	{
		int cycles = (time_s * 1000) / 50;
		int steps = int(double(255 - end_analog_val)/cycles); //Each dimming cycle will be 50ms, The whole routing is 1s long.
		this->max_brightness();
		int analog_val = 255;
		for(int j = 0; j < cycles-1; j++)
		{
			analog_val -= steps;
			dim(analog_val);
			delay(50);
		}
		dim(end_analog_val);
		//return cycles;
		this->digital_mode(); //Set back to digitalmode

	}

	/** @brief Sets maximum brightness for all ON-LEDs. */
	inline void max_brightness()
	{
		this->dim(255);
		this->digital_mode(); //Set back to digitalmode

	}

	/** Sets the minimum brighness for all ON-LEDs. */
	inline void min_brightness()
	{
		this->dim(1);
		this->digital_mode(); //Set back to digitalmode

	}
#endif

	/** @brief Turn on all available LEDs. */
	inline void set_all() __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			digitalWrite(LEDs[i], HIGH);
			LEDStatus[i] = true;
		}
	}

	/** Turn off all available LEDs. */
	inline void unset_all() __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			digitalWrite(LEDs[i], LOW);
			LEDStatus[i] = false;
		}
	}

	/** @brief Turn-ON a particular LED Pin. */
	inline void set(int led_pin) __attribute__((always_inline))
	{
		int index = fetch_index(led_pin);
		if(index != -1)
		{
			digitalWrite(led_pin, HIGH);
			LEDStatus[index] = true;
		}
	}

	/** @brief Turn-OFF a particular LED Pin. */
	void unset(int led_pin) __attribute__((always_inline))
	{
		int index = fetch_index(led_pin);
		if(index != -1)
		{
			digitalWrite(led_pin, LOW);
			LEDStatus[index] = false;
		}
	}

	/** @brief Turn-ON two LED Pins simultaneously. */
	void set(int led_pin1, int led_pin2) __attribute__((always_inline))
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
	void unset(int led_pin1, int led_pin2) __attribute__((always_inline))
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
	void set(int led_pin1, int led_pin2, int led_pin3) __attribute__((always_inline))
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
	void unset(int led_pin1, int led_pin2, int led_pin3) __attribute__((always_inline))
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

	/** @brief Special function that sets a valid pin and sets the flag `LedSet::Error_State` to true. */
	void error(int pin) __attribute__((always_inline))
	{
		if(fetch_index(pin) != -1)
		{
			this->Error_State = true;
			this->set(pin);
		}

	}


private:
	/** @brief Retrive data structure index based on the pin number. Returns -1 if the pin was not found.
	 * \return Local array index of pin in the LEDs array (bookeeping object). -1 if the search fails.  */
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


	/** @brief  Sets up pin mode OUTPUT on all ON Pins. This function needs to be called after every ANALOG function to mux the pins back to gpio from PWM timers. */
	inline void digital_mode()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			if(LEDStatus[i] == true)
				{ pinMode(LEDs[i], OUTPUT); }
		}
		asm volatile ("dsb");
	}
};



