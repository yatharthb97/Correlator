#pragma once
#include <imxrt.h>

#ifdef CORR_SIMULATOR
  #include "./../software/pseudoSerial.hpp"
#else
  #include <Arduino.h>
#endif

/** @brief Performance Counter class for teensy 4.1 microcontrollers. 
 * \note `imxrt.h` defines `ARM_DWT_CYCCNT` as `(*(volatile uint32_t *)0xE0001004)`.*/
class PerfCounter
{
public:
  
  uint32_t Stack = 0; //!< Stack for mean calculation
  uint32_t Count = 0; //!< Count for mean calculation
  uint32_t Cycles = 0; //!< Last recorded duration (difference in clock cycles recorded)
  uint32_t StartTime = 0; //!< Stored starting time of the measurement

  /** @brief Set up the debug module for clock cycle counting.
   * \warning This function sets the registers globally and will affect all instances
   * and also the overall performance. */
  void static init() __attribute__((always_inline))
  {
    ARM_DEMCR |= ARM_DEMCR_TRCENA;   //Enable Trace
    ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA; //Enable Cycle counting
  }

  /** @brief Start Measurement. */
  void start() __attribute__((always_inline))
  {
    StartTime = ARM_DWT_CYCCNT;
  }

  /** @brief End measurement. */
  void end() __attribute__((always_inline))
  {
    volatile uint32_t now = ARM_DWT_CYCCNT;
    Cycles =  now - StartTime;
    Stack += Cycles;
    Count++;
  }

  /** @brief Returns the mean number of cycles between `start()` and `stop()`. */
  float inline mean()
  {
    return float(Stack)/float(Count);
  }

  /** @brief Returns the last recorded duration. */
  uint32_t inline last_duration()
  {
    return Cycles;
  }

  void reset() __attribute__((always_inline))
  {
    Count = 0;
    Stack = 0;
    Cycles = 0;
  }

  void output() __attribute__((always_inline))
  {
    float_t mean_ = mean(); //Mean
    Serial.write((char8cast_t*)&(mean_), sizeof(float_t));

    //Serial.write((char8cast_t*)(last_duration()), sizeof(uint32_t)); //Last Cycles

  }
};