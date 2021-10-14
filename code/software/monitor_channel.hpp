#pragma once
#include <cstdint>
#include <cmath>
#include "./../types.hpp"

/**
 * @brief A simple Averager Class that calculates the estimated mean. Template parameter `Construct` specifies a template specialization that either constructs a functional object when it is `true` or constructs a near zerp-signature **dummy object** when set to `false`.*/
template <bool MeanChannel=true>
class MonitorChannel;

/** @brief Template specialization */
template<>
class MonitorChannel<true>
{
using AccumulateType = float;
public:

	volatile AccumulateType Accumulate = 0.0; //!< Sum of the received values
	volatile uint32_t Count = 0; //!< Count of the received values (Initialized to prevent DivideByZero error)

	/** @brief Adds datum to the channel. */
	template<typename DataType>
	void push_back(DataType data)// __attribute__((always_inline))
	{
		Accumulate += (float_t)data;
		Count++; 
		//Expected Compiler optimization for integers → ++.
	}

	/** @brief Returns the estimated mean. */
	float_t mean() const __attribute__((always_inline))
	{
		
		return float_t(this->Accumulate)/ float_t(this->Count);
	}

	/** @brief Returns the `Accumulate` so far. */
	AccumulateType accumulate() const __attribute__((always_inline))
	{
		return Accumulate;
	}

	void reset()
	{
		Accumulate = 0.0;
		Count = 0;
	}

};

/** @brief Specialization -> Counter Monitor (degenerated mean monitor).*/
template <>
class MonitorChannel<false>
{
public:
	using AccumulateType = uint32_t;
	volatile uint32_t Count = 0; 
	//!< Count of the received values (Initialized to prevent DivideByZero error)

	/** Increments the `Count`.*/
	void push_back() __attribute__((always_inline))
	{
		Count++;
	}

	/** Returns a mean value of 1.0. */
	float_t mean() const __attribute__((always_inline))
	{
		return 1.0;
	}

	/** @brief Returns the accumulate so far, which is the same as 'Count`. */
	AccumulateType accumulate() const __attribute__((always_inline))
	{
		return Count;
	}

	void reset()
	{
		Count = 0;
	}
};


/** @brief Measures the zeroth auto-correlation, which is equal to the second moment of the sampled time series. 
 * This object was intended for experimentation with Linear-autocorrelators, which start from lag1, instead of lag0. */
class ZeroLagMonitor
{
public:
	float_t Second_Moment = 0;

	void push_back(counter_t datum) __attribute__((always_inline))
	{
		Second_Moment += datum * datum;
	}

	float_t get_output() __attribute__((always_inline))
	{
		return Second_Moment;
	}
};




/** @brief An object that can accumulate data into a fixed size bin before changing its output value. */
class RTCoarseGrainer
{
public:
	float Accumulate = 0.0; //!< Accumulate of the data
	float Out = 0.0; //!< Last Ready Coarse Grained Value
	uint32_t UpdateCntr = 0; //!< Local counter that keep tracks of the updates made
	const uint32_t CGFactor; //!< Number of points that are coarse-grained together
	float Error = 0.0; //!< Error in the coarsening interval due to discretization

	/** @brief Constructor that accepts a `coarsening_interval` and the `minimum_resolution` of data and assumes 
	 * the coarse graining factor (`CGFactor`) as the  `coarsening_interval` divide by `minimum_resolution`. */
	constexpr RTCoarseGrainer(float coarsing_interval, float minimum_resolution): 
							  CGFactor(uint32_t(std::floor(coarsing_interval / minimum_resolution))), Error(coarsing_interval - CGFactor)
	{}

	/** @brief Constructor that accepts a `coarsening_interval` and the `minimum_resolution` of data and assumes 
	 * the coarse graining factor (`CGFactor`) as the  `coarsening_interval` divide by `minimum_resolution`. */
	constexpr RTCoarseGrainer(float coarsing_interval): CGFactor(uint32_t(std::floor(coarsing_interval))), Error(coarsing_interval - CGFactor)
	{}

	/** @brief Updates the `RTCoarseGrainer` object with the passed datum.*/
	template <typename DataType>
	void push_back(const DataType datum)
	{
		Accumulate += (datum);
		UpdateCntr++;
		if(UpdateCntr >= CGFactor)
		{
			Out = Accumulate;
			Accumulate = 0.0;
			UpdateCntr = 0;
		}
	}

	/** @brief Returns the last available coarse-grained value. */
	float inline output() const
	{
		return Out;
	}

	/** @brief Returns the discretization error. */
	float inline error() const
	{
		return Error;
	}
};
