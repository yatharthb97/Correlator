//FAST void pit_isr() __attribute__((interrupt ("FIQ")))
//{
//  //1. PIT Starts Counting
//  IMXRT_PIT_CHANNELS[0].TFLG = 1;
//  //PI_t.clear_interrupt_flag(); → scoping issues
//
//  //2. Wait for Capture
//  while(IMXRT_TMR1.CH[ChID].SCTRL & TMR_SCTRL_IEF == 0)
//    //Wait
//    { }
//
//  //3. Starts counting again after reset
//  IMXRT_TMR1.CH[ChID].SCTRL &= ~TMR_SCTRL_IEF; //Clear Edge Flag
//  IMXRT_TMR1.CH[ChID].CNTR = 0; //Reset Counter to 0.
//
//  //4. Copy data 
//  New_Datum = TTL_C.get_capture_val();
//  IMXRT_TMR1.CH[ChID].CAPT = 0; //Clear Capture Register (optional)
//  Push_Datum_Flag = true;
//}
