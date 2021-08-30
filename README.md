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

* `Lin_CrossCorr_RT_Base.hpp` - Base interface for Linear Cross Correlators
* `Lin_CrossCorr_RT_Teensy.hpp` - Teensy specific Liner Cross Correlator interface

## Hardware 

File descriptions:

 * `pit.hpp` - Defines `class PITController` that provides an abstraction layer on the PIT timer controls.
* `qtmr1.hpp` - Defines 'class QTMR1Controller' that provides an abstraction layer on the QTMR1 timer controls.
* `lifetime_timer.hpp` - Interface for using PIT timers in chained mode to create a 64-bit lifetime counter

## Common Interface 

* `modules.hpp` - Defines functions that represent highest-abstracted modules in the system.
* `errors.hpp` - Defines common error codes and error generating functions.
* `utilities.hpp` - Contains some utility functions







## Standard Flushing Procedure for USB Serial on Teensy:

• Libraries will use `Serial.write(buffer, bytes);` for all outputs.

• For line buffered outputs, at the end, user calls:

    `Serial.send_now('\n')`. Note: The libraries are not allowed to use endlines on output calls. This also solves the "`\r\n`" problem of using `Serial.println()`.

• As per the PJRC website, `Serial.flush()` waits for the buffer to clear, but might not actually initiate the clearing. Hence, Using `Serial.send_now()` might prove more judicious.



## SLOC as on 28/08/21

---------- Result ------------

            Physical :  1873
              Source :  1093
             Comment :  503
 Single-line comment :  284
       Block comment :  223
               Mixed :  132
 Empty block comment :  0
               Empty :  409
               To Do :  0

Number of files read :  23

----------------------------