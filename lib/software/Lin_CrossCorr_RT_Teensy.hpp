#pragma once
#include "types.hpp"
#include "Lin_CrossCorr_RT_Base.hpp"


/** @brief This is an implementation of Lin_ACorr_RT_Base for Teensy with \b(No normalisation or baseline subtraction.)
 * \note{ Template parameter -  Size of the Series and Channel array and indicates the maximum points that can be stored by the correlator object. The circuular buffer will then rewrite the older points to accomodate for the other points. }
 * */
template <index_t Series_size>
class Lin_CrossCorr_RT_Teensy : public Lin_CrossCorr_RT_Base
{

public:

  counter_t Channel_array12[Series_size] = {0}; //! Stores the Channel output - 12
  counter_t Channel_array21[Series_size] = {0}; //! Stores the Channel output - 21

  
  //! Stores the data points in a circular Buffer
  Simpler_Circular_Buffer<counter_t, Series_size> Series_array1;
  Simpler_Circular_Buffer<counter_t, Series_size> Series_array2;


  index_t Series_index1 = 0; //! Stores the last active index → Post-increment
  index_t Series_index2 = 0; //! Stores the last active index → Post-increment


  //1
  void __attribute__((flatten)) push_datum(counter_t datum1, counter_t datum2) override
  {
    Series_array1.push_back(datum1);
    Series_array2.push_back(datum2);


    for(unsigned int i = 0; i <= Series_size; i++)
    {
      Channel_array12[i] += (Series_array1[Series_index] * Series_array2[Series_index - i]);
      Channel_array21[i] += (Series_array2[Series_index] * Series_array1[Series_index - i]);

    }

    Series_index1++;
    Series_index2++;
  }


  //2
  void __attribute__((flatten)) push_data(const counter_t *container1,const counter_t *container2,  const index_t size) override
  {
    //unsigned int size = std::distance(std::begin(container), std::end(container));
    for (int i = 0; i < size; i++)
    {
      push_datum(container1[i], container2[i]);
    }
  }


  //3
  /** @brief Blank normalisation function that always returns 1.
   * @param Lag time that is ignored by the function. */
  double inline norm(unsigned int lag) {return 1;}


  //4
  void __attribute__((flatten)) ch_out() override
  {
    Serial.write(Channel_array12, sizeof(counter_t) * Series_size);
    Serial.write(Channel_array21, sizeof(counter_t) * Series_size);

  }


  //5
  void __attribute__((flatten)) ch_out_norm() override
  {
    #pragma unroll Series_size
    for(unsigned int i = 0; i < Series_size; i++)
    {
      Serial.write(double(Channel_array12[i])/norm(i), sizeof(double));
      Serial.write(double(Channel_array21[i])/norm(i), sizeof(double));

    }
  }

  //6
  counter_t* get_ch_array12() __attribute__((always_inline)) const
  {
    return Channel_array12;
  }


  //7
  counter_t* get_ch_array21() __attribute__((always_inline)) const
  {
    return Channel_array21;
  }
  
};