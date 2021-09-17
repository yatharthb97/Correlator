#include <cstdint>
#include <cmath>

#include <imxrt.h>
// Main's Utility functions

uint32_t serial_max_count(double Gate_time_us, double Serial_out_time_ms)
{
	uint32_t count = std::floor((Serial_out_time_ms * 1000) / Gate_time_us);
	return count; 
}