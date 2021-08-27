#pragma once

/** @brief Interface for using the "Life Time TImer" functionality of the Periodic Interrut Timer on Teensy 4.x microcontrollers. The module uses Channel 0 and 1 of the 4 PIT channels.
 * \attention The module has no checks on the use of CH0 and CH1 by external drivers, however, the interface enforces a singleton template and can only be constructed once. 
 * \attention The module does not set the clock frequency and uses either the default or the one set globally (as is the only use case) by external drivers outside its scope. 
 * \reference : Code adapted from IMXRT Manual Page 2977. */
class PIT_LifetimeTimer
{
	
private:
	static constexpr bool singleton_flag = false; //! Enforces a singleton template
public:

	//0 - Constructor
	PIT_LifetimeTimer()
	{
		static_assert(singleton_flag, "PIT_LifetimeTimer : The resource is already constructed.");
		singleton_flag  = true;
	}


	//1
	/** @brief Sets up Timer 0 and Timer 1 channels for  Lifetime timing - chained configuration. This function also makes a call to enable PIT timers. The timers are aet at maximum values and will start downcounting once ::start() method is called.
	 * \reference : Code adapted from IMXRT Manual Page 2977. 
	 * \attention : The timers will operate at the clock frequency set for PIT timers outside this library. */
	void init() __attribute__((always_inline))
	{
		// Enable on PIT 
		PIT_MCR = 0x00;
		
		//Timer 1
		PIT_LDVAL1 = 0xFFFFFFFF; // setup timer 1 for maximum counting period 
		PIT_TCTRL1 = 0x0; // disable timer 1 interrupts 
		PIT_TCTRL1 |= CHN; // chain timer 1 to timer 0 

		// Timer 0
		PIT_LDVAL0 = 0xFFFFFFFF; // setup timer 0 for maximum counting period 
		PIT_TCTRL0 = 0x0; // disable timer 0 interrupts  // Attention: This might be incorrect

	}

	//2
	/** @brief Starts the timers for down counting. */
	void start() __attribute__((always_inline))
	{
		PIT_TCTRL1 |= TEN; // start timer 1
		PIT_TCTRL0 |= TEN; // start timer 0
	}

	//3
	/** @brief Stops the timers from counting. The timers values will freeze. */
	void stop() __attribute__((always_inline))
	{
		PIT_TCTRL0 &= ~TEN; // stop timer 0
		PIT_TCTRL1 &= ~TEN; // stop timer 1
	}

	//4
	/** @brief Stops the timers and resets the timer back to the init value (max) - 0xFFFFFFFFFFFFFF. */
	void reset() __attribute__((always_inline))
	{	
		this->stop();
		PIT_LDVAL1 = 0xFFFFFFFF;
		PIT_LDVAL0 = 0xFFFFFFFF;
	}

	//5
	/** @brief Returns the complete 64-bit timing value. */
	uint64_t read_val() __attribute__((always_inline))
	{
		//To access the lifetime, read first LTMR64H and then LTMR64L.
		uint64_t current_uptime = (PIT_LTMR64H << 32); //Bit shift Left 32 bits
		current_uptime = current_uptime + PIT_LTMR64L;
		return current_uptime;
	}

	//6
	/** @brief Returns the lower 32-bit half of the 64-bit timing value. */
	uint32_t read_low_val() __attribute__((always_inline))
	{
		return PIT_LTMR64L;
	}


	//7
	/** @brief Returns the higher 32-bit half of the 64-bit timing value. */
	uint32_t read_high_val() __attribute__((always_inline))
	{
		PIT_LTMR64H;
	}


	//8
	/** @brief Returns the elapsed duration from the start call of the timer. The return is the complete 64bit value. */
	uint64_t elapsed64() __attribute__((always_inline))
	{
		return 0xFFFFFFFFFFFFFFFF - read_val();
	}


	//9
	/** @brief Returns the elapsed duration from the start call of the timer. The return is the lower 32-bit half elapsed time value.
	 * \warning If the elapsed duration is greater than the 32-bit rollover time, then the quantity returned is meaningless. Hence, use of this function should be reserved for measuring small time durarions. */
	uint32_t elapsed32() __attribute__((always_inline))
	{
		return 0xFFFFFFFF - PIT_LTMR64L;
	}

}; //End of class PIT_LifetimeTimer