
// Blink.ino OR blink.ino

#include<Arduino.h>
//#include "./../code/software/multi_tau.hpp"
//#include "./../code/software/Lin_ACorr_RT_Teensy.hpp"
#include "./../code/hardware/pit.hpp"
#include "./../code/hardware/pins.hpp"
#include "./../code/hardware/qtmr1.hpp"
#include "./../code/hardware/utilities.hpp"
#include "./../code/hardware/ledpanel.hpp"


//Hardware Resource
PITController<2> PI_t; //! PI_t Resource
TMR1Controller TTL_c; //! TTL_c Resource

//Software Resource
#define LIN_CORRS 9
#define SERIES_SIZE 27
#define BIN_RATIO 3


//Global volatile variables
volatile counter_t Counter_val = 0; //! Stores the value read by the counter
volatile bool Update_flag = false; //! Indicates if a new value has arrived from the counting module
volatile unsigned int Update_count = 0; //! Stores the number of updates made on the correlator channels 


//Time Values
const unsigned int SerialOut_CountMax = 100000; //! Serial output is done after these many updates
const double Gate_time_us = 0.5; //! The gate time of TTL_C in microseconds (us) 
const double Allowed_period_error_us = 0.1; //! Gate time precision error due to finite precision of timers.

#define PIT_TEST_TOOGLE_PIN1 3
#define PIT_TEST_TOOGLE_PIN2 7


void isr_fn() //__attribute__((interrupt ("FIQ")))
{
 //1. PIT Starts Counting
 IMXRT_PIT_CHANNELS[2].TFLG = 1;
 //PI_t.clear_interrupt_flag(); → scoping issues

 //2. Wait for Capture (Until IEF is *NOT* set.)
 //while((IMXRT_TMR1.CH[0].SCTRL & TMR_SCTRL_IEF) == 0)
  // { } //Wait

 //3. Starts counting again after reset
 IMXRT_TMR1.CH[0].SCTRL &= ~TMR_SCTRL_IEF; //Clear Edge Flag
 Counter_val = IMXRT_TMR1.CH[0].CNTR;
 IMXRT_TMR1.CH[0].CNTR = 0; //Reset Counter to 0.

 Update_flag = true;

digitalToggleFast(PIT_TEST_TOOGLE_PIN1);
digitalToggleFast(PIT_TEST_TOOGLE_PIN2);
//LEDPanel.toggle_all();

 //4. Print Counter Value
 //Serial.println(Counter_val);
}


void setup()
{
  //1. PinModes
  LEDPanel.init(); //Set Pinmode for all LEDs.
  digitalWrite(LED_BUILTIN, HIGH);
  //pinMode(TTL_C_PULSE_INPUT_PIN, INPUT);
  pinMode(PIT_TEST_TOOGLE_PIN1, OUTPUT);
  pinMode(PIT_TEST_TOOGLE_PIN2, OUTPUT);
  digitalWriteFast(PIT_TEST_TOOGLE_PIN1, HIGH);
  digitalWriteFast(PIT_TEST_TOOGLE_PIN2, HIGH);



  //2. Serial Setup
  Serial.begin(115200);
  while(!Serial){}
  Serial.println("Serial Connected!");


  //3. PIT_t Setup
  PI_t.enable_PITs(); //Enable all PITs
  PI_t.sel_FBUS_clock(); //Select 150MHz clock
  PI_t.interrupt(); //Enable Interrupts
  PI_t.set_interrupt(isr_fn, 255); //Set interrupt
  Error_t er_invalid_gt = PI_t.set_gate_time(Gate_time_us);
  Error_t er_period_precision = Errors::Precison_Threshold(PI_t.period_error_us(), Allowed_period_error_us);

  //4. TTL_C Setup
  TTL_c.init();
  TTL_c.init_pins();

  //5. XBAR Connection
  Serial.println(PI_t.get_xbar_in_pin());
  Serial.println(TTL_c.get_xbar_out_pin());

  xbar_connect(PI_t.get_xbar_in_pin(), TTL_c.get_xbar_out_pin());

  delay(5000);
  //6. Setup is complete
  asm volatile ("dsb");
  digitalWrite(SETUP_LED, LOW);

  //7. Validation
  Errors::Validate(er_invalid_gt);
  Errors::Validate(er_period_precision);
  Errors::Validate(Errors::Auto_MultiTau_Input_Validator<LIN_CHANNELS, SERIES_SIZE, BIN_RATIO>());

  //7. Indicate End of Setup
  LEDPanel.unset_all();

} // End of setup()

void loop()
{
  
  LEDPanel.set(LOOP_LED);
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

      if(Update_count >= SerialOut_CountMax)
      {
        //Multitau.ch_out();
        Update_count = 0;
        Serial.println(Counter_val);
        Serial.flush();
      }
    }
  }

  // <<while loop breaks>> ↓
  digitalWrite(LED_BUILTIN, HIGH);
  LEDPanel.unset(LOOP_LED);
  abort();
} //End of loop()


/** @brief Main Function*/
int main()
{
    Serial.begin(115200);
    //while(!Serial)

    //Serial.println("Serial!");
    
    LEDPanel.init();
    //pinMode(LED_BUILTIN, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(7, OUTPUT);
    digitalWriteFast(3, HIGH);
    digitalWriteFast(7, HIGH);
    asm volatile ("dsb");
    int i = -1;
    while(++i < 5)
    {
      //digitalWriteFast(LED_BUILTIN, HIGH);
      LEDPanel.set(LED_BUILTIN);
      LEDPanel.set(LED_YELLOW);

      digitalToggleFast(3);
      digitalToggleFast(7);
      delay(50);
      LEDPanel.unset(LED_BUILTIN);
      LEDPanel.unset(LED_YELLOW);
      delay(50);
    }
    //LEDPanel.set_all();
    LEDPanel.set(LED_RED);
    LEDPanel.set(LED_YELLOW);
    
    delay(2000);
    LEDPanel.unset(LED_RED);
    LEDPanel.unset(LED_YELLOW);



    setup();

    i = -1;
    while(++i < 5)
    {
      //digitalWriteFast(LED_BUILTIN, HIGH);
      LEDPanel.set(LED_BUILTIN);
      LEDPanel.set(LED_YELLOW);

      digitalToggleFast(3);
      digitalToggleFast(7);
      delay(50);
      LEDPanel.unset(LED_BUILTIN);
      LEDPanel.unset(LED_YELLOW);
      delay(50);
    }

    loop();
}