#pragma once
#include <imxrt.h>



#define PRREG(x) Serial.print(#x" 0x"); Serial.println(x,HEX)




/** @brief Establishes connection over XBAR1-A, given the input and output xbar pins
 * \reference - https://github.com/manitou48/teensy4/blob/bc8fc46af5065a3f84352e0474069ae7a1a13064/pitxbaradc.ino#L40
 * \licence - Not specified*/
 void xbar_connect(unsigned int input, unsigned int output)
 {
   if (input >= 88) return;
   if (output >= 132) return;
   volatile uint16_t *xbar = &XBARA1_SEL0 + (output / 2);
   uint16_t val = *xbar;
   if (!(output & 1)) {
     val = (val & 0xFF00) | input;
   } else {
     val = (val & 0x00FF) | (input << 8);
   }
   *xbar = val;
 }


//#define F_CPU_TICK ARM_DWT_CYCCNT
uint32_t F_CPU_tick_count() //__attribute__((always_inline))
{
  return ARM_DWT_CYCCNT;
}


float get_CPU_temp() //__attribute__((always_inline))
{
  return tempmonGetTemp();
}



/** @brief Returns true if the given DMA channel is in the error state. */
// bool has_DMA_error()
// {
//   return DMAx_ES != 0; //x is the channel number.
// }