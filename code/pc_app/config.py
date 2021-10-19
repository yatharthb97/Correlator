import json
import numpy as np
import sys

default_config =  { 
                    "Live Graph": False,
                    "Sampling Delay ms": 200,
                    "Baud": 115200,
                    "Port": "",
                    "ISR Pin Toggle": True,
                    "Abort On Error": True,
                    "CPU Clock Freq MHz" : 600,
                    "Some Debug Value" : 1.23456,
                    "Lights Off" : False,
                    

                    "Feature Line" : "ACF",
                    "Enable Sync Check": False,
                    "Sync Code": int(np.int32(-123456789)),


                    "Gate Time us": 5,
                    "Allowed Gate Timing Error us": 0.1,


                    "Enable ACF": True,
                    "Log xAxis ACF": False,
                    
                    "MT Linear Correlators(LCs)": 9,
                    "MT LC Series Size": 27,
                    "MT Bin Ratio": 3,
                    
                    "Enable Count Rate(CR)": False,
                    "CR Coarse Graining Interval s" : 1.0,
                    
                    "Enable Points Norm" : True,
                    "Enable Mean Norm" : True,
                    
                    "Enable Photon Count Histogram": False,
                    "PC Histogram Bins": 10,
                    
                    "Enable Performance Counters":True,

                    "Instructions": 
                    { 
                      0 : "Feature Lines: {`ACF`, `Interarrival`, `Sampler`}.",
                      1 : "Missing fields will generate warnings and a default value will be added to the configuration.",
                      2 : "SomeDebug value is set as a macro to help debug build system, macro conversion of different data types, etc.",
                      3 : "Enabling Interarrival Mode will disable all other functionality.",
                      4 : "Options specific to a Feature Line are ignored unless that Feature Line is selected.",
                      5 : "The address for the emulator's virtual Port is `/dev/corr_virtual_serial_port` (only for linux)."
                    }
                  }


def config_validation(config):
  
  def RED(string):
    return f"\x1B[31m{string}\x1B[0m"

  # V1
  if config['Enable Mean Norm'] and (not config['Enable Points Norm']):
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

