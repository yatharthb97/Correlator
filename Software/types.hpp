#pragma once
#include <cstdint>

//Type Declarations
using counter_t = uint_fast32_t ; //! Data type received from the pulse counter. It is the fundamental type used for representing series data.

using index_t = uint_fast8_t ; //! It is used as the array indices and thus determine the maximum size of the Channel_array and the Series_array.








//Options
#define LC_DSep ','

//Feature Macros
#define LC_SAFETY_CHECKS 1