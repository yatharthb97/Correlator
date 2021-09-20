#pragma once
#include <Arduino.h>

#include <algorithm>
#include "pins.hpp"
#include <array> //For std::begin and std::end
#include <initializer_list>


/** @Provides an interface for accessing the LEDs on the dashboar. The object serves primarily as a bookeeping object, and hence, enables state conservation and switching, and automatically manages dimming, and complex routines. */
template <unsigned int SET_SIZE>
class LEDSet
{
public:
	
	enum ledstate_t {OFF = 0, ON = 1, Analog = 2};

	const int LEDs[SET_SIZE] = {SAFE_OUTPUT_DUMP_PIN}; //!< LED pins
	unsigned int State[SET_SIZE]; //!< ON-OFF/Analog states for all LEDs
	bool Error_State = false; //!< Whether the LED Panel is in an error state

	//0
	/** @brief Constructor for LEDSet class. 
	 * @param pins - list of LED pins. */
	LEDSet(std::initializer_list<unsigned int> pins)
	{
		std::copy(std::begin(pins), std::end(pins), const_cast<int*>(LEDs));
		for(unsigned int i = 0; i < this->size(); i++)
		{
			State[i] = false;
		}
		this->state_reload();
	}
	
	//1
	/** @brief Returns the size of the led sets. */
	unsigned int size() __attribute__((always_inline))
	{
		return std::distance(std::begin(LEDs), std::end(LEDs));
	}

	//2
	/** @brief  Sets up the pin mode of all LEDs. */
	inline void init()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			pinMode(LEDs[i], OUTPUT);
		}
		asm volatile ("dsb");
	}

	//3
	/** @brief Sets the LEDs based on the value of the corresponding state variable. This function is useful when the state is altered seperately from the digitalWrite calls. 
	 * The function also calls pinMode() which muxes the pins to gpio.
	 * For State == Analog: The state maintains the analog dim state. */
	inline void state_reload()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			if(State[i] < Analog)
			{
				pinMode(LEDs[i], OUTPUT);
				digitalWrite(LEDs[i], State[i]);
			}
		}
	}

	//4
	/** @brief Sets all  leds with states (ON == 1) and (Analog == 2) to (ON == 1).*/
	inline void ON_todigital()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			if(State[i] != OFF)
			{
				pinMode(LEDs[i], OUTPUT);
				digitalWrite(LEDs[i], State[i]);
				State[i] = ON;
			}
		}
	}

	//5
	/** @brief Toggles the state of all the LEDs. This is a pure digital function and destroys all Analog dim states. */
	inline void toggle_all()
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			State[i] = !(State[i] != OFF);
		}
		this->state_reload();
	}

	//6
	/** @brief Toggles the state of a particular LED. This is a pure digital function and destroys all Analog dim states. */
	inline void toggle_twice(int pin, double time_ms)
	{
		int index = fetch_index(pin);
		if(index != -1)
		{
			pinMode(pin, OUTPUT);
			State[index] = !(State[index] != OFF); //Collapse (ON and ANALOG) to just ON.
			digitalWrite(pin, State[index]);
			
			delay(time_ms);

			State[index] = !(State[index] != OFF); //Collapse (ON and ANALOG) to just ON.
			digitalWrite(pin, State[index]);
		}
		
	}

	//7
	/** @brief Toggles all the LEDs twice, with some delay. 
	 * After the end of the routine, all digital states are conserved.
	 * \property(Animation) */
	inline void toggle_all_routine(int delay_ms)
	{
		toggle_all();
		
		delay(delay_ms);
		
		toggle_all();
	}

	//8
	/** @brief dims a particular valid led pin. */
	inline void dim(int pin, unsigned int analog_val)
	{
		int index = fetch_index(pin);
		if(index != -1)
		{
			analogWrite(pin, analog_val);
			State[index] = Analog;
		}
	}

	//9
	/**@brief Dims the ON-LEDs with the provided analog voltage. Only affects LEDs that are ON or on Analog Mode. */
	inline void dim_all(unsigned int analog_val)
	{
		for(unsigned int i = 1; i < size(); i++)
		{
			if(State[i] != OFF)
			{
				analogWrite(LEDs[i], analog_val);
			}
		}
	}

	//10
	/** @brief Slowly dims the ON-LEDs with a routine that lasts `time_s` seconds.
	 * \property(Animation) */
	inline void dim_all_routine(unsigned int end_analog_val, double time_s)
	{
		int cycles = (time_s * 1000) / 50; //Dimming Resolution is 50 ms.
		int steps = int(double(255 - end_analog_val)/cycles);
		this->dim_all(255); //Max Brightness
		volatile int analog_val = 255;
		delay(50);
		for(int j = 0; j < cycles-1; j++)
		{
			analog_val -= steps;
			dim_all(analog_val);
			delay(50);
		}
		this->dim_all(end_analog_val);
	}

	//11
	/** @brief Sets maximum brightness for all ON-LEDs. */
	inline void max_bright_all()
	{
		this->dim_all(255);
	}

	//12
	/** Sets the minimum brighness for all ON-LEDs. */
	inline void min_bright_all()
	{
		this->dim_all(1);
	}

