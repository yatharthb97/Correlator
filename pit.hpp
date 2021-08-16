#pragma once



/** @brief Interface for PIT timers on Teensy 4.x microcontrollers. */
template <unsigned int ChID>
class PITController
{

public:

	//Period values are in microseconds (us)
	double Actual_period = 0; //! Actual Time Period set by the device due to finite resolution
  double Set_period = 0; //! Time Period requested by the user.

	uint32_t Clk_freq = uint32_t(24 * 1e6); //! Clock frequency used by the timer. Initalized to 24MHz → default oscillator clock 
	const static constexpr uint32_t PIT_MAX_COUNTER = 4294967295; //! Constant - Maximum possible counter value. 32 bit counter.

PITController()
{ static_assert(ChID <=3, "No such PIT channel exists"); }

//Group1
/** \defgroup PIT_Glb ''PIT Global Controls'' */
/* @{ */

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

	/** @brief Pause/Resume - \b Toggle all PIT Channels. */
	void pause_resume_PITs() __attribute__((always_inline))
	{
		PIT_MCR ^= PIT_MCR_FRZ;
	}

/* @} */ // End of Group PIT Global Controls



//Group 2
/** \defgroup Controls ''Control Functions'' */
/* @{ */

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
	 * @brief Sets the gate time after which the counter returns to zero and marks the end of onegate interval. */
	Error_t set_gate_time(double gt_microseconds) __attribute__((flatten))
	{
		double ldval = gt_microseconds * this->Clk_freq * 1e-6;
    this->Set_period = gt_microseconds;
		
		if(ldval > PIT_MAX_COUNTER)
		{
			IMXRT_PIT_CHANNELS[ChID].LDVAL = PIT_MAX_COUNTER;
			return Error_t::Counter_Overflow;

		}

		else if(ldval <= 0)
		{
			IMXRT_PIT_CHANNELS[ChID].LDVAL = PIT_MAX_COUNTER;
			return Error_t::Counter_Underflow;
		}

		else
		{	
			IMXRT_PIT_CHANNELS[ChID].LDVAL = uint32_t(ldval) - 1;
			this->Actual_period = (uint32_t(ldval) - 1) * this->tick_period_us();
			return Error_t::Success;
		}
	}

	/** @brief Returns the value of one \b Tick - Minimum resolution of the timer. */
	double tick_period_us() __attribute__((always_inline))
	{
		return 1e-6 / this->Clk_freq;
	}
/* @} */ //End of Control Functions



//Group 3
/** \defgroup Interrupt ''Interrupt Functions'' */
/* @{ */

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
		//assert(priority <= 255);
    NVIC_ENABLE_IRQ(IRQ_PIT);
    NVIC_SET_PRIORITY(IRQ_PIT, priority);
    attachInterruptVector(IRQ_PIT, isr_fn);
	}

	/** @brief Clears the interrupt flag and hence prepares the timer for the next gate interval. The clearing has to be done manually. If the flag is not cleared, the interrupt will be called again and again. */
	void clear_interrupt_flag() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TFLG = 1; //Clear by writing 1
	}
/* @} */ //End of Interrupt Group



//Group 4
/** \defgroup CLK ''Clock Select Functions'' */
/* @{ */

	/** @brief Sets the clock source frequency of all PIT channels to the value F_BUS_ACTUAL (= F_CPU_ACTUAL / 4 ) which is nominally 150 MHz. The exact value depends on the CPU clock rate. */
	void static sel_FBUS_clock() __attribute__((always_inline))
	{
		CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL; //Clear CCM-Peripheral clock gate bit -> gate down
		this->Clk_freq = F_BUS_ACTUAL; //Set Equal to Peripheral Bus frequency  
	}

	/** @brief Sets the source clock frequency of all PIT channels to 24 MHz which is the default oscillator clock. This setting is also the default state, if no clock is selected. */
	void static sel_24MHz_clock() __attribute__((always_inline))
	{
		  CCM_CSCMR1 |= CCM_CSCMR1_PERCLK_CLK_SEL; //Set CCM-Peripheral clock gate bit -> gate up
		  this->Clk_freq = 24000000; //24MHz
	}
/* @{ */ // ENd of Clock Select Functions

}; //End of class PIT Controller
