#pragma once
#include <Arduino.h>
#include <imxrt.h>

#include "global.hpp"
#include "./../code/software/monitor_channel.hpp"
#include "./../code/software/histogram.hpp"


#include "./../code/hardware/pins.hpp"
#include "./../code/hardware/utilities.hpp"
#include "./../code/hardware/ledpanel.hpp"
#include "./../code/hardware/errors.hpp"
#include "./../code/hardware/pit.hpp"
#include "./../code/hardware/qtmr1.hpp"
#include "./../code/hardware/perf_counter.hpp"



//======================================================

//   /$$$$$$  /$$$$$$  /$$$$$$$ 
//  |_  $$_/ /$$__  $$| $$__  $$
//    | $$  | $$  \__/| $$  \ $$
//    | $$  |  $$$$$$ | $$$$$$$/
//    | $$   \____  $$| $$__  $$
//    | $$   /$$  \ $$| $$  \ $$
//   /$$$$$$|  $$$$$$/| $$  | $$
//  |______/ \______/ |__/  |__/
/** @brief ISR function used for processing counter values. */
void isr_fn() //__attribute__((interrupt ("FIQ")))
{
  //1. PIT Starts Counting
  IMXRT_PIT_CHANNELS[PIT_CHANNEL_IN_USE].TFLG = 1;
  
  //2. Starts counting again after reset
  IMXRT_TMR1.CH[0].SCTRL &= ~TMR_SCTRL_IEF; //Clear Edge Flag

  //3. Read Counter Value
  Counter_val = IMXRT_TMR1.CH[0].CNTR;
  
  //4. Reset Couter
  IMXRT_TMR1.CH[0].CNTR = 0; //Reset Counter to 0.

  //5. Assert Update Flag
  Update_flag = true;

  asm volatile ("dsb");
  //-6. Toggle Pin for debugging
  #if ISR_PIN_TOGGLE == 1
    digitalToggleFast(ISR_TEST_TOOGLE_PIN);
  #endif
}


//                                 /$$           /$$
//                                |__/          | $$
//    /$$$$$$$  /$$$$$$   /$$$$$$  /$$  /$$$$$$ | $$
//   /$$_____/ /$$__  $$ /$$__  $$| $$ |____  $$| $$
//  |  $$$$$$ | $$$$$$$$| $$  \__/| $$  /$$$$$$$| $$
//   \____  $$| $$_____/| $$      | $$ /$$__  $$| $$
//   /$$$$$$$/|  $$$$$$$| $$      | $$|  $$$$$$$| $$
//  |_______/  \_______/|__/      |__/ \_______/|__/
//! Outputs the data struct to the serial buffer
void inline serial_out()
{
  
  //0. Start performance counting when enabled
  #if ENABLE_PERFORMANCE_COUNTERS == 1
    Serialpf.start();
  #endif

  //1.0 Sync Code
  #if ENABLE_SYNC_CODE == 1
    Serial.write((uint8_t*)&(sync_code), sizeof(int32_t));
  #endif

  //2. Count Rate
  #if ENABLE_COUNT_RATE == 1
      float mean = count_rate_mon.output();
      Serial.write((uint8_t*)&(mean), sizeof(float));
  #endif
  
  //3. Points FED
  #if (ENABLE_POINTS_NORM == 1) && (ENABLE_ACF_CALC == 1)
    Serial.write((uint8_t*)&(multitau.MeanMonitor.Count), sizeof(float));
  #endif

  //4. Mean/Accumulate
  #if ENABLE_MEAN_NORM == 1
       float_t accumulate = multitau.MeanMonitor.accumulate();
       Serial.write((uint8_t*)&(accumulate), sizeof(float_t));
  #endif

  //5. ACF
  #if ENABLE_ACF_CALC == 1
    multitau.ch_out(); // Initial output of zeroes to check the channel fields.
  #endif
  

  //6. Histogram
  #if ENABLE_PC_HISTOGRAM == 1
    hist.output();
  #endif


  //7. Performance counters
  #if ENABLE_PERFORMANCE_COUNTERS == 1
      float_t pf_serial = Serialpf.mean();
      Serial.write((uint8_t*)&(pf_serial), sizeof(float_t));
      
      #if ENABLE_ACF_CALC == 1
        float_t pf_acf = ACFpf.mean();
        Serial.write((uint8_t*)&(pf_acf), sizeof(float_t));
      #endif
      
      Serialpf.end();
  #endif

  //-.
  //Serial.flush()
}



