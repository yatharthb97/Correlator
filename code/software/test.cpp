#ifdef CORR_SIMULATOR
	//#include "monitor_channel.hpp"
	//#include "accumulator.hpp"
	//#include "Lin_ACorr_RT_Teensy.hpp"
	//#include "multi_tau.hpp"
	#include <iostream>
	#include <random>
	#include <cmath>
	#include <unistd.h>
	#include <array>
	#include <fstream>
	#include "pseudoSerial.hpp"

/*
// TEST 1. For RTCoarseGrainer
int main()
{
	float gt_us = 10;
	RTCoarseGrainer count_rate(1000000, gt_us);
	std::mt19937 mt;
	std::uniform_int_distribution<uint32_t> dist(0, 100);
	float sigfreq = 510000;
	uint32_t num = std::floor(sigfreq * gt_us * 1e-6);
	float fnum = sigfreq * gt_us * 1e-6;

	std::cerr << "\n\n" << num << "   " << fnum;
	for(unsigned long int i = 0; i < 100000000; i++)
	{
		//num = dist(mt);
		count_rate.push_back(num);

		std::cerr << i << ".  " <<  count_rate.output() << '\n';
	}
	
}*/





/*//TEST 2. For Accumulator
class Channel
{
public:
	uint32_t counter = 0;
	void push_datum(unsigned int datum)
	{
		counter++;
		std::cerr << counter << " .  " << datum << '\n';
	}
};

int main()
{
	Accumulator accumulator;
	accumulator.do_accumulate(15);
	Channel chan;

	for(auto i = 0; i < 1000000; i++)
	{
		std::cerr << i << "\n";
		accumulator.pipe(chan, 1);
	} 

}*/




//TEST 3. Linear Correlator
/*typedef counter_t uint32_t;
int main()
{
	std::mt19937 mt;
	std::uniform_int_distribution<uint32_t> dist(0, 100);
	

	// Define 9 linear correlators
	std::array< LinACorrRTTeensy<27, true>, 9 > lincorrs;
	
	//Defien 9 Accumulators
	std::array<Accumulator, 9> accumulators;
	for (unsigned int i = 0; i < 9; i++)
	{
		accumulators[i].do_accumulate(std::pow(3,i));
	}

	counter_t val = 1;
	for(counter_t i = 0; i < 100000; i++)
	{
		for(counter_t j = 0; j < 13; j++)
		{
			for(unsigned int k = 0; k < 9; k++)
			{
				accumulators[k].pipe(lincorrs[k], dist(mt));
				//lincorrs[k].push_datum(val);
			}
			
		}

		std::cerr << i << "   Accumulate:\n";
		for(unsigned int k = 0; k < 9; k++)
		{
			std::cerr << lincorrs[k].Monitor.accumulate() << ",  ";
		}
		std::cerr << '\n';

		std::cerr << i << "   Means:\n";
		for(unsigned int k = 0; k < 9; k++)
		{
			std::cerr << lincorrs[k].Monitor.mean() << ",  ";
		}
		std::cerr << '\n';
		//sleep(0.1);
	}

}*/




//TEST 4. PCHISTOGRAM test
/*std::mt19937 mt;
std::uniform_int_distribution<uint32_t> dist(0, 100);
PCHistogram<uint32_t, 10> hist;
#include "histogram.hpp"
int main()
{
	mt.seed(5417587);
	
	uint32_t samples = 1000;
	uint32_t Histogram[hist.size()] = {0};
	for(unsigned int i = 0; i < samples; i++)
	{
		hist.push_back(dist(mt));
	}

	hist.output();
	uint32_t tmp = 0;
	for(unsigned int i = 0; i < hist.size()-1; i++)
	{
		Serial.read(reinterpret_cast<char*>(&tmp), sizeof tmp);
		Histogram[i] = tmp;
		std::cerr << i << ":  " << Histogram[i] << "    ";
	}
	Serial.read(reinterpret_cast<char*>(&tmp), sizeof tmp);
	Histogram[hist.size()-1] = tmp;
	std::cerr << "overflow: " << Histogram[hist.size()-1];
	std::cerr << '\n';

}*/



// TEST 5. REAL DATA ACF PROCESSING

//typedef counter_t uint32_t;
//#define LOAD_POINTS 500000
//Resource Init
//MultiTauACorrRTTeensy<9,27,3> multitau;
//LinACorrRTTeensy<10000, false> multitau;
//PCHistogram<uint32_t, 10> hist;
//counter_t TraceData[LOAD_POINTS] = {0};
//const unsigned int channel_size = 10000;
//double ACF[channel_size] = {0};
//double ACFnorm[channel_size] = {0};

/*void read_acf(std::string filename)
{
	//Output to binary buffer → Serial
	multitau.ch_out();

	//Fill ACF Array from buffer
	counter_t tmp = 0;
	for(unsigned int i = 0; i < channel_size; i++)
	{
		Serial.read(reinterpret_cast<char*>(&tmp), sizeof tmp);
		ACF[i] = tmp;
	}

	//Print ACF Array
	std::ofstream outfile(filename, std::ios::out);
	for(unsigned int i = 0; i < channel_size; i++)
	{
		std::cerr << ACF[i] << ", ";
		outfile << ACF[i] << ", ";
	}
	std::cerr << '\n';
}
*/

/*
int main()
{

	read_acf("0.dat");


	//Open File
	std::ifstream file("fluorescein_1us0.txt", std::ios::in);

	

	//Loop while !EOF
	bool EOFflag = false;
	while(!EOFflag)
	{
		//Load X points
		counter_t tmp_disc = 0;
		for(unsigned int i = 0; i < LOAD_POINTS; i++)
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
		std::cerr << "First : " << TraceData[0] << "    Last: " << TraceData[LOAD_POINTS-1] << '\n';
		//Push X points
		for(unsigned int i = 0; i < LOAD_POINTS; i++)
		{
			multitau.push_datum(TraceData[i]);
		}
		std::cerr << ".";

	}
	std::cerr << "\n";
	read_acf("lin.dat");
}*/



//TEST 6. MonitorChannel
#include "monitor_channel.hpp"

int main()
{
	MonitorChannel<true> mon;
}

#endif // All the code is blocked inside CORR_SIMULATOR
