// Main file

//   /$$      /$$           /$$          
//  | $$$    /$$$          |__/          
//  | $$$$  /$$$$  /$$$$$$  /$$ /$$$$$$$ 
//  | $$ $$/$$ $$ |____  $$| $$| $$__  $$
//  | $$  $$$| $$  /$$$$$$$| $$| $$  \ $$
//  | $$\  $ | $$ /$$__  $$| $$| $$  | $$
//  | $$ \/  | $$|  $$$$$$$| $$| $$  | $$
//  |__/     |__/ \_______/|__/|__/  |__/
/** @brief Main Function. */
#include "featureline1.hpp"
#include "featureline2.hpp"
#include "featureline3.hpp"


int main()
{
    
    #if DEVICE_FEATURELINE == 1
      gt_setup();
      gt_loop();
    #elif DEVICE_FEATURELINE == 2
      setup_interarrival();
      loop_interarrival();
    #elif DEVICE_FEATURELINE == 3
      sampler_setup();
      sampler_loop();
    #endif
}