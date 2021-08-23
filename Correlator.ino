//#include "Lin_ACorr_RT_Teensy.hpp"
#include "pit.hpp"
//#include "DigitalToggle/DigitalToggle.h"

#include "TTL_C.hpp"

PITController<0> PI_t;
void setup()
{
  PI_t.enable_PITs(); //Enable all PITs
  PI_t.sel_FBUS_clock(); //Select 150MHz clock
  PI_t.interrupt(); //Enable Interrupts
  PI_t.set_interrupt(pit_isr, 255);
  PI_t.set_gate_time(0.009); //Set gate_time as 5 us
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  
  pinMode(LED_BUILTIN, OUTPUT);
  delay(2000); //2sec
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  PI_t.start();
  while(1)
  {}
}

void pit_isr() //__attribute__((interrupt ("FIQ")))
{
  IMXRT_PIT_CHANNELS[0].TFLG = 1;
  digitalToggleFast(8);
  digitalToggleFast(9);
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
  asm("nop");
  asm("dsb"); //Data Synchronization Barrier
}
