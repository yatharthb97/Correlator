#pragma once

//Abstraction Layer for Hardware modules

PITController<0> PI_t; 


//Part of setup
/** @brief Typical setup for PI_t module. */
Error_t PI_t_typical_setup(double gate_time, void (*isr_fn)())
{
  PI_t.enable_PITs(); //Enable all PITs
  PI_t.sel_FBUS_clock(); //Select 150MHz clock
  PI_t.interrupt(); //Enable Interrupts
  PI_t.set_interrupt(isr_fn, 255); //Set interrupt
  Error_t error = PI_t.set_gate_time(gate_time); //Set gate_time as 5 us
  asm("dsb"); //Data Synchronisation Barrier
  return error;
}


/** @brief Test ISR for PI_t*/
FAST void pit_isr() __attribute__((interrupt ("FIQ")))
{
  IMXRT_PIT_CHANNELS[0].TFLG = 1;
  //PI_t.clear_interrupt_flag(); → scoping issues
  digitalToggleFast(8);
  digitalToggleFast(9);
  asm("nop"); //32 "No operations" 
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("dsb"); //Data Synchronization Barrier
}

//Enable PIN for PIT_TRIGGER00 → How to route AD_B0_04 to a physical pin or directly to TTL_C ?
IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_04 &= ~SION; //Unset SION Bit (Modality - "Input Path is determined by functionality")
IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_04 |= MUX_MODE(6); //Set Trigger

FAST void pit_isr() __attribute__((interrupt ("FIQ")))
{
  //1. PIT Starts Counting
  IMXRT_PIT_CHANNELS[0].TFLG = 1;
  //PI_t.clear_interrupt_flag(); → scoping issues

  //2. Wait for Capture
  while(IMXRT_TMR1.CH[ChID].SCTRL & TMR_SCTRL_IEF == 0)
    //Wait
    { }

  //3. Starts counting again after reset
  IMXRT_TMR1.CH[ChID].SCTRL &= ~TMR_SCTRL_IEF; //Clear Edge Flag
  IMXRT_TMR1.CH[ChID].CNTR = 0; //Reset Counter to 0.

  //4. Copy data 
  New_Datum = TTL_C.get_capture_val();
  IMXRT_TMR1.CH[ChID].CAPT = 0; //Clear Capture Register (optional)
  Push_Datum_Flag = true;
}