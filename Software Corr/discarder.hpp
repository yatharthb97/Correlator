#pragma once

#include "types.hpp"
#include "Lin_ACorr_RT_Base.hpp"


template <unsigned int Series_size, unsigned int Front, unsigned int End>
class Serial_Discarder_Teensy
{
  void output(const Lin_ACorr_RT_Base &channel)
  {
    Serial.write(channel.get_ch_array() + Front, sizeof(counter_t) * (Series_size - End));
  }
};