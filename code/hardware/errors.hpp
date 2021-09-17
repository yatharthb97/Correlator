/** \brief This file contain the Common Error Codes for Software-Hardware Interface. 
 * It defines the `Error_t` and namespace `Errors` that are used to handle the errors and interact with the LED dashboard.
 * \author Yatharth Bhasin (github → yatharthb97) */

#pragma once
#include "pins.hpp" 
#include "./../types.hpp"
#include "ledpanel.hpp"
#include <Arduino.h>

#define TEENSY_MAX_ALLOCATION 125000 //!< Maximum Allocations of 32-bit integers allowed on the microcontroller Teensy.



/** @brief Enumarates the error codes thrown by the different modules. */
enum class Error_t
{
	Success = 0,      //!< No error
	Counter_Overflow, //!< Passed Value caused a counter overflow
	Counter_Underflow, //!< Passed Value caused a counter underflow
	Precision, 		 //!< Error generated when difference due to finite resolution is greater than acceptable
	Input_Validation //!< Input validation failed
};

/** @breif Contains functions that handle errors of type `Error_t`. */
namespace Errors{
	
	/** @brief Receives an error state and sets up the corresponding LED indicator.
	*/
	void Validate(const Error_t error)
	{
		if(error == Error_t::Success)
			return;
		else if(error == Error_t::Counter_Underflow || error == Error_t::Counter_Overflow)
			{ LEDPanel.error(ER_OVERFLOW_PIN); }
		else if(error == Error_t::Precision)
			{ LEDPanel.error(ER_PRECISION_PIN); }
		else if(error == Error_t::Input_Validation)
			{ LEDPanel.error(ER_INPUT_VALIDATION); }
	}

	/** @brief Receives error value and error threshold and returns an appropriate error state. */
	Error_t Precison_Threshold(double error_val, double error_limit)
	{
		if(error_val <= error_limit)
			{ return Error_t::Success; }
		else
			{ return Error_t::Precision; } 
	}


	/** @brief Input Validator for MultiTau auto-correlator. 
	 * \todo Change name to indicate *Auto* Correlator link. */
	template <unsigned int Lin_channels, index_t Series_size, unsigned int Bin_Ratio>
	Error_t Auto_MultiTau_Input_Validator()
	{
		bool input_ok = (Series_size * Lin_channels <= TEENSY_MAX_ALLOCATION);

		input_ok = input_ok && (Bin_Ratio >= 1);

		input_ok =  input_ok && (Series_size % Bin_Ratio == 0); //Multiple

		input_ok = input_ok && (Lin_channels > 0);

		return static_cast<Error_t>
		(
			static_cast<unsigned int>(Error_t::Success) * input_ok +
			!input_ok * static_cast<unsigned int>(Error_t::Input_Validation)
		);
	}
};

/** \if NDEBUG is not defined: 
 * @brief Custom runtime assert function that sets all the dashboard LEDs to high. */
#ifndef NDEBUG
	
	/** @brief Custom assertion function. 
	 * @param expression which must evaluate to false for the assertion to call abort.
	 * @param message which is ignored in the function,  but helps with code redability*/
	void _assert_(bool expression, const char* __attribute__((unused)) message = "") //__attribute__((noreturn, flatten))
	{
		if(!expression)
		{
			LEDPanel.set_all(); //Set all LED Pins to High
			abort(); //Call abort
		}

	}

#elif /** \else if NDEBUG is defined: */
	/** @brief Empty version that is uded in release mode. */
	void _assert_(bool expression)
	{
		return;
	}

#endif

