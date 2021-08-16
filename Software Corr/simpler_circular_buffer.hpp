#pragma once


//Todo - Optimize this for use with Teensy
#include <cstddef>

template <typename Type, size_t MaxSize>
class Simpler_Circular_Buffer
{
	
private:

	Type buffer[MaxSize] = {0}; //! Fixed Size Buffer
	size_t head = 0; //! Index / Pointer of the first element of the buffer
	size_t tail = 0; //! Index / Pointer to the last element of the buffer 

public:

	/** @brief Resets the buffer (pointers) without clearing the elements. */
	void reset() __attribute__((always_inline))
	{
		head = 0;
		tail = 0;
	}

	/** @brief Adds a datum to the circular buffer. */
	void push_back(const Type datum) __attribute__((flatten))
	{
		//Advance Pointer
		buffer[tail] = datum;
		tail = (tail + 1 ) % MaxSize;

	}

	/** @brief Random access operator can be used to retrive the nth element in the buffer at any state of the buffer. */
	Type operator[] (const size_t index) const __attribute__((always_inline))
	{	
		return buffer[index % MaxSize];
	}

};