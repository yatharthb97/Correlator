//#include "Lin_ACorr_RT_Teensy.hpp"
#include "pit.hpp"
#include "pins.hpp"
#include "qtmr1.hpp"


//Hardware Resource
const int PI_t_CH = 3; //! PIT Channnel that will be used for PI_t
PITController<PI_t_CH> PI_t; //! PI_t Resource
TMR1Controller<> TTL_C; //! TTL_c Resource

//Software Resource
MultiTau_ACorr_RT_Teensy<3, 10, 3> multitau; //! Multi Tau Object

//Global volatile variables
volatile counter_t counter_val = 0; //! Stores the value read by the counter
volatile bool update_flag = false; //! Indicates if a new value has arrived from the counting module
volatile unsigned int update_count = 0; //! Stores the number of updates made on the correlator channels 

//Time Values
const unsigned int serial_out_count_max = 1; //! Serial output is done after these many updates
const double gate_time_us = 1; //! The gate time of TTL_C in microseconds (us) 
const double allowed_period_error = 0; //! Gate time precision error due to finite precision of timers.

void setup()
{
  pinMode(SETUP_LED, OUTPUT);
  digitalWrite(SETUP_LED, HIGH);


  //1. PinModes
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);
  pinMode(,OUTPUT);



  //2. Serial Setup
  Serial.begin();
  while(!Serial)
  Serial.println("Serial Connected!")

  //3. PIT_t Setup
  PI_t.enable_PITs(); //Enable all PITs
  PI_t.sel_FBUS_clock(); //Select 150MHz clock
  PI_t.interrupt(); //Enable Interrupts
  PI_t.set_interrupt(isr_fn, 255); //Set interrupt
  Error_t er_invalid_gt = PI_t.set_gate_time(gate_time);
  Error_t er_period_precision = Errors::Precison_Threshold(PI_t.period_error(), allowed_period_error);

  //4. TTL_C Setup
  //** Pending


  //5. Setup is complete
  asm volatile ("dsb");
  digitalWrite(SETUP_LED, LOW);

  //6. Validation
  Errors::Validate(er_invalid_gt);
  Errors::Validate(er_period_precision);

  //7. Indicate Start
  BlinkLED(SETUP_LED, 5);


} // End of setup()

void loop()
{
  
  digitalWrite(RUN_LED, HIGH);
  RTC.start();
  TTL_C.start();
  PI_t.start();
  asm volatile ("dsb");

  while(1)
  {
    if(update_flag) //Calculate Multi-tau correlation
    {
      multitau.push_datum(counter_val);
      update_count++;
    }

    if(update_count >= serial_out_count_max)
    {
      multitau.ch_out();
      update_count = 0;
    }
  }

  digitalWrite(RUN_LED, LOW);
} //End of loop()


