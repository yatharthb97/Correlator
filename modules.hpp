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