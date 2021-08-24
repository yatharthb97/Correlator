#pragma once
#include "types.hpp"


/** @brief This class is a blank interface for all objects that are — \b {Linear Real-Time Software Auto-Correlators}. */
class Lin_CrossCorr_RT_Base
{
public:

  //1
  /** @brief Accepts two single `datum` from the counters — which is the lowest order of data aggregate, appends to Series_array(s) and updates relevant channels. */
  void inline virtual push_datum(counter_t datum1, counter_t datum2) = 0;

  //2
  /** @brief Repeatedy calls `Lin_CrossCorr_RT_Base::push_datam()` on all the elements of the countainers
   * \attention The containers must be of the same size. */
  void inline virtual push_data(const counter_t *container1, const counter_t *container2, const index_t size) = 0;


  //3
  /** @brief Takes in the lag value and returns the normalization (divider) for the passed lag value. */
  double inline virtual norm(index_t lag) = 0;


/** \defgroup Lin_CorrCorr_Base_Out ''Linear Cross-correlator Base Output Functions'' */
/* @{ */
  
  /** \note {The output methods do not issue an endline character - '\n' after the output is complete. That responsibility is reserved for end user.}*/

  //4
  /** @brief Outputs all the channels using specific implementation based methods. Both the channel outputs are appended together one after the other. 12-21*/
  void inline virtual ch_out() const = 0;


  //5
  /** @brief Outputs all the channels with \b <normalisation> using specific implementation based methods. \attention{All types that inherit this interface must seperately provide an on-demand normalisation method.} Both the channel outputs are appended together one after the other. 12-21.*/
  void inline virtual ch_out_norm() const = 0;

  //6
  /** Returns a pointer to the front of the Channel array - Cross correlation of 1 with lagged 2. */
  inline virtual counter_t* get_ch_array12() const = 0;

  //6
  /** Returns a pointer to the front of the Channel array - Cross correlation of 2 with lagged 1. */
  inline virtual counter_t* get_ch_array21() const = 0;

/* @} */
};
