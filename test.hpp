
//#define F_CPU_TICK ARM_DWT_CYCCNT
uint32_t __attribute__((always_inline)) F_CPU_tick_count() //Restructure as a MACRO
{
  return ARM_DWT_CYCCNT;
}


float __attribute__((flatten)) get_CPU_temp()
{
  return tempmonGetTemp();
}


//Is this an active low pulse?
void single_pulse(const uint8_t PIN) __attribute__((always_inline))
{
  digitalWriteFast(PIN, HIGH);
  asm("dsb");
  asm("nop");
  digitalToggleFast(PIN);
  asm("dsb")
}

//Route to PIN 14
void test_sigle_pulse() __attribute__((flatten))
{
  unsigned long duration = pulseIn(14, HIGH);
  Serial.println(duration);

  //Serial.println(ARM_DWT_CYCCNT);
  asm("nop");
  //Serial.println(ARM_DWT_CYCCNT);

}


IMXRT_TMR_t * TMRx = (IMXRT_TMR_t *)&IMXRT_TMR4;


/** @brief Returns true if the given DMA channel is in the error state. */
bool has_DMA_error()
{
  return DMAx_ES != 0; //x is the channel number.
}