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
ch_size = channel_size(config['MT Linear Correlators(LCs)'], config['MT LC Series Size'], config['MT Bin Ratio'])

fl = config['Feature Line']
feature_line_code = (fl == "ACF")*1 + (fl == "Interarrival")*2 + (fl == "Sampler")*3


#4. Append Macros to the build environment
env.Append(
CPPDEFINES=[
              ("BIN_RATIO", config["MT Bin Ratio"]),
              ("SERIES_SIZE", config["MT LC Series Size"]),
              ("LIN_CORRS", config["MT Linear Correlators(LCs)"]),
              ("CHANNEL_SIZE", ch_size),

              ("GATE_TIME_US", config["Gate Time us"]),
              ("ALLOWED_GATE_TIME_ERROR_US", config["Allowed Gate Timing Error us"]),
              ("SAMPLING_TIME_MS", config["Sampling Delay ms"]),
              ("ISR_PIN_TOGGLE", int(config["ISR Pin Toggle"])),
              ("ABORT_ON_ERROR", int(config["Abort On Error"])),
              
              ("ENABLE_COUNT_RATE", int(config["Enable Count Rate(CR)"])),
              ("ENABLE_SYNC_CODE", int(config["Enable Sync Check"])),
              ("SYNC_CODE", config["Sync Code"]),
              ("ENABLE_ACF_CALC", int(config["Enable ACF"])),
              ("F_CPU", int(config["CPU Clock Freq MHz"]*1000000)),
              ("CR_CG_INTERVAL_US", config["CR Coarse Graining Interval s"]*1e6), #Converted to microseconds
              ("DEBUG_VALUE", config["Some Debug Value"]),
              ("ENABLE_POINTS_NORM", int(config["Enable Points Norm"])),
              ("ENABLE_MEAN_NORM", int(config["Enable Mean Norm"])),
              ("ENABLE_PC_HISTOGRAM", int(config["Enable Photon Count Histogram"])),
              ("PC_HISTOGRAM_BINS", int(config["PC Histogram Bins"])), 
              ("ENABLE_PERFORMANCE_COUNTERS", int(config["Enable Performance Counters"])),
              
              #FeatureLine
              ("DEVICE_FEATURELINE", feature_line_code),
              ("LIGHTS_OFF", config['Lights Off'])
            ])




#5. Print Configuration Information
print(f"Feature Line in use -> {config['Feature Line']}\n")

if config["Feature Line"] == "ACF":
  print(f"""
  ••• Building Multi-Tau object •••
     ACF Calculation -> {config['Enable ACF']}
         Series Size -> {config['MT LC Series Size']}
           Bin Ratio -> {config['MT Bin Ratio']}
  Linear Correlators -> {config['MT Linear Correlators(LCs)']}
        Channel Size -> {ch_size}
          Count Rate -> {config['Enable Count Rate(CR)']}
           Sync Code -> {config['Enable Sync Check']}
  """)

#print(env.Dump())




#6. Define Post Upload Action
def after_upload_action(*args, **kwargs):
  print("Post upload script called...")
  os.system('python ./code/python/post_upload_actions.py')
  #exec(open('./code/python/post_upload_actions.py').read())





#7. Attach Post Upload Action
#env.AddPostAction("upload", after_upload_action)