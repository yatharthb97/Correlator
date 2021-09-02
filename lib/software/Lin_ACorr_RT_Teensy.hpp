#pragma once
#include "./../types.hpp"
#include "Lin_ACorr_RT_Base.hpp"


/** @brief This is an implementation of Lin_ACorr_RT_Base for Teensy with \b(No normalisation or baseline subtraction.)
 * \note{ Template parameter -  Size of the Series and Channel array and indicates the maximum points that can be stored by the correlator object. The circuular buffer will then rewrite the older points to accomodate for the other points. }
 * */
template <index_t Series_size>
class Lin_ACorr_RT_Teensy : public Lin_ACorr_RT_Base
{

public:

  counter_t Channel_array[Series_size] = {0}; //! Stores the Channel output
  Simpler_Circular_Buffer<counter_t, Series_size> Series_array; //! Stores the data points in a circular Buffer

  index_t Series_index = 0; //! Stores the last active index → Post-increment

  //1
  void __attribute__((flatten)) push_datum(counter_t datum) override
  {
    Series_array.push_back(datum);

    for(unsigned int i = 0; i <= Series_size; i++)
    {
      Channel_array[i] += (Series_array[Series_index] * Series_array[Series_index - i]);
    }

    Series_index++;
  }


  //2
  void __attribute__((flatten)) push_data(const counter_t *container, const index_t size) override
  {
    //unsigned int size = std::distance(std::begin(container), std::end(container));
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
  void __attribute__((flatten)) ch_out() override
  {
    Serial.write(Channel_array, sizeof(counter_t) * Series_size);
  }


  //5
  void __attribute__((flatten)) ch_out_norm() override
  {
    #pragma unroll Series_size
    for(unsigned int i = 0; i < Series_size; i++)
    {
      Serial.write(double(Channel_array[i])/norm(i), sizeof(double));
    }
  }


  //6
  counter_t* get_ch_array() __attribute__((always_inline)) const
  {
    return Channel_array;
  }
  
};