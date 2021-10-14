#include "global.hpp"


//#ifdef FEATURE_BRANCH1_GATECOUNTING
// The template parameters are defined in the build system
MultiTauACorrRTTeensy<LIN_CORRS, SERIES_SIZE, BIN_RATIO> multitau;

#if ENABLE_COUNT_RATE == 1
  RTCoarseGrainer count_rate_mon(CR_CG_INTERVAL_US, GATE_TIME_US);
#endif

#if ENABLE_PC_HISTOGRAM == 1
  PCHistogram<uint32_t, PC_HISTOGRAM_BINS> hist;
#endif


//Hardware Resource
PITController<PIT_CHANNEL_IN_USE> PI_t; //!< PI_t Resource
TMR1Controller TTL_c; //!< TTL_c Resource


//Global volatile variables
volatile counter_t Counter_val = 0; //!< Stores the value read from the counter.
volatile bool Update_flag = false; //!< Indicates if a new value has arrived from the counting module.
volatile unsigned int Update_count = 0; //!< Stores the number of updates made on the correlator channels since the last serialout.

//Time Values
uint32_t SerialOut_After = 100; //! Serial output is done after these many updates (default → overriden in the setup function).
const double Gate_time_us = GATE_TIME_US; //! The gate time of TTL_C in microseconds (us) 
const double Allowed_period_error_us = ALLOWED_GATE_TIME_ERROR_US; //! Gate time precision error due to finite precision of timers.
const int32_t sync_code = SYNC_CODE;

float array[CHANNEL_SIZE] = {0};

//Performance Counters
#if ENABLE_PERFORMANCE_COUNTERS == 1
  PerfCounter ACFpf;
  PerfCounter Serialpf;
#endif

//#endif Feature Branch
//InterArrivalTime<uint16_t, uint32_t> IATimer(&IMXRT_TMR1.CH[0].CNTR); //!< Interarrival Counter
//#ifdef FEATURE_BRANCH2_TIGHTPOLLING


//#endif