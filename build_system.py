import json
import os
import subprocess

import sys
sys.path.append("./code/pc_app")
from multitau import channel_size
import config as configr

Import("env")

#1. Check for configuration file
if not os.path.exists("./config.json"):
  configr.generate_config_file()


#2.0. Validate and read configuration file
configr.fill_missing_in_config_file('./config.json')
f = open('./config.json', 'r')
config = json.load(f)
f.close()

#2.1 config validation
configr.config_validation(config)


#3. Parameter Conversions
ch_size = channel_size(config['LinCorrs'], config['SeriesSize'], config['BinRatio'])

fl = config['Feature Line']
feature_line_code = (fl == "ACF")*1 + (fl=="Interarrival")*2 + (fl=="Sampler")*3


#4. Append Macros to the build environment
env.Append(
CPPDEFINES=[
              ("BIN_RATIO", config["BinRatio"]),
              ("SERIES_SIZE", config["SeriesSize"]),
              ("LIN_CORRS", config["LinCorrs"]),
              ("CHANNEL_SIZE", ch_size),

              ("GATE_TIME_US", config["GateTime_us"]),
              ("ALLOWED_GATE_TIME_ERROR_US", config["AllowedGateTimeError_us"]),
              ("SAMPLING_TIME_MS", config["SamplingDelay_ms"]),
              ("ISR_PIN_TOGGLE", int(config["ISRPinToggle"])),
              ("ABORT_ON_ERROR", int(config["AbortOnError"])),
              
              ("ENABLE_COUNT_RATE", int(config["EnableCountRate"])),
              ("ENABLE_SYNC_CODE", int(config["EnableSyncCode"])),
              ("SYNC_CODE", config["SyncCode"]),
              ("ENABLE_ACF_CALC", int(config["EnableACFCalc"])),
              ("F_CPU", int(config["CPUClockFrequency_MHz"]*1000000)),
              ("CR_CG_INTERVAL_US", config["CRCoarseGrainingInterval_s"]*1e6),
              ("DEBUG_VALUE", config["SomeDebugValue"]),
              ("ENABLE_POINTS_NORM", int(config["EnablePointsNorm"])),
              ("ENABLE_MEAN_NORM", int(config["EnableMeanNorm"])),
              ("ENABLE_PC_HISTOGRAM", int(config["Enable PC Histogram"])),
              ("PC_HISTOGRAM_BINS", int(config["PC Histogram Bins"])), 
              ("ENABLE_PERFORMANCE_COUNTERS", int(config["Enable Performance Counters"])),
              
              #FeatureLine
              ("DEVICE_FEATURELINE", feature_line_code)
            ])




#5. Print Configuration Information
print(f"Feature Line in use -> {config['Feature Line']}\n")

if config["Feature Line"] == "ACF":
  print(f"""
  ••• Building Multi-Tau object •••
     ACF Calculation -> {config['EnableACFCalc']}
         Series Size -> {config['SeriesSize']}
           Bin Ratio -> {config['BinRatio']}
  Linear Correlators -> {config['LinCorrs']}
        Channel Size -> {ch_size}
          Count Rate -> {config['EnableCountRate']}
           Sync Code -> {config['EnableSyncCode']}
  """)

#print(env.Dump())




#6. Define Post Upload Action
def after_upload_action(*args, **kwargs):
  print("Post upload script called...")
  os.system('python ./code/python/post_upload_actions.py')
  #exec(open('./code/python/post_upload_actions.py').read())





#7. Attach Post Upload Action
#env.AddPostAction("upload", after_upload_action)