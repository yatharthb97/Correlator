#pragma once




[[DEPRECIATED → AS THE CAPTURE IS ROUTED INTERNALLY THROUGH XBAR]]
/** @brief Returns the pin number on which the capture signal must be connected. 
 * \attention This function does not configure the pin and assign it to the QTMR module, for that TMR1Controller::init_pin() must be called. */
int get_capture_signal_pin()
{
	#if _TMR1_CONTROLLER_CH_ == 0
		return 12; //Assigned to CH1
	#elif _TMR1_CONTROLLER_CH_ == 1
		return 11; //Assigned to CH2
	#elif _TMR1_CONTROLLER_CH_ == 2
		return 11; //Assigned to CH0
	#elif _TMR1_CONTROLLER_CH_ == 3
		static_assert(false, "This Channel has no TTL Pin");
		return  SAFE_INPUT_DUMP_PIN;
	#endif
	 
}

/* @brief Sets the pins and mutexes for input/output. \reference Page 312 Manual. The Pins can be used for either outputting OFLAG or can act as a Capture Signal Pin. The pins are bidirectional and require no speicific configuration. */
void init_extern_pins(int channel) __attribute__((always_inline))
{
	if(channel == 0)
	{ IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 = 1; }
	

	else if(channel == 1)
	{ IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_01 = 1; }
	

	else if(channel == 2)
	{ IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_02 = 1; }
	
	else
	{ assert(false); }
	
	asm volatile ("dsb");
}

//Enable PIN for PIT_TRIGGER00
IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_04 &= ~SION; //Unset SION Bit (Modality - "Input Path is determined by functionality")
IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_04 |= MUX_MODE(6); //Set Trigger
//Which pin is this???