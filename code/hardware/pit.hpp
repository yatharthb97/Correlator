#pragma once
#include "errors.hpp"


/** @brief Interface for PIT timers on Teensy 4.x microcontrollers. */
template <unsigned int ChID>
class PITController
{

public:

	//Period values are in microseconds (us)
	double Actual_period = 0; //!< Actual Time Period set by the device due to finite resolution
  	double Req_period = 0; //!< Time Period requested by the user.

	uint32_t Clk_freq = uint32_t(24 * 1e6); //!< Clock frequency used by the timer. Initalized to 24MHz → default oscillator clock 
	uint32_t LoadVal = 0; //!< The value loaded in to the counter during after overflow.
	const static constexpr uint32_t PIT_MAX_COUNTER = 4294967295; //!< Constant - Maximum possible counter value. 32 bit counter.

PITController()
{ static_assert(ChID <=3, "No such PIT channel exists!"); }

	/** @brief Enable all PIT channels. */
	void static enable_PITs() __attribute__((always_inline))
	{
		PIT_MCR = 0x00; //Write 0 is ON
	} 

	/** @brief Disable all PIT Channels. */
	void static disable_PITs() __attribute__((always_inline))
	{
		PIT_MCR = 0x01; //Write 1 is OFF
	}

	/** @brief Pause/Resume - \b Toggle \b all \b PIT \b Channels. */
	void static pause_resume_PITs() __attribute__((always_inline))
	{
		PIT_MCR ^= PIT_MCR_FRZ;
	}


	/** @brief Start counting on the timer. */
	void start() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TCTRL |= PIT_TCTRL_TEN; //Set BIT
	}

	/** @brief stop counting on the timer. */
	void  stop() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TCTRL &= ~PIT_TCTRL_TEN; //Unset BIT
	}

	/**
	 * @brief Sets the gate time after which the counter returns to zero and marks the end of onegate interval. 
	 * \attention In case of an error, the gate time is set to the maximum. 
	 * \attention The clock source must be selected before calling this function. */
	Error_t set_gate_time(double gt_microseconds) __attribute__((flatten))
	{
		double ldval_ = gt_microseconds * this->Clk_freq * 1e-6;
		uint32_t ldval = ceil(ldval_);

    	this->Req_period = gt_microseconds;
		
		if(ldval > PIT_MAX_COUNTER)
		{
			this->LoadVal = PIT_MAX_COUNTER;
			IMXRT_PIT_CHANNELS[ChID].LDVAL = LoadVal;
			this->Actual_period = ((this->LoadVal+1) * this->tick_period_us());
			return Error_t::Counter_Overflow;

		}

		else if(ldval < 0)
		{
			this->LoadVal = PIT_MAX_COUNTER;
			IMXRT_PIT_CHANNELS[ChID].LDVAL = LoadVal;
			this->Actual_period = ((this->LoadVal+1) * this->tick_period_us());
			return Error_t::Counter_Underflow;
		}

		else
		{	
			this->LoadVal =  uint32_t((ldval)) - 1;
			IMXRT_PIT_CHANNELS[ChID].LDVAL = LoadVal;
			this->Actual_period = ((this->LoadVal+1) * this->tick_period_us());
			return Error_t::Success;
		}
	}

	/** @brief Returns the error in the gate timing period due to the finite resolution of the timers.
	 * @Return Error in microseconds. */
	double period_error_us() const __attribute__((always_inline))
	{
		return (this->Req_period - this->Actual_period);
	}

	/** @brief Returns the value of one \b Tick - Minimum resolution of the timer. */
	double tick_period_us() __attribute__((always_inline))
	{
		return 1e6 / double(this->Clk_freq);
	}


	/** @brief Returns the correspoding XBAR1-A Input Pins for the corresponding channel TRIGGER signal. 
	 * \attention Reference - Manual Page 63. 
	 * \attention This function does not initalize the xbar channels. It only returns the corresponding pin. */
	unsigned int static get_xbar_in_pin() __attribute__((always_inline))
	{
		if(ChID == 0)
		{ return 56; }

		else if(ChID == 1)
		{ return 57; }

		else if (ChID == 2)
		{ return 58; }

		else if (ChID == 3)
		{ return 59; }
	}

	/** @brief Enable Interruptsthat are triggered after one  gate time completion. */
	void interrupt() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TCTRL |= PIT_TCTRL_TIE; //Set BIT
	}

	/** @brief Disable Interrupts that are triggered after one gate time completion. */
	void no_interrupt() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TCTRL &= ~PIT_TCTRL_TIE; //Clear BIT

	}

	/** @brief Sets a common ISR and its priority for all PIT Channels. \note All PIT channels share a single ISR on Teensy 4.x micro-controllers. */
	void static set_interrupt(void (*isr_fn)(), unsigned int priority ) __attribute__((always_inline))
	{
		//_assert_(priority <= 255, "Priority value should be below 255.");
    	NVIC_ENABLE_IRQ(IRQ_PIT);
    	NVIC_SET_PRIORITY(IRQ_PIT, priority);
    	attachInterruptVector(IRQ_PIT, isr_fn);
	}

	/** @brief Clears the interrupt flag and hence prepares the timer for the next gate interval. The clearing has to be done manually. If the flag is not cleared, the interrupt will be called again and again. */
	void clear_interrupt_flag() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TFLG = 1; //Clear by writing 1 (TIF is the only field in the register)
	}


	/** @brief Sets the clock source frequency of all PIT channels to the value F_BUS_ACTUAL (= F_CPU_ACTUAL / 4 ) which is nominally 150 MHz. The exact value depends on the CPU clock rate. 
	    \warning This will change the clock source for all 4 PITs and also the 2 GPTs.*/
	void sel_FBUS_clock() __attribute__((always_inline))
	{
		CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL; //Clear CCM-Peripheral clock gate bit -> gate down
		Clk_freq = F_BUS_ACTUAL; //Set Equal to Peripheral Bus frequency  
	}

	/** @brief Sets the source clock frequency of all PIT channels to 24 MHz which is the default oscillator clock. This setting is also the default state, if no clock is selected.
	    \warning This will change the clock source for all 4 PITs and also the 2 GPTs.*/
	void sel_24MHz_clock() __attribute__((always_inline))
	{
		  CCM_CSCMR1 |= CCM_CSCMR1_PERCLK_CLK_SEL; //Set CCM-Peripheral clock gate bit -> gate up
		  this->Clk_freq = uint32_t(24*1e6); //24MHz
	}


}; //End of class PIT Controller
