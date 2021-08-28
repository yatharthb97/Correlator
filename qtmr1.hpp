#pragma once
#include "errors.hpp"

#define _TMR1_CONTROLLER_CH_ 0 //! Channel used for counting.

/** @brief Templated interface for Quad Timer 1 channels for Gate Counting. The module uses the macro `_TMR1_CONTROLLER_CH_` to identify the main channel and then assigns the next channel's (_TMR1_CONTROLLER_CH_ + 1) external pin to the _TMR1_CONTROLLER_CH_ as the "Capture Pin" or the "Secondary Count Source". */
class TMR1Controller
{
private:
	static constexpr bool Singleton_flag = false; //! Singleton template
public:

	//0
	/** @brief Default Constructor that assers if the user constructed object with a  valid _TMR1_CONTROLLER_CH_, and asserts a singleton template. */
	TMR1Controller()
	{
		static_assert((_TMR1_CONTROLLER_CH_ <= 2 && _TMR1_CONTROLLER_CH_ >= 0), 
					  "TMR1Controller: CHID is invalid (try <= 2).");
		_assert_(!Singleton_flag,  "TMR1Controller: The resource is already constructed."); //Assert Singleton
	}

	//1
	/** @brief Starts up-counting from the set counter value.*/
	void start() __attribute__((always_inline))
	{
		IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CTRL |= TMR_CTRL_CM(1); //Set (001)
	}

	//2
	/**@brief Stops counting and freezes the current counter value. */
	void stop() __attribute__((always_inline))
	{
		IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CTRL |= TMR_CTRL_CM(0); //Set (000)
	}

