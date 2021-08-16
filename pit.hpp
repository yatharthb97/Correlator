#pragma once


// Error codes
enum class RC_t
{
	Success = 0,
	Counter_Overflow,
	Counter_Underflow
};

template <unsigned int ChID>
class PITController
{
public:
  //Period values are in microseconds (us)
	double Actual_period = 0;
  double Set_period = 0;
	uint32_t Clk_freq = 24 * 1e6; //24MHz → default oscillator clock 
	const static constexpr uint32_t PIT_MAX_COUNTER = 4294967295;

PITController()
{ static_assert(ChID <=3, "No such PIT channel exists"); }

//Group1
/** \defgroup PIT_Glb ''PIT Global Controls'' */
/* @{ */
	void static enable_PITs() __attribute__((always_inline))
	{
		PIT_MCR = 0x00; //Write 0 is ON
	} 

	void static disable_PITs() __attribute__((always_inline))
	{
		PIT_MCR = 0x01; //Write 1 is OFF
	}

	void pause_resume_PITs() __attribute__((always_inline))
	{
		PIT_MCR ^= PIT_MCR_FRZ;
	}

/* @} */ //End of Interrupt Group // End of Group PIT Global Controls



//Group 2
/** \defgroup Controls ''Control Functions'' */
/* @{ */

	void start() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TCTRL |= PIT_TCTRL_TEN; //Set BIT
	}

	void  stop() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TCTRL &= ~PIT_TCTRL_TEN; //Unset BIT
	}

	/**
	 * @brief Returns the actual value set - silent errors. \todo{Add actual TP calculation.} */
	RC_t set_gate_time(double gt_microseconds) __attribute__((flatten))
	{
		double ldval = gt_microseconds * this->Clk_freq * 1e-6;
    this->Set_period = gt_microseconds;
		
		if(ldval > PIT_MAX_COUNTER)
		{
			IMXRT_PIT_CHANNELS[ChID].LDVAL = PIT_MAX_COUNTER;
			return RC_t::Counter_Overflow;

		}

		else if(ldval <= 0)
		{
			IMXRT_PIT_CHANNELS[ChID].LDVAL = PIT_MAX_COUNTER;
			return RC_t::Counter_Underflow;
		}

		else
		{	
			IMXRT_PIT_CHANNELS[ChID].LDVAL = uint32_t(ldval) - 1;
			this->Actual_period = (uint32_t(ldval) - 1) * this->tick_period_us();
			return RC_t::Success;
		}
	}

	double tick_period_us() __attribute__((always_inline))
	{
		return 1e-6 / this->Clk_freq;
	}
/* @} */ //End of Control Functions



//Group 3
/** \defgroup Interrupt ''Interrupt Functions'' */
/* @{ */

	void interrupt() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TCTRL |= PIT_TCTRL_TIE; //Set BIT
	}

	void no_interrupt() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TCTRL &= ~PIT_TCTRL_TIE; //Clear BIT

	}

	void static set_interrupt(void (*isr_fn)(), unsigned int priority ) __attribute__((always_inline))
	{
		//assert(priority <= 255);
    NVIC_ENABLE_IRQ(IRQ_PIT);
    NVIC_SET_PRIORITY(IRQ_PIT, priority);
    attachInterruptVector(IRQ_PIT, isr_fn);
	}

	void clear_interrupt_flag() __attribute__((always_inline))
	{
		IMXRT_PIT_CHANNELS[ChID].TFLG = 1; //Clear by writing 1
	}
/* @} */ //End of Interrupt Group



//Group 4
/** \defgroup CLK ''Clock Select Functions'' */
/* @{ */

	void sel_FBUS_clock() __attribute__((always_inline))
	{
		CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL; //Clear CCM-Peripheral clock gate bit -> gate down
		this->Clk_freq = F_BUS_ACTUAL; //Set Equal to Peripheral Bus frequency  
	}

	void sel_24MHz_clock() __attribute__((always_inline))
	{
		  CCM_CSCMR1 |= CCM_CSCMR1_PERCLK_CLK_SEL; //Set CCM-Peripheral clock gate bit -> gate up
		  this->Clk_freq = 24000000; //24MHz
	}
/* @{ */ // ENd of Clock Select Functions

}; //End of class PIT Controller
