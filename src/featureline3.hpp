#pragma once
#include <Arduino.h>
#include <imxrt.h>

#include "global.hpp"


#include "./../code/hardware/pins.hpp"
#include "./../code/hardware/ledpanel.hpp"
#include "./../code/hardware/errors.hpp"
#include "./../code/hardware/pit.hpp"
#include "./../code/hardware/qtmr1.hpp"

#include "./../code/software/monitor_channel.hpp"


/** @brief This featureline (see `featurelines.md) samples Counter values at periodic intervals and prints it to the Serial buffer(using `println()`). This machinary is a subset of `*featureline1*`, which is the complete Gate Counting featureline.
 * 
 * */


//Scoped Resource
#if DEVICE_FEATURELINE == 3
	MonitorChannel<true> CounterMean;
#endif

void isr_fn3()
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




void sampler_setup()
{
	//1. LEDPanel Indicator Setup
	LEDPanel.init(); //Set Pinmode for all LEDs.
	LEDPanel.set(LED_WHITE);

	//2. Pin Modes
	#if ISR_PIN_TOGGLE == 1
	  pinMode(ISR_TEST_TOOGLE_PIN, OUTPUT);
	#endif
	pinMode(TTL_C_PULSE_INPUT_PIN, INPUT);
	
	// 3.1. PIT_t Setup
	PI_t.enable_PITs(); //Enable all PITs
	PI_t.sel_FBUS_clock(); //Select 150MHz clock
	PI_t.interrupt(); //Enable Interrupts
	PI_t.set_interrupt(isr_fn3, 255); //Set interrupt
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
	LEDPanel.assert_errors();

	//7. Serial Setup
	Serial.begin(115200);
	while(!Serial){}
	

	//8.
	asm volatile ("dsb");
} //End of sampler_setup()

void sampler_loop()
{
	while(1)
	{

		if(!LEDPanel.Error_State)
		{
		  LEDPanel.set(LOOP_LED);
		}

		//Start Timers
		TTL_c.start();
		PI_t.start();

		if(Update_flag)
		{
			noInterrupts(); //NO INTERRUPT BLOCK


			Update_count++;
			#if DEVICE_FEATURELINE == 3
				CounterMean.push_back(Counter_val);
			#endif


			if(Update_count >= SerialOut_After)
			{
				Serial.print("cv: ");
				Serial.print(Counter_val);

				#if DEVICE_FEATURELINE == 3
					float mean = CounterMean.mean();
					Serial.print("  m: ");
					Serial.print(mean);

					Serial.print("  c: ");
					Serial.print(CounterMean.Count);

					Serial.print("  a: ");
					Serial.println(CounterMean.Accumulate);
					//CounterMean.reset();
				#endif

				Update_count = 0;
		
			}

			interrupts(); //NO INTERRUPT BLOCK

		} //Update

	} //infinite loop

} //End of sampler_loop()