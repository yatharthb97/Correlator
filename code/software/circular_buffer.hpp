#pragma once

/** \attention{Compilation - Not-OK, Tests - Incomplete}*/

/** @brief Implementation of simple Serial Buffer with bounded index random accessor. */
template <typename Type, size_t MaxSize>
class Circular_Buffer
{
	
private:

	Type buffer[MaxSize] = {0}; //! Fixed Size Buffer
	size_t head; //! Index / Pointer of the first element of the buffer
	size_t tail; //! Index / Pointer to the last element of the buffer 
	bool is_full_flag; //! Flag indicates whether the buffer is running at full capacity


	/** @brief Adjusts the pointer after addition or push_back() operation. */
	void advance_pointer() __attribute__((always_inline))
	{
		if(is_full_flag)
		{
			if (++tail == MaxSize) 
			{ tail = 0; }
		}

		if (++head == Max) 
		{ head = 0; }

		is_full_flag = (head == tail);
	}

	/** @brief Adjusts the pointers after a removal or pop_back() operation. */
	void retreat_pointer() __attribute__((always_inline))
	{
		is_full_flag = false;
		if (++tail == MaxSize) 
		{ tail = 0; }
	}

public:

	/** @brief Resets the buffer (pointers) without clearing the elements. */
	void reset() __attribute__((always_inline))
	{
		head = 0;
		tail = 0;
		is_full_flag = false;
	}

	/** @brief Returns whether the buffer is full. */
	bool is_full() const __attribute__((always_inline))
	{
		return is_fill_flag;
	}

	/** @brief Returns whether the buffer is empty. \note{!is_full() != is_empty()}*/
	bool is_empty() const __attribute__((always_inline))
	{
		return ( head==tail && !is_full_flag );
	}

	/** @brief Returns the maximum capacity of the buffer. */
	size_t capacity() const __attribute__((always_inline))
	{
		return MaxSize;
	}

	/** @brief Returns the current size of the buffer. */
	size_t size() const __attribute__((always_inline))
	{
		if(is_full_flag)
			{ return MaxSize; }
		else if (head >= tail)
			{ return (head - tail); }
		else
			{ return MaxSize + (head - tail); }
	}


	/** @brief Adds a datum to the circular buffer. */
	void push_back(const Type data) __attribute__((flatten))
	{
		this->buffer[head] = data;
		this->advance_pointer();
	}

	/** @brief Returns the last element from the buffer and deletes it - pop operation. */
	Type pop_back() __attribute__((always_inline))
	{
		if(!is_empty())
		{
			Type temp =  buffer[tail];
			retreat_pointer();
			return temp;
		}
		else
			return Type();
	}

	/** @brief Random access operator can be used to retrive the nth element in the buffer at any state of the buffer. */
	Type operator[] (const size_t index) const __attribute__((always_inline))
	{
		assert(index < MaxSize);
		size_t Remaining = MaxSize - head; //Indices servicible by adding to head pointer.
		return (Remaining > index) buffer[head + index] + !(Remaining > index) * (buffer[index - Remaining]);
	}

};