/////// Digital Set/Unset Functions

	//All LEDs
	//13
	/** @brief Turn on all available LEDs. */
	inline void set_all() __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			pinMode(LEDs[i], OUTPUT);
			digitalWrite(LEDs[i], HIGH);
			State[i] = ON;
		}
	}

	//14
	/** Turn off all available LEDs. */
	inline void unset_all() __attribute__((always_inline))
	{
		for(unsigned int i = 0; i < size(); i++)
		{
			pinMode(LEDs[i], OUTPUT);
			digitalWrite(LEDs[i], LOW);
			State[i] = OFF;
		}
	}

	//Single LEDs
	//15
	/** @brief Turn-ON a particular LED Pin. */
	inline void set(int led_pin) __attribute__((always_inline))
	{
		int index = fetch_index(led_pin);
		if(index != -1)
		{
			pinMode(led_pin, OUTPUT);
			digitalWrite(led_pin, HIGH);
			State[index] = ON;
		}
	}

	//16
	/** @brief Turn-OFF a particular LED Pin. */
	void unset(int led_pin) __attribute__((always_inline))
	{
		int index = fetch_index(led_pin);
		if(index != -1)
		{
			pinMode(led_pin, OUTPUT);
			digitalWrite(led_pin, LOW);
			State[index] = OFF;
		}
	}

	//Two LEDs
	//17
	/** @brief Turn-ON two LED Pins simultaneously. The function only works if both the Pins are valid pins. */
	void set(int led_pin1, int led_pin2) __attribute__((always_inline))
	{
		int index1 = fetch_index(led_pin1);
		int index2 = fetch_index(led_pin2);

		if(index1 != -1 && index2 != -1)
		{
			pinMode(led_pin1, OUTPUT);
			pinMode(led_pin2, OUTPUT);
			digitalWrite(led_pin1, HIGH);
			digitalWrite(led_pin2, HIGH);

			State[index1] = ON;
			State[index2] = ON;

		}
	}

	//18
	/** @brief Turn-OFF two LED Pins simultaneously. */
	void unset(int led_pin1, int led_pin2) __attribute__((always_inline))
	{
		int index1 = fetch_index(led_pin1);
		int index2 = fetch_index(led_pin2);

		if(index1 != -1 && index2 != -1)
		{
			pinMode(led_pin1, OUTPUT);
			pinMode(led_pin2, OUTPUT);

			digitalWrite(led_pin1, LOW);
			digitalWrite(led_pin2, LOW);

			State[index1] = OFF;
			State[index2] = OFF;

		}
	}

	//Three LEDs
	//19
	/** @brief Turn-ON three LED Pins simultaneously. */
	void set(int led_pin1, int led_pin2, int led_pin3) __attribute__((always_inline))
	{	
		int index1 = fetch_index(led_pin1);
		int index2 = fetch_index(led_pin2);
		int index3 = fetch_index(led_pin3);

		if(index1 != -1 && index2 != -1 && index3 != -1)
		{
			pinMode(led_pin1, OUTPUT);
			pinMode(led_pin2, OUTPUT);
			pinMode(led_pin3, OUTPUT);

			digitalWrite(led_pin1, HIGH);
			digitalWrite(led_pin2, HIGH);
			digitalWrite(led_pin3, HIGH);

			State[index1] = ON;
			State[index2] = ON;
			State[index3] = ON;
		}
	}

	//20
	/** @brief Turn-FF three LED Pins simultaneously. */
	void unset(int led_pin1, int led_pin2, int led_pin3) __attribute__((always_inline))
	{	
		int index1 = fetch_index(led_pin1);
		int index2 = fetch_index(led_pin2);
		int index3 = fetch_index(led_pin3);

		if(index1 != -1 && index2 != -1 && index3 != -1)
		{
			pinMode(led_pin1, OUTPUT);
			pinMode(led_pin2, OUTPUT);
			pinMode(led_pin3, OUTPUT);

			digitalWrite(led_pin1, LOW);
			digitalWrite(led_pin2, LOW);
			digitalWrite(led_pin3, LOW);

			State[index1] = OFF;
			State[index2] = OFF;
			State[index3] = OFF;
		}
	}

	//21
	/** @brief Special function that sets a valid pin and sets the flag `LedSet::Error_State` to true. This function also clears all LED states prior to the Error State but does not modify states set after the error state. */
	void inline error(int pin)
	{
		if(!Error_State)
			{ this->unset_all(); }
		
		if(fetch_index(pin) != -1)
		{
			this->Error_State = true;
			this->set(pin);
		}

	}

	//22
	/** @brief Special function that initiates a special routine if the device is in the Error State. It also calls `abort()` , if that feature macro is enabled. */
	void inline assert_errors()
	{
		if(this->Error_State)
		{
			for(unsigned int i = 0; i < 10; i++)
			{
				this->toggle_all_routine(500);
			}

			this->set_all();
			this->unset(LOOP_LED);

			#if ABORT_ON_ERROR == 1
				abort();
			#endif
		}	
	}

	//23
	/** @brief Returns whether the given pin is a "valid pin". A valid pin is a pin that is registered with the LEDSet. */
	bool is_valid_pin(int pin)
	{
		return fetch_index(pin) != -1;
	}

private:
	//24
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

};
