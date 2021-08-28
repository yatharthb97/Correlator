//#include "Lin_ACorr_RT_Teensy.hpp"
#include "pit.hpp"
#include "pins.hpp"
#include "qtmr1.hpp"
#include "utilities.hpp"

//Hardware Resource
PITController<2> PI_t; //! PI_t Resource
TMR1Controller TTL_c; //! TTL_c Resource

//Software Resource
#define LIN_CHANNELS 9
#define SERIES_SIZE 27
#define BIN_RATI0 3
MultiTau_ACorr_RT_Teensy<LIN_CHANNELS, SERIES_SIZE, BIN_RATI0> multitau; //! Multi Tau Object

//Global volatile variables
volatile counter_t Counter_val = 0; //! Stores the value read by the counter
volatile bool Update_flag = false; //! Indicates if a new value has arrived from the counting module
volatile unsigned int Update_count = 0; //! Stores the number of updates made on the correlator channels 


//Time Values
const unsigned int SerialOut_CountMax = 1; //! Serial output is done after these many updates
const double Gate_time_us = 1; //! The gate time of TTL_C in microseconds (us) 
const double Allowed_period_error_us = 1e-3; //! Gate time precision error due to finite precision of timers.

void setup()
{
  //0. Set Setup LED
  pinMode(SETUP_LED, OUTPUT);
  digitalWrite(SETUP_LED, HIGH);


  //1. PinModes
  LEDSet::init(); //Set Pinmode for all LEDs.
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);


  //2. Serial Setup
  Serial.begin();
  while(!Serial)
  //Serial.println("Serial Connected!")


  //3. PIT_t Setup
  PI_t.enable_PITs(); //Enable all PITs
  PI_t.sel_FBUS_clock(); //Select 150MHz clock
  PI_t.interrupt(); //Enable Interrupts
  PI_t.set_interrupt(isr_fn, 255); //Set interrupt
  Error_t er_invalid_gt = PI_t.set_gate_time(gate_time);
  Error_t er_period_precision = Errors::Precison_Threshold(PI_t.period_error(), allowed_period_error);

  //4. TTL_C Setup
  TTL_c.init();
  TTL_c.pin_init();

  //5. XBAR Connection
  xbar_connect(PI_t.get_xbar_in_pin(), TTL_c.get_xbar_out_pin());


  //6. Setup is complete
  asm volatile ("dsb");
  digitalWrite(SETUP_LED, LOW);

  //7. Validation
  Errors::Validate(er_invalid_gt);
  Errors::Validate(er_period_precision);
  Errors::Validate(Auto_MultiTau_Input_Validator<LIN_CHANNELS, SERIES_SIZE, BIN_RATI0>());

  //7. Indicate End of Setup
  BlinkLED(SETUP_LED, 5, 20);


} // End of setup()

void loop()
{
  
  digitalWrite(LOOP_LED, HIGH);
  TTL_C.start();
  PI_t.start();
  asm volatile ("dsb");

  while(1)
  {
    if(update_flag) //Calculate Multi-tau correlation
    {
      Multitau.push_datum(counter_val);
      Update_count++;

      if(Update_count >= SerialOut_CountMax)
      {
        multitau.ch_out();
        Update_count = 0;
      }
    }
  }

  // <<while loop breaks>> ↓
  digitalWrite(LOOP_LED, LOW);
  LEDSet::set_all();
  LEDSet::unset(RUN_LED);
  abort();
} //End of loop()


