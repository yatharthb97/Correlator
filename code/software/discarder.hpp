#pragma once

#include <Arduino.h>
#include "./../types.hpp"
#include "Lin_ACorr_RT_Teensy.hpp"

/** @brief Teensy specific Front back discarder implementation. */
template <unsigned int Series_size, unsigned int Front, unsigned int End>
class Discarder_Teensy
{
public:
  void output(const Lin_ACorr_RT_Teensy<Series_size> &channel) const __attribute__((always_inline))
  {
    //                                             ↓ Which is usually uint32_t.
    //uint8_t *buffer = reinterpret_cast<const uint8_t *> (channel.get_ch_array()[Front]); 
    //Serial.write(buffer, sizeof(counter_t) * (Series_size - End));
    
    Serial.write((char*)&(channel.get_ch_array()[Front]), sizeof(counter_t) * (Series_size - End));
  }

  /** @brief Returns the number of elements discarded by the discarder object.*/
  unsigned int inline virtual discard_count() const
  {
    return (Front + End);
  }

};
