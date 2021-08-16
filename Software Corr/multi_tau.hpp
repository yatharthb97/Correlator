#pragma once
 
#include <cmath>
#include "types.hpp"
#include "Lin_ACorr_RT_Base.hpp"
#include "Lin_ACorr_RT_Teensy.hpp"
#include "accumulator.hpp"
#include "discarder.hpp"


/** @brief MultiTau Auto-Correlator object that is composed of multiple linear - autocorrelators. Specialised for teensy. */
//using Lin_Corr_t = Lin_ACorr_RT_Teensy;		         //→ S        //→P                  //→m
template <unsigned int Lin_channels, index_t Series_size, unsigned int Bin_Ratio>
class MultiTau_ACorr_RT_Teensy
{

private:

	Accumulator Accumulators[Lin_channels]; //! Accumulator Objects for each channel
	Lin_ACorr_RT_Teensy<Series_size> Lin_Corrs[Lin_channels]; //! Linear ACorrelators
	Serial_Discarder_Teensy<Series_size, Bin_Ratio, 0> Discarder; //! Discarder that discards first #Bin_Ration points
	
public:

	uint32_t DataCounter = 0; //! Counts the total number of data points sent to the counter.

	MultiTau_ACorr_RT_Teensy()
	{
		//Initalize the Accumulators
		for(unsigned int s = 0; s < Lin_channels; s++)
		{
			Accumulators[s].BufferPoints = tau_scaling_scheme(s);
		}
	}

	void inline push_datum(counter_t datum)  __attribute__((flatten))
	{
		DataCounter++;

		#pragma unroll Lin_channels
		for(unsigned int i = 0; i < Lin_channels; i++)
		{
			Accumulators[i].pipe(Lin_Corrs[i], datum);
		}
		
	}


	void push_data(const counter_t *container, const index_t size) __attribute__((flatten))
	{
		for(unsigned int i = 0; i < size; i++)
		{
			this->push_datum(container[i]);
		}
	}

	void ch_out() const __attribute__((flatten))
	{
		Lin_Corrs[0].ch_out(); //Output without discarding

		//Output with discarding for all subsequent correlators
		for(unsigned int s = 1; s < Lin_channels; s++)
		{
			Discarder.output(Lin_Corrs[s]);
		}
	}

	/**
	 * @Brief Returns the number of data points after which, the timescale is scaled according to the MultiTau_ACorr_RT::tau_scaling_scheme(). */
	index_t time_scaling_factor() const __attribute__((always_inline))
	{
		return (Series_size - Bin_Ratio);
	}

	/** @brief Returns the relavent Tau scaling factor, based on the specialised scheme. */
	index_t tau_scaling_scheme(unsigned int s) const __attribute__((always_inline))
	{
		return pow(Bin_Ratio, s);
	}

};





