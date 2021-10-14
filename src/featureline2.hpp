#include <Arduino.h>
#include <imxrt.h>

#pragma once
#include "global.hpp"
#include "./../code/hardware/qtmr1.hpp"
//#include "./../code/hardware/errors.hpp"
#include "./../code/hardware/ledpanel.hpp"
#include "./../code/software/multi_tau.hpp"
#include "./../code/hardware/qtmr1.hpp"


void setup_interarrival()
{

  //0. Start-up Indicator
  LEDPanel.set(LED_BLUE);
  LEDPanel.toggle_twice(LED_BLUE, 500);
  LEDPanel.toggle_twice(LED_BLUE, 500);


  //1. Set Serial Sampling Time
  Serial.begin(115200);
  while(!Serial)
  {}
  extern uint32_t serial_max_count(double Gate_time_us, double Serial_out_time_m);
  SerialOut_After = 1000000;

 
  // 2. Init QTMR
  TTL_c.init();
  TTL_c.init_pins();

  //3. Exit Startup Indicator
  LEDPanel.set(LED_WHITE);

  asm volatile ("dsb");

}  // End of setup_interarrival()


void loop_interarrival()
{
  Update_count = 0;
  Counter_val = 0;
  TTL_c.start();
  
  while(1)
  {
    //while(IATimer.PCount < SerialOut_After)
    //{
      //IATimer.measure();
      //Update_count++;
    //}

  // Serial Output Block
    
    //1. Sync Code
    #if ENABLE_SYNC_CODE == 1
      Serial.write((uint8_t*)&(sync_code), sizeof(int32_t));
    #endif

    //2. Stats Output
    Counter_val = IMXRT_TMR1.CH[0].CNTR;
    IMXRT_TMR1.CH[0].CNTR = 0;
    Serial.write((uint8_t*)&(Counter_val), sizeof(counter_t));

    // Serial output of stat struct
    //IATimer.output();


    Update_count = 0;


  }
} // End of loop_interarrival()