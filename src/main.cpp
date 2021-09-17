// Main file

#include<Arduino.h>
#include <imxrt.h>

#include "./../code/software/multi_tau.hpp"
#include "./../code/hardware/pit.hpp"
#include "./../code/hardware/pins.hpp"
#include "./../code/hardware/qtmr1.hpp"
#include "./../code/hardware/utilities.hpp"
#include "./../code/hardware/ledpanel.hpp"


// The template parameters are defined in the build system
MultiTau_ACorr_RT_Teensy<LIN_CORRS, SERIES_SIZE, BIN_RATIO> multitau;

//Hardware Resource
PITController<PIT_CHANNEL_IN_USE> PI_t; //!< PI_t Resource
TMR1Controller TTL_c; //!< TTL_c Resource

//Global volatile variables
volatile counter_t Counter_val = 0; //!< Stores the value read from the counter
volatile bool Update_flag = false; //!< Indicates if a new value has arrived from the counting module
volatile unsigned int Update_count = 0; //!< Stores the number of updates made on the correlator channels since the last serialout

//Time Values
uint32_t SerialOut_After = 100000; //! Serial output is done after these many updates
const double Gate_time_us = 500; //! The gate time of TTL_C in microseconds (us) 
const double Allowed_period_error_us = 1; //! Gate time precision error due to finite precision of timers.

//const double Gate_time_us = GATE_TIME; //! The gate time of TTL_C in microseconds (us) 
//const double Allowed_period_error_us = ALLOWED_GATE_TIME_ERROR; //! Gate time precision error due to finite precision of timers.

// Resources for Testing (temporary)
#define CHANNEL_SIZE 171
int array[CHANNEL_SIZE];

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

  //-6. Toggle Pin for debugging
  #if ISR_PIN_TOGGLE == 1
    digitalToggleFast(PIT_TEST_TOOGLE_PIN1);
  #endif
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
void setup()
{
  //1. LEDPanel Indicator Setup
  LEDPanel.init(); //Set Pinmode for all LEDs.
  LEDPanel.set(SETUP_LED);
  LEDPanel.toggle_all_routine(500);
  LEDPanel.toggle_all_routine(500);

  //2. Pin Modes
  #if ISR_PIN_TOGGLE == 1
    pinMode(PIT_TEST_TOOGLE_PIN, OUTPUT);
  #endif
  pinMode(TTL_C_PULSE_INPUT_PIN, INPUT);
  
  //3. Serial Setup
  Serial.begin(115200);
  while(!Serial){}
  Serial.println("Serial Connected!");


  //3. PIT_t Setup
  PI_t.enable_PITs(); //Enable all PITs
  PI_t.sel_FBUS_clock(); //Select 150MHz clock
  PI_t.interrupt(); //Enable Interrupts
  PI_t.set_interrupt(isr_fn, 255); //Set interrupt
  // Colllect Errors
  Error_t er_invalid_gt = PI_t.set_gate_time(Gate_time_us);
  Error_t er_period_precision = Errors::Precison_Threshold(PI_t.period_error_us(), Allowed_period_error_us);

  //4. TTL_C Setup
  TTL_c.init();
  TTL_c.init_pins();

  //6. Setup is complete
  LEDPanel.toggle(SETUP_LED, 500);
  LEDPanel.toggle(SETUP_LED, 500);

  LEDPanel.unset(SETUP_LED);

  

  //7. Error Validation Validation
  Errors::Validate(er_invalid_gt);
  Errors::Validate(er_period_precision);
  Errors::Validate(Errors::Auto_MultiTau_Input_Validator<LIN_CORRS, SERIES_SIZE, BIN_RATIO>());
  LEDPanel.assert_errors();
  asm volatile ("dsb");

  //7. Testing block
  for (int i=0; i < CHANNEL_SIZE; i++) 
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
void loop()
{
  // Assert Loop LED Indicator if the device is not in error state
  if(!LEDPanel.Error_State)
  {
    LEDPanel.set(LOOP_LED);
    LEDPanel.min_bright_all();
  }

  TTL_c.start();
  PI_t.start();
  asm volatile ("dsb");

  while(1)
  {
    if(Update_flag) //Calculate Multi-tau correlation
    {
      //Multitau.push_datum(Counter_val);
      Update_flag = false;
      Update_count++;

      if(Update_count >= SerialOut_After)
      {
        //Testing → Serial Out Block
        Serial.write((char*)&(array), sizeof(int)*CHANNEL_SIZE);
        Serial.print('\n');
        Serial.flush();
      
      }//Serial Block
    
    }//Update Block
  
  }//While Loop

  
  // <<while loop breaks>> (rare case)↓
  LEDPanel.unset(LOOP_LED);
  LEDPanel.set_all();
  abort();
} //End of loop()



//   /$$      /$$           /$$          
//  | $$$    /$$$          |__/          
//  | $$$$  /$$$$  /$$$$$$  /$$ /$$$$$$$ 
//  | $$ $$/$$ $$ |____  $$| $$| $$__  $$
//  | $$  $$$| $$  /$$$$$$$| $$| $$  \ $$
//  | $$\  $ | $$ /$$__  $$| $$| $$  | $$
//  | $$ \/  | $$|  $$$$$$$| $$| $$  | $$
//  |__/     |__/ \_______/|__/|__/  |__/
/** @brief Main Function. */
int main()
{
    setup();
    loop();
}