#pragma once


//Common Error Codes for Software-Hardware Interface

// Error codes
enum class Error_t
{
	Success = 0,
	Counter_Overflow,
	Counter_Underflow
};

namespace Errors{
	void Validate(Error_t error)
	{
		if(error == Error_t::Success)
			return;
		else if(error == Error_t::Counter_Underflow || Error_t::Counter_Overflow)
			{ digitalWrite(OU_Flow_PIN, HIGH); }
		else if(error == Error_t::Precision)
			{ digitalWrite(PRECISION_PIN, HIGH);}
	}

	void Precison_Threshold(double error_val, double error_limit)
	{
		if(error_val <= error_limit)
			{ return Error_t::Success; }
		else
			{ return Error_t::Precision; } 
	}
}