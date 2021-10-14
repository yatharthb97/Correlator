#pragma once


#include "./../types.hpp"
#include "simpler_circular_buffer.hpp"
#ifdef CORR_SIMULATOR
  #include "pseudoSerial.hpp"
#else
  #include <Arduino.h>
#endif

/** @brief This is an implementation of Lin_ACorr_RT_Base for Teensy with \b(No normalisation or baseline subtraction.)
 * \note{ Template parameter -  Size of the Series and Channel array and indicates the maximum points that can be stored by the correlator object. The circuular buffer will then rewrite the older points to accomodate for the other points. }
 * */
template <index_t Series_Size, bool hasMonitorChannel>
class LinACorrRTTeensy
{

public:

  //! Defines a run-time polymorphic MonitorChannel object, which decomposes to `ghost` channel (UnitMeanChannel) if `hasMonitorChannel` is true. Basically `UnitMeanChannel` does nothing.
  //MonitorChannel<float_t, hasMonitorChannel> Monitor;

  //MonitorChannel<hasMonitorChannel> Mean_monitor; //!< Mean monitor object
  channel_t Channel_array[Series_Size] = {0}; //!< Stores the Channel output
  Simpler_Circular_Buffer<counter_t, Series_Size> Series_array; //!< Stores the data points in a circular Buffer

  index_t Series_index = 0; //!< Stores the last active index → Post-increment

  //0
  /** @brief Default Constructor. */
  LinACorrRTTeensy()
  {
  }

  //1
  /** @brief Adds new *single* data point and processes it to the `Channel`. */
  void inline push_datum(counter_t datum)
  {
    Series_array.push_back(datum);

    unsigned int LoopFor = (Series_index > Series_Size) ? Series_Size-1 : Series_index;
    //LoopFor = (Series_index > Series_Size) * Series_Size +
    //          !(Series_index > Series_Size) * Series_index;
    
    for(unsigned int i = 0; i <= LoopFor; i++)
    {
      //Channel_array[i] += (Series_array[Series_index] * 
      //                    Series_array[Series_index - i]);

      Channel_array[i] += (datum * Series_array[Series_index - i]);
    }

    Series_index++;
  }


  //2
  /** @brief Repeatedly calls `push_datum()` on the given container of values, one at a time. */
  void push_data(const counter_t *container, const index_t size) __attribute__((flatten))
  {
    for (int i = 0; i < size; i++)
    {
      push_datum(container[i]);
    }
  }


  //3
  /** @brief Returns the accumulate of the channel so far.
   * @param Lag time that is ignored by the function. */
  float_t inline norm() {return 1;}


  //4
  /** @brief Outputs the complete channel to the Serial port. */
  void ch_out() const __attribute__((always_inline))
  {
     //                                              ↓ Which is usually uint32_t. 
    Serial.write((char8cast_t*)&(Channel_array), sizeof(channel_t)*Series_Size);
  }
 


  //5
  /** @brief Outputs the channel array to the Serial port after normalising it. */
  void  ch_out_norm() const __attribute__((flatten))
  {
    // TODO - Try to unroll
    for(unsigned int i = 0; i < Series_Size; i++)
    {
      Serial.write((char8cast_t*)&(float_t(Channel_array[i])/norm()), sizeof(float_t));
    }
  }


  //6
  /** @Returns a reference to the channel array. */
/*  channel_t* get_ch_array()
  {
    return (Channel_array);
  }*/
  
};
