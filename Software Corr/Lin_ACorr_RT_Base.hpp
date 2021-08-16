#pragma once
#include "types.hpp"

/** @brief This class is a blank interface for all objects that are — \b {Linear Real Time Auto Correlators}. */
class Lin_ACorr_RT_Base
{
public:

  /** @brief Accepts a single `datum` from the counter — which is the lowest order of data aggregate, appends to Series_array and updates relevant channels. */
  void inline virtual push_datum(counter_t datum) = 0;

  /** @brief Repeatedy calls `Lin_ACorr_RT_Base::push_datam()` on all the elements of the countainer. */
  void inline virtual push_data(const counter_t *container, const index_t size) = 0;

  /** @brief Outputs all the channels using specific implementation based methods.*/
  void inline virtual ch_out() const = 0;

  /** @brief Outputs all the channels with \b <normalisation> using specific implementation based methods. \attention{All types that inherit this interface must seperately provide an on-demand normalisation method.}*/
  void inline virtual ch_out_norm() const = 0;

  /** Returns a pointer to the front of the Channel array. */
  inline virtual counter_t* get_ch_array() const = 0;
};
