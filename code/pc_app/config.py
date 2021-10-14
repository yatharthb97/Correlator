import json
import numpy as np
import sys

default_config =  { 
                    "Feature Line" : "ACF",
                    "EnableACFCalc": True,
                    "Log xAxis ACF": False,
                    "GateTime_us": 5,
                    "AllowedGateTimeError_us": 0.1,
                    "LinCorrs": 9,
                    "SeriesSize": 27,
                    "BinRatio": 3,
                    "LiveGraph": False,
                    "SamplingDelay_ms": 200,
                    "Baud": 115200,
                    "Port": "",
                    "ISRPinToggle": True,
                    "AbortOnError": True,
                    "EnableCountRate": False,
                    "EnableSyncCode": False,
                    "SyncCode": int(np.int32(-123456789)),
                    "CPUClockFrequency_MHz" : 600,
                    "CRCoarseGrainingInterval_s" : 1.0,
                    "SomeDebugValue" : 1.23456,
                    "EnablePointsNorm" : True,
                    "EnableMeanNorm" : True,
                    "Enable PC Histogram": False,
                    "PC Histogram Bins": 10,
                    "Enable Performance Counters":True,
                    "Instructions": 
                    { 
                      0 : 'Feature Lines: {"ACF", "Interarrival", "Sampler"}.',
                      1 : "Missing fields will generate warnings and a default value will be added to the configuration.",
                      2 : "SomeDebug value is set as a macro to help debug build system, macro conversion of different data types, etc.",
                      3 : "Enabling Interarrival Mode will disable all other functionality.",
                      4 : "Options specific to a Feature Line are ignored unless that Feature Line is selected."
                    }
                  }


def config_validation(config):
  
  def RED(string):
    return f"\x1B[31m{string}\x1B[0m"

  # V1
  if config['EnableMeanNorm'] and (not config['EnablePointsNorm']):
    print(RED("[CONFIG ERROR] Points Norm must be enabled for Mean norm to be eanabled."))
    sys.exit(1)

  # V2
  fl = config['Feature Line']
  fl = str(fl)
  if not((fl == "ACF") or (fl == "Interarrival") or (fl == "Sampler")):
    print(RED(f'[CONFIG ERROR] Invalid feature line (device mode) passed: "{fl}".'))
    sys.exit(1)



def generate_config_file():
  ''' Generates the json configuration file in the current working directory '''

  j_config = json.dumps(default_config, indent=4)
  with open('config.json', 'w') as file:
      file.write(j_config)
  print(f" Added Configuration file — `config.json` with default pre-defined parameters.")


def fill_missing_in_config_file(config_file):
  ''' Checks for missing fields in the configuration file and adds default values, if missing fields are found.'''
  
  update_state = False

  with open('./config.json', 'r') as f:
    config = json.load(f)
  
  for key in default_config:
    if not key in config:
      config[key] = default_config[key] #Add
      print(f"[Warning] Configuration : Missing field added -> {key}: {config[key]}")
      update_state = True


  #Validate Values
  config_validation(config)

  if update_state == True:
      j_config = json.dumps(config, indent=4)
      with open('./config.json', 'w') as file:
          file.write(j_config)

