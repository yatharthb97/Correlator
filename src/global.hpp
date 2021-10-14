#include "./../code/software/multi_tau.hpp"
#include "./../code/software/histogram.hpp"

#include "./../code/hardware/pit.hpp"
#include "./../code/hardware/qtmr1.hpp"
#include "./../code/hardware/interarrival.hpp"
#include "./../code/hardware/perf_counter.hpp"



//#ifdef FEATURE_BRANCH1_GATECOUNTING
// The template parameters are defined in the build system
extern MultiTauACorrRTTeensy<LIN_CORRS, SERIES_SIZE, BIN_RATIO> multitau;

#if ENABLE_COUNT_RATE == 1
  extern RTCoarseGrainer count_rate_mon;
#endif

#if ENABLE_PC_HISTOGRAM == 1
  extern PCHistogram<uint32_t, PC_HISTOGRAM_BINS> hist;
#endif

//Hardware Resource
extern PITController<PIT_CHANNEL_IN_USE> PI_t; //!< PI_t Resource
extern TMR1Controller TTL_c; //!< TTL_c Resource



//Global volatile variables
extern volatile counter_t Counter_val; //!< Stores the value read from the counter.
extern volatile bool Update_flag; //!< Indicates if a new value has arrived from the counting module.
extern volatile unsigned int Update_count; //!< Stores the number of updates made on the correlator channels since the last serialout.

//Time Values
extern uint32_t SerialOut_After; //! Serial output is done after these many updates (default → overriden in the setup function).
extern const double Gate_time_us; //! The gate time of TTL_C in microseconds (us) 
extern const double Allowed_period_error_us; //! Gate time precision error due to finite precision of timers.
extern const int32_t sync_code;

extern float array[CHANNEL_SIZE];

//Performance Counters
#if ENABLE_PERFORMANCE_COUNTERS == 1
  extern PerfCounter ACFpf;
  extern PerfCounter Serialpf;
#endif

//#endif Feature Branch
//extern InterArrivalTime<uint16_t, uint32_t> IATimer(&IMXRT_TMR1.CH[0].CNTR); //!< Interarrival Counter
//#ifdef FEATURE_BRANCH2_TIGHTPOLLING


//#endif