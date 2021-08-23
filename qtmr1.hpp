#pragma once


/* TODO -> Channel ID is fixed for the timer and 1 Timer module is used completely. Make 32-bit timer ? [Chain two timers and use the output pin of the one remaining one. Use Pins 11 & 12 and use in chained config]. */

/** @brief Templated interface for Quad Timer 1 Channels. */
template <unsigned int ChID>
class TMR1Controller
{
	
	/** @brief Default Constructor that assers if the user constructed object with a  valid ChID, */
	{ static_assert( ChID <= 3, "Invalid Channel ID, no such timer exists.")}

	/** @brief Starts up-counting from the set counter value.*/
	void start() __attribute__((always_inline))
	{
		//setRegBitGroup(TMR1_CTRL1, TMR_CTRL_CM1, 0x01); //001 - Count rising edges of primary source
		IMXRT_TMR1.CH[ChID].CTRL |= TMR_CTRL_CM(1);
	}

	/**@brief Stops counting and freezes the current counter value. */
	void stop() __attribute__((always_inline))
	{
		//setRegBitGroup(TMR1_CTRL1, TMR_CTRL_CM, 0x00); //000 - No counting
		IMXRT_TMR1.CH[ChID].CTRL |= TMR_CTRL_CM(0);

	}

	/** @brief Resets the counter to zero. It also cleares the capture register and unsets the Input Edge Flag. */
	void reset() __attribute__((always_inline))
	{
		IMXRT_TMR1.CH[ChID].CNTR = 0; //Reset Counter to 0.
		IMXRT_TMR1.CH[ChID].CAPT = 0; //Reset Capture Register
		IMXRT_TMR1.CH[ChID].SCTRL &= ~TMR_SCTRL_IEF; //Write zero to the Input Edge Flag
	}

	/** @brief Reads and returns the Capture Register. */
	counter_t get_capture_val() __attribute__((always_inline))
	{
		return (IMXRT_TMR1.CH[ChID].CAPT);
	}

	/** @brief Returns the overflow status of the Counter. */
	bool is_overflow() __attribute__((always_inline))
	{
		//Timer Overflow Flag
		return (IMXRT_TMR1.CH[ChID].SCTRL & TMR_SCTRL_TOF != 0);
	}


	/*Input Edge Flag - This bit is set when CAPTMODE is enabled and a proper input transition occurs (on an input selected as a secondary count source) while the count mode does not equal 000. This bit is cleared by writing a zero to this bit position. This bit can also be cleared automatically by a read of CAPT when DMA[IEFDE] is set.*/
	
	/* @brief Clear `Input Edge Flag` - which indicates a Capture Event. */
	void clear_input_edge_flag() __attribute__((always_inline))
	{
		IMXRT_TMR1.CH[ChID].SCTRL &= ~TMR_SCTRL_IEF;
	}


	/* @brief Clears the overflow flag. */
	void clear_overflow_flag() __attribute__((always_inline))
	{
		IMXRT_TMR1.CH[ChID].SCTRL &= ~TMR_SCTRL_TOF;
	}

