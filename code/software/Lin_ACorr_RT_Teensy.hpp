#pragma once

#include <Arduino.h>
#include "./../types.hpp"
#include "simpler_circular_buffer.hpp"

#include <algorithm>


/** @brief This is an implementation of Lin_ACorr_RT_Base for Teensy with \b(No normalisation or baseline subtraction.)
 * \note{ Template parameter -  Size of the Series and Channel array and indicates the maximum points that can be stored by the correlator object. The circuular buffer will then rewrite the older points to accomodate for the other points. }
 * */
template <index_t Series_size>
class Lin_ACorr_RT_Teensy
{

public:

  counter_t Channel_array[Series_size] = {0}; //! Stores the Channel output
  Simpler_Circular_Buffer<counter_t, Series_size> Series_array; //! Stores the data points in a circular Buffer

  index_t Series_index = 0; //! Stores the last active index → Post-increment

  /** @brief Excepts new data value and processes it. */
  void inline push_datum(counter_t datum)
  {
    Series_array.push_back(datum);

    for(unsigned int i = 0; i <= Series_size; i++)
    {
      Channel_array[i] += (Series_array[Series_index] * Series_array[Series_index - i]);
    }

    Series_index++;
  }


  //2
  /** @brief Repeatedly calls `push_datum()` on the given container of values, one at a time. */
  void __attribute__((flatten)) push_data(const counter_t *container, const index_t size)
  {
    for (int i = 0; i < size; i++)
    {
      push_datum(container[i]);
    }
  }


  //3
  /** @brief Blank normalisation function that always returns 1.
   * @param Lag time that is ignored by the function. */
  double inline norm(unsigned int lag) {return 1;}


  //4
  /** @brief Outputs the complete channel to the Serial port. */
  void __attribute__((flatten)) ch_out() const
  {
     //                                             ↓ Which is usually uint32_t.
    //uint8_t *buffer = reinterpret_cast<const uint8_t *> (Channel_array); 
    Serial.write((char*)&Channel_array, sizeof(counter_t) * Series_size);
  }
 


  //5
  /** @brief Outputs the channel array to the Serial port after normalising it. */
  void __attribute__((flatten)) ch_out_norm() const
  {
    // TODO - Try to unroll
    for(unsigned int i = 0; i < Series_size; i++)
    {
      Serial.print(double(Channel_array[i])/norm(i), sizeof(double));
    }
  }


  //6
  /** @Returns a reference to the channel array. */
  counter_t* get_ch_array(unsigned int index = 0)
  {
    return (const_cast<counter_t*>(Channel_array) +  index);
  }
  
};