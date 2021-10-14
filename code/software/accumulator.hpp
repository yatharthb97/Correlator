#pragma once

#include "./../types.hpp"

/** @brief Adapter object responsible for accumulating the points and coarsening
 *  the time-series as per the relavent linear-correlator time-resolution. */
class Accumulator
{	
	counter_t Accumulate = 0; //!< Stores the value of the accumulate
	unsigned int Update_count = 0; //!< Maintains a counter of the items to accumuate

public:
	unsigned int Buffer_count = 0; //!< Number of points accumulated and binned together.
	
	/** @brief Sets the number of points to buffer(accumulate) before pushing to Correlator object. */
	void do_accumulate(unsigned int buffer_cnt)
	{
		Buffer_count = buffer_cnt;
		Update_count = buffer_cnt;
	}

	/**
	 * @brief Accumulates data points until the BufferPoints criteria is satisfied, 
	 * after which the points are pushed to the channel. */
	template <typename LinCorrType>
	void inline pipe(LinCorrType &channel, const counter_t datum)
	{
		Accumulate += datum;
		Update_count--;
				
		if(Update_count == 0)
		{
			channel.push_datum(Accumulate); //Push to Channel
			Accumulate = 0; // Reset Sccumulate
			Update_count = Buffer_count; //Recharge Local Counter
		}
	}

};