#pragma once
#include "./../types.hpp" 

/**
 * @brief A simple Averager Class that calculates the estimated mean.*/
class Monitor_Channel
{

public:

	counter_t Sample_Stack = 0; //!< Sum of the received values 
	counter_t Sample_Count = 0; //!< Count of the received values

	/** @brief Adds datum. */
	void push_back(const unsigned long data) __attribute__((always_inline))
	{
	 Sample_Stack += data;
	 Sample_Count++;
	}

	/** @brief Returns the estimated mean. */
	void mean() const __attribute__((always_inline))
	{
	 return double(this->Sample_Stack)/ double(this->Sample_Count);
	}

}

/*
Monitor_Channel[Series_size] Mean_monitor;
double norm(index_t lag)
{
	return Mean_monitor[lag].mean() * Mean_monitor[lag].mean();
}*/