	
void tmr1_setup()
{
	
	//A. Control Register setup -------------------------------------------------------------------

	//1. Set Counting mode as "Count rising edges of primary source" → 0001
	setRegBitGroup(TMR1_CTRL,CM,0x01);

	//2. Set Primary Count Source as "Counter 0 input pin" → 0000.
	setRegBitGroup(TMR1_CTRL,PCS,0x00);


	//3. Set Secondary Count Source to "Counter 1 input pin" → 01
	setRegBitGroup(TMR1_CTRL, SCS, 0x01);

	//4. Set Count Once field to "Count Repeatedly" → 0 (Unset)
	TMR1_CTRL &= ~TMR1_CTRL_ONCE;

	//5. Set LENGTH to "Count until roll over at $FFFF and continue from $0000" → 0 (Unset)
	TMR1_CTRL &= ~TMR1_CTRL_LENGTH;


	//6. Set Count Direction → Count Up (0 - Unset)
	TMR1_CTRL &= ~TMR1_CTRL_DIR;


	//7. Co-Channel Initalization → Disable with 0 (Unset)
	TMR1_CTRL &= ~TMR1_CTRL_COINIT;

	//8. OUTMODE - Output Mode These bits determine the mode of operation for the OFLAG output signal. Set to Asserted while counter is active. (0000)
	setRegBitGroup(TMR1_CTRL, OUTMODE, 0x00);

	//B. TMR1_SCTRL Registers  -------------------------------------------------------------------

	//1. un-set overflow interruot
	TMR1_SCTRL &= ~TMR1_SCTRL_TOFIE;

	//2. set RISING INPUT POLARITY → Unset BIT
	TMR1_SCTRL &= ~TMR1_SCTRL_IPS; 

	//3. Set Output Polarity to TRUE Polarity → Unset BIT
	TMR1_SCTRL &= ~TMR1_SCTRL_OPS; 

	//4. Disable Master mode → Unset BIT
	TMR1_SCTRL &= ~TMR1_SCTRL_MSTR;

	//5. Output Enable mode → Configure the "EXTERNAL PIN". 0 → The external pin is configured as an input.
	TMR1_SCTRL &= ~TMR1_SCTRL_OEN;

	//C. Timer Channel Input Filter Register (TMRx_FILTn) ------------------------------------------
		
	//1. → Zero values disables input filtering.
	TMR1_FILT1 = 0x00;
	
	//D. Timer Channel DMA Enable Register (TMRx_DMAn) ---------------------------------------------
		
	//Input Edge Flag DMA Enable
	//1. Setting this bit enables DMA read requests for CAPT when SCTRL[IEF] is set.
	TMR1_DMA1 |= TMR1_DMA1_IEFDE;

	//E. Timer Channel Enable Register (TMRx_ENBL) ---------------------------------------------

	//1. Setting the corresponding Bits enables the corresponding channels → Enable channel 1
	setRegBitGroup(TMR1_ENBL, ENBL, 0x01);

}