//    /$$$$$$              /$$                        
//   /$$__  $$            | $$                        
//  | $$  \__/  /$$$$$$  /$$$$$$   /$$   /$$  /$$$$$$ 
//  |  $$$$$$  /$$__  $$|_  $$_/  | $$  | $$ /$$__  $$
//   \____  $$| $$$$$$$$  | $$    | $$  | $$| $$  \ $$
//   /$$  \ $$| $$_____/  | $$ /$$| $$  | $$| $$  | $$
//  |  $$$$$$/|  $$$$$$$  |  $$$$/|  $$$$$$/| $$$$$$$/
//   \______/  \_______/   \___/   \______/ | $$____/ 
//                                          | $$      
//                                          | $$      
//                                          |__/
/** @brief Setup function for Initalization and setup.*/
void gt_setup()
{
  //1. LEDPanel Indicator Setup
  LEDPanel.init(); //Set Pinmode for all LEDs.
  LEDPanel.set(SETUP_LED);
  LEDPanel.toggle_twice(SETUP_LED, 2000);

  //2. Pin Modes
  #if ISR_PIN_TOGGLE == 1
    pinMode(ISR_TEST_TOOGLE_PIN, OUTPUT);
  #endif
  pinMode(TTL_C_PULSE_INPUT_PIN, INPUT);
  
  // 3.1. PIT_t Setup
  PI_t.enable_PITs(); //Enable all PITs
  PI_t.sel_FBUS_clock(); //Select 150MHz clock
  PI_t.interrupt(); //Enable Interrupts
  PI_t.set_interrupt(isr_fn, 255); //Set interrupt
  // 3.2. Colllect Errors
  Error_t er_invalid_gt = PI_t.set_gate_time(Gate_time_us);
  Error_t er_period_precision = Errors::Precison_Threshold(PI_t.period_error_us(), Allowed_period_error_us);

  //4. TTL_C Setup
  TTL_c.init();
  TTL_c.init_pins();


  //5. Set Serial Sampling Time
  extern uint32_t serial_max_count(double Gate_time_u, double Serial_out_time_ms);
  SerialOut_After = serial_max_count(GATE_TIME_US, SAMPLING_TIME_MS);

  //6. Error Validation Validation
  Errors::Validate(er_invalid_gt);
  Errors::Validate(er_period_precision);
  Errors::Validate(Errors::Auto_MultiTau_Input_Validator<LIN_CORRS, SERIES_SIZE, BIN_RATIO>());
  LEDPanel.assert_errors();


  //7. Serial Setup
  Serial.begin(115200);
  while(!Serial){}
  
  //7.1 Serial Output of zeroes
  serial_out();

  //8.
  LEDPanel.unset(SETUP_LED);
  asm volatile ("dsb");

  for(unsigned int i = 0; i < CHANNEL_SIZE; i++)
  {
    array[i] = i;
  }
} // End of setup()



//   /$$                                    
//  | $$                                    
//  | $$        /$$$$$$   /$$$$$$   /$$$$$$ 
//  | $$       /$$__  $$ /$$__  $$ /$$__  $$
//  | $$      | $$  \ $$| $$  \ $$| $$  \ $$
//  | $$      | $$  | $$| $$  | $$| $$  | $$
//  | $$$$$$$$|  $$$$$$/|  $$$$$$/| $$$$$$$/
//  |________/ \______/  \______/ | $$____/  
//                                | $$      
//                                | $$      
//                                |__/      

/** @brief Loop function that processes the updates from the counters. */
void gt_loop()
{
  // Assert Loop LED Indicator if the device is not in error state
  if(!LEDPanel.Error_State)
  {
    LEDPanel.set(LOOP_LED);
    LEDPanel.dim(LOOP_LED, 124);
  }

  //Entropy.Initialize();
  TTL_c.start();
  PI_t.start();
  asm volatile ("dsb");

  while(1)
  {
    if(Update_flag) //Calculate Multi-tau correlation
    {
      noInterrupts(); //Disable all interrupts

      Update_flag = false;
      Update_count++;

      // Push Backs ↓
      #if ENABLE_COUNT_RATE == 1
        count_rate_mon.push_back(Counter_val);
      #endif

      #if ENABLE_PC_HISTOGRAM == 1
        hist.push_back(Counter_val);
      #endif

      #if ENABLE_ACF_CALC == 1
          #if ENABLE_PERFORMANCE_COUNTERS == 1
            ACFpf.start();
          #endif

          multitau.push_datum(Counter_val);

          #if ENABLE_PERFORMANCE_COUNTERS == 1
            ACFpf.end();
          #endif
      #endif
      

      if(Update_count >= SerialOut_After)
      { Update_count = 0; //Reset Update_count
        
        serial_out();

      }//Serial Block

    }//Update Block
    interrupts(); //Enable all interrupts
    
  }//While Loop

  
  // <<while loop breaks>> (rare case)↓
  LEDPanel.unset(LOOP_LED);
  LEDPanel.set_all();
  abort();
} //End of loop()