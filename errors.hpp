#pragma once
#include "pins.hpp"

//Common Error Codes for Software-Hardware Interface

// Error codes
enum class Error_t
{
	Success = 0,
	Counter_Overflow,
	Counter_Underflow,
	Precision,
	Input_Validation
};

/** @breif Contains functions that handle errors of type `Error_t`. */
namespace Errors{
	
	/** @brief Receives an error state and sets up the corresponding LED indicator. [[Incomplete]]*/
	void Validate(Error_t error)
	{
		if(error == Error_t::Success)
			return;
		else if(error == Error_t::Counter_Underflow || Error_t::Counter_Overflow)
			{ digitalWrite(OU_Flow_PIN, HIGH); }
		else if(error == Error_t::Precision)
			{ digitalWrite(PRECISION_PIN, HIGH);}
	}

	/** @brief Receives error value and error threshold and returns an appropriate error state. */
	void Precison_Threshold(double error_val, double error_limit)
	{
		if(error_val <= error_limit)
			{ return Error_t::Success; }
		else
			{ return Error_t::Precision; } 
	}


	template <unsigned int Lin_channels, index_t Series_size, unsigned int Bin_Ratio>
	Error_t Auto_MultiTau_Input_Validator()
	{
		bool input_ok = (Series_size * Lin_channels <= TEENSY_MAX_ALLOCATION);

		input_ok = input_ok && (Bin_Ratio >= 1 &&);

		input_ok =  input_ok && (Series_size % Bin_Ratio == 0); //Multiple

		input_ok = input_ok && (Lin_channels > 0);

		return static_cast<Error_t>
		(
			static_cast<unsigned int>(Error_t::Success) * input_ok +
			!input_ok * static_cast<unsigned int>(Error_t::Input_Validation)
		);
	}
};

/** @brief Custom runtime assert function that sets all the dashboard LEDs to high. */
#ifndef NDEBUG
	
	void _assert_(bool expression) __attribute__((noreturn, flatten))
	{
		if(!expression)
		{
			//Set all LED Pins to High
			LEDSet::set_all();
		}

		//Call abort
		abort();
	}

#elif

	void _assert_(bool expression)
	{
		return;
	}

#endif

