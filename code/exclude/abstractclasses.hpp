#pragma once


/** @brief Base class for Front and Back discarder objects. */
template <unsigned int Series_size, unsigned int Front, unsigned int End>
class FrontBack_Discarder_Base
{
public:
  /** @brief Outputs the channel while discarding the selected channels, by using the implementation specific output method. */
  void inline virtual const output(const Lin_ACorr_RT_Teensy<Series_size> &channel) const = 0;

  /** @brief Returns the number of elements discarded by the discarder object.*/
  void inline virtual discard_count() const
  {
    return (Front + End);
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////

