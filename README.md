# DIY Teensy Correlator Project Repository

## Software

This folder contains the implementation of the software correlator that will be used on Teensy. The file descriptions are as follows:
 
* `Lin_ACorr_RT_Base.hpp` - Base (Interface) for Linear Correlators
* `Lin_ACorr_RT_Teensy.hpp` - Implementation specific to Teensy
* `multi_tau.hpp`   -   Teensy specific implementation of multi-tau Auto-correlator
* `accumulator.hpp` - Adapter object used by Multi Tau ACorr
* `discarder.hpp` - Adapter object used by Multi Tau ACorr
* `simpler_circular_buffer.hpp` - Simple circlar buffer used for storing the cout values.
* `types.hpp` - Contains the `typedef` of the abstracted typenames 
    + `counter_t` - Type returned by the Counter module
    + `index_t` - Type used to index arrays and buffers in the implementation
* `test.cpp` - File used for tsting
* `circlar_buffer.hpp` - Another implementation of circular buffer (unsued right now) 

## Hardware 

File descriptions:

 * `pit.hpp` - Defines `class PITController` that provides an abstraction layer on the PIT timer controls.
* `tmr.hpp`
* `quadtmr_setup.hpp`

## Common Interface 

* `modules.hpp` - Defines functions that represent highest-abstracted modules in the system.
* `errors.hpp` - Defines common error codes and error generating functions.







## Standard Flushing Procedure for USB Serial on Teensy:

• Libraries use `Serial.write(buffer, bytes);` for all outputs.
• For line buffered outputs, the final user calls:
    `Serial.write_now('\n')`. Note: The libraries are not allowed to use endlines on output calls. This also solves the "`\r\n`" problem of using `Serial.println()`.