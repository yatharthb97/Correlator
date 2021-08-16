
//#define F_CPU_TICK ARM_DWT_CYCCNT
uint32_t __attribute__((always_inline)) F_CPU_tick() //Restructure as a MACRO
{
  return ARM_DWT_CYCCNT;
}
