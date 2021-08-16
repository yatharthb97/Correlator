#pragma once

#include "types.hpp"
#include "Lin_ACorr_RT_Base.hpp"


/** @brief Base class for Front and Back discarder objects. */
template <unsigned int Series_size, unsigned int Front, unsigned int End>
class FrontBack_Discarder_Base
{
public:
  /** @brief Outputs the channel while discarding the selected channels, by using the implementation specific output method. */
  void inline virtual const output(const Lin_ACorr_RT_Base &channel) const = 0;

  /** @brief Returns the number of elements discarded by the discarder object.*/
  void inline virtual discards() const
  {
    return (constexpr will_discard = Front + End);
  }
};


/** @brief Teensy specific Front back discarder implementation. */
template <unsigned int Series_size, unsigned int Front, unsigned int End>
class Discarder_Teensy : public FrontBack_Discarder_Base<Series_size, Front, End>
{
  void output(const Lin_ACorr_RT_Base &channel) const override __attribute__((always-inline))
  {
    Serial.write(channel.get_ch_array() + Front, sizeof(counter_t) * (Series_size - End));
  }
};