	/* @brief Initalise the counter and set registers for counting. */
	void init() __attribute__((always_inline))
	{
		
		//A. Control Register setup → TMR1_CTRL1 (16-bit register)                  ----------------------------------------------------------

			//0. Un-set the whole register that sets Counting mode as "No operation" → 0 [Disable Timer] (3 bit field)
			IMXRT_TMR1.CH[ChID].CTRL = 0;


			//2. Set Primary Count Source as "Counter #ChID input pin" 
			IMXRT_TMR1.CH[ChID].CTRL |= TMR_CTRL_PCS(ChID); 

			//3. Set Secondary Count Source "CAPTURE SIGNAL PIN" to "Counter 1 input pin" - PIN12  → Bits(01)
			//setRegBitGroup(TMR1_CTRL1, TMR_CTRL_SCS, 0x01);
			IMXRT_TMR1.CH[ChID].CTRL |= TMR_CTRL_SCS(ChID+1); 

			//→ Rest of the Bitfields are set to zero (those modes are the required modes.

		//Timer Channel Status and Control Register                    
		//B. (TMR1_SCTRL1) Register (16-Bit)                                            -------------------------------------------

			//0. Set Everything to zero 
			IMXRT_TMR1.CH[ChID].SCTRL = 0;

			//1. These bits specify the operation of the capture register as well as the operation of the input edge flag. The input source is the secondary count source. "01 Load capture register on rising edge (when IPS=0) or falling edge (when IPS=1) of input"
			IMXRT_TMR1.CH[ChID].SCTRL  |= TMR_SCTRL_CAPTURE_MODE(1);

		//C. TMR1_CSCTRL1 Timer Channel Comparator Status and Control Register       ----------------------------------------------------------------------

			//0. Reset Register 
			IMXRT_TMR1.CH[ChID].CSCTRL = 0;

			//1. Reload on Capture - This bit enables the capture function to cause the counter to be reloaded from the LOAD register. → 1 Reload the counter on a capture event.
			IMXRT_TMR1.CH[ChID].CSTRL |= TMR_CSTRL_ROC; //Set Bit


		//D. Timer Channel Input Filter Register (TMR1_FILT1)                                      -----------------------------------------------------
			
			//1. → Zero values disables input filtering.
			IMXRT_TMR1.CH[ChID].FILT = 0;
		
		//E. Timer Channel DMA Enable Register (TMRx_DMAn)                                  ---------------------------------------------------
		
			//Input Edge Flag DMA Enable
			//1. Setting this bit enables DMA read requests for CAPT when SCTRL[IEF] is set. This also unsets the SCTRL[IEF] automatically.
			IMXRT_TMR1.CH[ChID].DMA |= TMR_DMA_IEFDE;


		//F. TMR1_LOAD1 -> Timer Channel Load Register                                                 -----------------------------------------------
			IMXRT_TMR1.CH[ChID].LOAD = 0; //Set value to zero (upcounting)


		//G. Timer Channel Counter Register (TMR1_CNTR1)                                                  -------------------------------------------------
			IMXRT_TMR1.CH[ChID].CNTR = 0; //Set value to zero (upcounting)

		//H. Timer Channel Capture Register (TMR1_CAPT1)                                          ------------------------------------------------
			IMXRT_TMR1.CH[ChID].CAPT = 0; //Reset

		//I. Timer Channel Enable Register (TMR1_ENBL)                                -----------------------------------------------

			//1. Setting the corresponding Bits enables the corresponding channels
			IMXRT_TMR1.CH[ChID].ENBL |= (1 << CHID);

		asm volatile ("dsb");

	} //End of init()


	//XBAR and IOMUX input selections → IOMUXC_GPR_GPR6 Manual Page 347

	/* @brief Sets the pins and mutexes for input/output. \reference Page 312 Manual. The Pins can be used for either outputting OFLAG or can act as a Capture Signal Pin. The pins are bidirectional and require no speicific configuration. */
	void init_pins() __attribute__((always_inline))
	{
		if(ChID == 0)
		{ IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 1; }
		

		else if(ChID == 1)
		{ IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 1; }
		

		else if(ChID == 2)
		{ IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02 = 1; }
		
		else if (ChID = 3)
		{ IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_08 = 1; }
		
		asm volatile ("dsb");
	}

	/** @brief Reset Counter and output flags. \reference Manual Page 332. 
	 * \attention This setting applies to all the channels of the Timer 1 group. */
	void static timer_freeze() __attribute__((always_inline))
	{
		IOMUXC_GPR_GPR2 |= QTIMER1_TMR_CNTS_FREEZE; //Set Bit
	}

	/** @brief Enable counting normally. \reference Manual Page 332.
	 * \attention This setting applies to all the channels of the Timer 1 group. */
	void static timer_anti_freeze() __attribute__((always_inline))
	{
		IOMUXC_GPR_GPR2 &= ~QTIMER1_TMR_CNTS_FREEZE; //Set Bit
	}

	/** @brief Returns the pin number on which the capture signal must be connected. 
	 * \attention This function does not configure the pin and assign it to the QTMR module, for that TMR1Controller::init_pin() must be called. */
	int get_capture_signal_pin()
	{
		if(ChID == 0)
		{ return 10; }
		

		else if(ChID == 1)
		{ return 12; }
		

		else if(ChID == 2)
		{ return 11; }
		
		else if (ChID = 3)
		{ /* static_assert(false, "This Channel has no Capture Pin"); */ return  SAFE_INPUT_DUMP_PIN; }
		
	}

	/** @brief Returns the pin number on which the TTL input must be connected. 
	 * \attention This function does not configure the pin and assign it to the QTMR module, for that TMR1Controller::init_pin() must be called. */
	int get_ttl_input_pin()
	{
		if(ChID == 0)
		{ return 10; }
		

		else if(ChID == 1)
		{ return 12; }
		

		else if(ChID == 2)
		{ return 11; }
		
		else if (ChID = 3)
		{ /* static_assert(false, "This Channel has no Capture Pin"); */ return  SAFE_INPUT_DUMP_PIN; }
	}


};