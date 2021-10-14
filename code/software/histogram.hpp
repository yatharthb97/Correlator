#pragma once
#include<type_traits>

#include "./../types.hpp"
#ifdef CORR_SIMULATOR
	#include "pseudoSerial.hpp" //→ Add for testing
#else
	#include <Arduino.h>
#endif

/** @brief Photon Counting Histogram module for Real time calculation on *Teensy 4.1 microcontrollers*. */


/** @brief Describes a real time histogram function that is updated point by point. *The Bin index starts from zero.*
 * The histogram's lasrt bin, which is `Bins + 1`th bin is the overflow bin and includes all points above the maximum bin  size.
 * The histogram bin is left centred, i.e., the bin contains all numbers lower or equal to the Bin value.
 * \verbatim
 * [Bin 0][Bin 1][Bin 2][Bin 3][Bin 4]...[Bin Max][•|Bin Overflow|•]
 * \endverbatim
 * */
template <typename BinType, unsigned int Bins>
class PCHistogram
{
	unsigned int Index; //!< Index used by the pushback function
public:
	BinType Histogram[Bins+1] = {0}; //!< Histogram function array

	/** @brief Returns the total number of buns, including the overflow bin.*/
	constexpr unsigned int size() __attribute__((always_inline))
	{
		return(Bins + 1);
	}

	/** @brief Receives a single datum and processes it to into the Histogram. */
	template <typename DataType>
	void inline push_back(DataType datum)
	{
		constexpr unsigned int MaxIndex = Bins-1;
		Index = (datum > MaxIndex) * (Bins) +
				!(datum > MaxIndex) * (datum);
		
		Histogram[Index]++;
	}

	/** @brief Sends the complete Histogram to the Serial output. */
	void output() __attribute__((always_inline))
	{
		Serial.write((char8cast_t*)(Histogram), sizeof(BinType)*(Bins+1));
	}
};