	//3
	/** @brief Resets the counter to zero. It also cleares the capture register and unsets the Input Edge Flag. */
	void reset() __attribute__((always_inline))
	{
		IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CNTR = 0; //Reset Counter to 0.
		IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CAPT = 0; //Reset Capture Register
		IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].SCTRL &= ~TMR_SCTRL_IEF; //Write zero to the Input Edge Flag
	}

	//4
	/** @brief Reads and returns the Capture Register. */
	counter_t get_capture_val() __attribute__((always_inline))
	{
		return (IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CAPT);
	}


	//5
	/** @brief Returns a value that has range (2 X 2**n - 1). This is done by checking the overflow flag*/
	counter_t get_capture_double_range() __attribute__((flatten, always_inline))
	{
		return (this->is_overflow() * 65535) + (this->get_capture_val());
	}


	//6
	/** @brief Returns the overflow status of the Counter. Reads the TOF - Timer Overflow Flag*/
	bool is_overflow() __attribute__((always_inline))
	{
		return (IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].SCTRL & TMR_SCTRL_TOF != 0);
	}


	//7
	/*Input Edge Flag - This bit is set when CAPTMODE is enabled and a proper input transition occurs (on an input selected as a secondary count source) while the count mode does not equal 000. This bit is cleared by writing a zero to this bit position. This bit can also be cleared automatically by a read of CAPT when DMA[IEFDE] is set.*/
	/* @brief Clear `Input Edge Flag` - which indicates a Capture Event. */
	void clear_input_edge_flag() __attribute__((always_inline))
	{
		IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].SCTRL &= ~TMR_SCTRL_IEF; //Unset
	}


	//8
	/* @brief Clears the overflow flag. */
	void clear_overflow_flag() __attribute__((always_inline))
	{
		IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].SCTRL &= ~TMR_SCTRL_TOF; //Unset
	}


	//9
	/** @brief Returns the pin number on which the TTL input must be connected. 
	 * \attention This function does not configure the pin or assign it to the QTMR module. For that TMR1Controller::init_pin() must be called. */
	int get_ttl_input_pin() __attribute__((always_inline))
	{
		#if _TMR1_CONTROLLER_CH_ == 0
			return 10; //Assigned to CH0
		#elif _TMR1_CONTROLLER_CH_ == 1
			return 12; //Assigned to CH1
		#elif _TMR1_CONTROLLER_CH_ == 2
			return 11; //Assigned to CH2
		#elif _TMR1_CONTROLLER_CH_ == 3
			static_assert(false, "This Channel has no IO Pin");
			return SAFE_INPUT_DUMP_PIN;
		#endif
	}


	//10
	/* @brief Initalise the counter and set registers for counting. */
	void init() __attribute__((always_inline))
	{
		
		//A. Control Register setup → TMR1_CTRL1 (16-bit register)                  ----------------------------------------------------------

			//0. Un-set the whole register that sets Counting mode as "No operation" → 0 [Disable Timer] (3 bit field)
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CTRL = 0;


			//2. Set Primary Count Source as "Counter #_TMR1_CONTROLLER_CH_ input pin" 
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CTRL |= TMR_CTRL_PCS(_TMR1_CONTROLLER_CH_); 

			//3. Set Secondary Count Source "CAPTURE SIGNAL PIN" to _TMR1_CONTROLLER_CH_ + 1 → The pin of the next successive channel.
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CTRL |= TMR_CTRL_SCS(_TMR1_CONTROLLER_CH_ + 1); 

			//→ Rest of the Bitfields are set to zero (those modes are the required modes.


		//Timer Channel Status and Control Register                    
		//B. (TMR1_SCTRL1) Register (16-Bit)                                            -------------------------------------------

			//0. Set Everything to zero 
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].SCTRL = 0;

			//1. These bits specify the operation of the capture register as well as the operation of the input edge flag. The input source is the secondary count source. "01 Load capture register on rising edge (when IPS=0) or falling edge (when IPS=1) of input"
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].SCTRL  |= TMR_SCTRL_CAPTURE_MODE(1);

			//OEN Output Enable (Output Pin Mode selection) - {0-Input-SCS, 1-OFLAG output}
			
			//Set the Next channel's external pin as an input pin (unset)
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_ + 1].SCTRL &= TMR_SCTRL_OEN;


		//C. TMR1_CSCTRL1 Timer Channel Comparator Status and Control Register       ----------------------------------------------------------------------

			//0. Reset Register 
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CSCTRL = 0;

			//1. Reload on Capture - This bit enables the capture function to cause the counter to be reloaded from the LOAD register. → 1 Reload the counter on a capture event.
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CSTRL |= TMR_CSTRL_ROC; //Set Bit


		//D. Timer Channel Input Filter Register (TMR1_FILT1)                                      -----------------------------------------------------
			
			//1. → Zero values disables input filtering.
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].FILT = 0;
		
		//E. Timer Channel DMA Enable Register (TMRx_DMAn)                                  ---------------------------------------------------
		
			//Input Edge Flag DMA Enable
			//1. Setting this bit enables DMA read requests for CAPT when SCTRL[IEF] is set. This also unsets the SCTRL[IEF] automatically.
			//IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].DMA |= TMR_DMA_IEFDE; // ATTENTION


		//F. TMR1_LOAD1 -> Timer Channel Load Register                                                 -----------------------------------------------
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].LOAD = 0; //Set value to zero (For upcounting)


		//G. Timer Channel Counter Register (TMR1_CNTR1)                                                  -------------------------------------------------
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CNTR = 0; //Set value to zero (For upcounting)

		//H. Timer Channel Capture Register (TMR1_CAPT1)                                          ------------------------------------------------
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].CAPT = 0; //Reset


		//I. Timer Channel Enable Register (TMR1_ENBL)                                -----------------------------------------------

			//1. Setting the corresponding Bits enables the corresponding channels
			IMXRT_TMR1.CH[_TMR1_CONTROLLER_CH_].ENBL |= (1 << _TMR1_CONTROLLER_CH_);

		volatile asm("dsb");

	} //End of init()


	//11
	/** @brief Sets the Primary and Secondary inputs for Capture Mode, routed through XBAR.
	\reference XBAR and IOMUX input selections → IOMUXC_GPR_GPR6 Manual Page 347. */
	void init_pins() __attribute__((always_inline))
	{
		#if _TMR1_CONTROLLER_CH_ == 0
			IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 1; //Assign IOPAD for CH0
			IOMUXC_GPR_GPR6 |= QTIMER1_TRM1_INPUT_SEL; //Set CH1 input to XBAR
		#elif _TMR1_CONTROLLER_CH_ == 1
			IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 1; //Assign IOPAD for CH1
			IOMUXC_GPR_GPR6 |= QTIMER1_TRM2_INPUT_SEL; //Set CH2 input to XBAR
		#elif _TMR1_CONTROLLER_CH_ == 2
			IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02 = 1; //Assign IOPAD to CH2
			IOMUXC_GPR_GPR6 |= QTIMER1_TRM3_INPUT_SEL; //Set CH3 input to XBAR
		#elif _TMR1_CONTROLLER_CH_ == 3
			static_assert(false, "This Channel has no IO Pin");
		#endif

		volatile asm("dsb");
	}

	//12
	/** @brief Returns the corresponding XBAR Output Pin to use for routing the Capture Signal. 
	 * \reference Manual Page 70. */
	unsigned int get_xbar_out_pin() __attribute__((always_inline))
	{
		#if _TMR1_CONTROLLER_CH_ == 0
			return 87; //Return CH1's pin
		#elif  _TMR1_CONTROLLER_CH_ == 1
			return 88;  //Return CH2's pin
		#elif  _TMR1_CONTROLLER_CH_ == 2
			return 89;  //Return CH3's pin 
		#elif  _TMR1_CONTROLLER_CH_ == 3
			static_assert(false, "This Channel does not have an assigned capture pin. ");
			return 133; //Any out-pin greater than 132 is invalid and ignored.
		#endif
	}


	//13
	/** @brief Reset Counter and output flags. \reference Manual Page 332. 
	 * \attention This setting applies to all the channels of the Timer 1 group. */
	void static timers_freeze() __attribute__((always_inline))
	{
		IOMUXC_GPR_GPR2 |= QTIMER1_TMR_CNTS_FREEZE; //Set Bit
	}


	//14
	/** @brief Enable counting normally. \reference Manual Page 332.
	 * \attention This setting applies to all the channels of the Timer 1 group. */
	void static timers_anti_freeze() __attribute__((always_inline))
	{
		IOMUXC_GPR_GPR2 &= ~QTIMER1_TMR_CNTS_FREEZE; //Unset Bit
	}

};

