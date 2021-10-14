#pragma once
#include <type_traits>
#include "./../software/monitor_channel.hpp"


#define INTERARRIVALTIME_MINMAX_STATISTICS 0
template <typename CounterType, typename CPUTickType>
class InterArrivalTime
{
private:
	CounterType get_counter() __attribute__((always_inline))
	{
		return Cntr;
	}

public:
	CounterType* Cntr; //!< Saves the address of the counter location
	CounterType CountDiff = 0; //!< The difference between the arrival time of last two photons
	CounterType LastCntr = 0; //!< Last Counter value recorded
	CounterType Now = 0; //!< Current Counter value storage
	MonitorChannel<true> Mean; //!< Mean Inter-arrival time

	#if INTERARRIVALTIME_MINMAX_STATISTICS == 1
		CounterType Max = 0; //!< Maximum inter-arrival time recorded so far.
		CounterType Min = 100; //!< Minimum inter-arrival time recorded so far. Init to '100' to avoid deadlock at '0'.
	#endif


	/** @brief Constructor that accepts the counter location, that it frequently samples.
	 * It only accepts `unsigned int` types. */
	constexpr InterArrivalTime(const CounterType* cntr_location): Cntr(cntr_location)
	{
		static_assert(std::is_unsigned<CounterType>::value, "The CounterType must be an unsigned integer type.");

	}

	/** @brief Measures the difference between two *observable* counts. If the mean is closer to 1,
	 *  the pulse inter-arrival time will be implemented later using the CPU cycle count. */
	void measure() __attribute__((flatten))
	{
		
		Now = get_counter();
		CountDiff = Now - LastCntr;
		
		if(CountDiff  > 0)
		{
			Mean.push_back(CountDiff);

			#if INTERARRIVALTIME_MINMAX_STATISTICS == 1
				//if(CountDiff < Min && CountDiff != 0) {Min = CountDiff;}
				//if (CountDiff > Max) {Max = CountDiff;}
			#endif

			LastCntr = Now;
		}
	}

	/** @brief Outputs the calculated statistics as a binary struct. */
	void output() __attribute__((always_inline))
	{
		float_t mean = Mean.mean();
		Serial.write((uint8_t*)&(mean),  sizeof(float_t));
		//Serial.send_now();

		#if INTERARRIVALTIME_MINMAX_STATISTICS == 1
			//Serial.write((uint8_t*)&(Min), constexpr sizeof(CounterType));
			//Serial.write((uint8_t*)&(Max), constexpr sizeof(CounterType));
		#endif
	}
};