#pragma once

#include "types.hpp"
#include "Lin_ACorr_RT_Base.hpp"

/** @brief Adapter object responsible for accumulating the points and coarsening the time-series as per the relavent linear-correlator time-resolution. */
class Accumulator
{
	unsigned int ReceivedPointsCounter = 0; //! Maximum number of points accepted by the accumulator. For debugging.
	unsigned int BufferPoints = 0; //! Nmber of points accumulated and binned together.
	
	counter_t Accumulate = 0.0; //Stores the value of the accumulate
	unsigned int LocalCounter = 0; //! Maintains a cunter of the items to accumuate

public:

	/**
	 * @brief Accumulates data points until the BufferPoints criteria is satisfied, after which the points are pushed to the channel. */
	void pipe(Lin_ACorr_RT_Base &channel, counter_t datum) __attribute__((flatten))
	{
		Accumulate += datum;
		LocalCounter--;
		ReceivedPointsCounter++;
				
		if(LocalCounter == 0)
		{
			channel.push_datum(Accumulate); //Push to Channel
			Accumulate = 0; // Reset Sccumulate
			LocalCounter = BufferPoints; //Recharge Local Counter
		}
	}

};