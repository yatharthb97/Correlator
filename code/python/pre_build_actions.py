import json
import os
import subprocess

from multitau import channel_size
import config

Import("env")

#1. Check for configuration file
if not os.path.exists("./config.json"):
  from config import generate_config_file
  generate_config_file()


#2. RValidate and read configuration file
config.validate_config_file('./config.json')
f = open('./config.json', 'r')
config = json.load(f)
f.close()


ch_size = channel_size(config['LinCorrs'], config['SeriesSize'], config['BinRatio'])

#3. Append Macros to the build environment
env.Append(
CPPDEFINES=[
              ("BIN_RATIO", config["BinRatio"]),
              ("SERIES_SIZE", config["SeriesSize"]),
              ("LIN_CORRS", config["LinCorrs"]),
              ("CHANNEL_SIZE", ch_size),
              ("GATE_TIME", config["GateTime_us"]),
              ("ALLOWED_GATE_TIME_ERROR", config["AllowedGateTimeError_us"]),
              ("ISR_PIN_TOGGLE", config["ISRPinToggle"]),
              ("ABORT_ON_ERROR", config["AbortOnError"])
            ])

#4. Print Configuration Information
print(f"""
Building Multi-Tau object with:
Series Size -> {config['SeriesSize']}
Bin Ratio -> {config["BinRatio"]}
Linear Correlators -> {config["LinCorrs"]}
Channel Size -> {ch_size}
""")


#5. Define Post Upload Action
def after_upload_action(*args, **kwargs):
  print("Post upload script called...")
  os.system('python ./code/python/post_upload_actions.py')
  #exec(open('./code/python/post_upload_actions.py').read())

#6. Attach Post Upload Action
env.AddPostAction("upload", after_upload_action)