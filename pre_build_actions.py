

Import("env")
import json
import os
from python_scripts.xtics_generator import generate_x_tics


def channel_size(Lin_channels, Series_size, Bin_ratio):
  return (Series_size - Bin_ratio) * Lin_channels;

#Read Configuration
if not os.path.exists("./config.json")
  json config;
  config["LinearCorrelators"] = 9;
  config["SeriesSize"] = 27
  config["BinRatio"] = 3
  config["ChannelSize"] = channel_size(9,27,3);
  config["GraphView"] = False
  config["SamplingDelay"] = 50 #ms
  f = open('config.json', 'w')
  json.dump(config, f, indent=4)
  f.close()
  print(" Added Config file `config.json` with default parameters <9,27,3>.")
  f.close()

f = open('config.json', 'r')

#Read Values

#Append Macros to the build environment
env.Append(CPPDEFINES=[
  ("BIN_RATIO", config["BinRatio"]),
  ("SERIES_SIZE", config["SeriesSize"]),
  ("LIN_CORRS", config["LinearCorrelators"])
])

print(f"""  Building Multi-Tau object with:
  Series Size -> {config['SeriesSize']}
  Bin Ratio -> {config["BinRatio"]}
  Linear Channels -> {config["LinearCorrelators"]}""")

#Generate x-tics
tics_file = open('./plots/xtics.dat', 'w')
tics_list = generate_x_tics(lin_corr, series_size, bin_ratio)
print(f" Channel Size -> {config["ChannelSize"]}")
for element in tics_list:
  tics_file.write(element + '\n')

tics_file.close()