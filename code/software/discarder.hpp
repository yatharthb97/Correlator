#pragma once

//#include <Arduino.h>
#include "./../types.hpp"
#include "Lin_ACorr_RT_Teensy.hpp"
#ifdef CORR_SIMULATOR
  #include "pseudoSerial.hpp"
#else
  #include <Arduino.h>
#endif

/** @brief Teensy specific Front back discarder implementation. */
template <unsigned int Series_size, unsigned int Front, unsigned int End>
class DiscarderTeensy
{
public:
  template <typename LinCorrType>
  void output(const LinCorrType &channel) const //__attribute__((always_inline))
  {
    //                      ↓ Which is usually uint32_t or 32-bit float
    Serial.write((char8cast_t*)(channel.Channel_array + Front), 
                 sizeof(channel_t)*(Series_size - (Front + End))
                );
  }

  /** @brief Returns the number of elements discarded by the discarder object.*/
  unsigned int discard_count() const //__attribute__((always_inline))
  {
    return (Front + End);
  }

};
