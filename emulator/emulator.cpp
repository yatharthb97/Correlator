#include "./../code/software/multi_tau.hpp" //multitau Object
#include "./../code/software/pseudoSerial.hpp" //pseudo Arduino Serial


#include "./json/single_include/nlohmann/json.hpp" //Json parsar
#include<unistd.h>
#include<fcntl.h>
#include <errno.h>


#include <iostream>
#include <fstream>
#include <random>

using counter_t = uint32_t;


//Configuration
#define TRACE_FEED "rnd"
#define LOAD_POINTS 10000
#define PSEUDOTERMINAL_ADDRESS "/dev/pts/5"
#define UPDATES 1000

//Gloabl Resources
counter_t TraceData[LOAD_POINTS] = {0};



void data_feed(); //Declaration

int main()
{
	//Create Multitau object
	MultiTauACorrRTTeensy<9, 27, 3> multitau;


	//Create and link Virtaul Port to file
	system("socat PTY,link=/dev/virtual_serial_port,raw,echo=0 -")

	std::ifstream virtual_port("/dev/corr_virtual_serial_port", std::iso::in | std::iso::binary);


	//Run Iterations
	for(auto i = 0; i < UPDATES; i++)
	{
		data_feed();
		for(auto j = 0; j < LOAD_POINTS; j++)
		{
			multitau.push_datum(TraceData[j]);
		}


		//Push data to Device's Serial Buffer
		multitau.ch_out();

		//DEBUG
		//std::cerr << multitau.DataCounter << '\n';
		

		//Read Serial Buffer to Port (Channel Size * 4 bytes)
		constexpr unsigned int buffer_size = 171*4;
		const uint32_t sync_code = -123456789;
		//ssize_t pstatus = write(sync_code, sync_code, 4);
		virtual_port << sync_code;
		virtual_port << Serial.rdbuf() << std::flush;

		sleep(1);

	}
	
}






/** @brief Fills up the TraceData array which is of size `LOAD_POINTS`. 
 * File reader assummes the relavent data is in the second column. */
void data_feed()
{
	//Data feed
	if (TRACE_FEED == "file")
	{
		const std::string filename = "";
		static std::ifstream file(filename, std::ios::in);
		static bool EOFflag = false;
		while(!EOFflag)
		{
			//Load X points
			counter_t tmp_disc = 0;
			for(auto i = 0; i < LOAD_POINTS; i++)
			{
				if(!file.eof())
				{
					
					file >> tmp_disc >> TraceData[i];
					if(TraceData[i] > 0)
					{
						std::cerr << TraceData[i] << '\n';
					}
				}

				else
				{
					EOFflag = true;
				}
			}
		}
	}

	else if (TRACE_FEED == "rnd")
	{
		static std::mt19937 mt;
		mt.seed(123456);

		std::poisson_distribution<counter_t> poiss;

		for(auto i = 0; i < LOAD_POINTS; i++)
		{
			TraceData[i] = poiss(mt);
		}
	}

} // End of data_feed()


int serial_port = open("/dev/ttyUSB0", O_RDWR);



