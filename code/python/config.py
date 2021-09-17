import json

default_config =  { "GateTime_us": 5,
                    "AllowedGateTimeError_us": 0.1,
                    "LinCorrs": 9,
                    "SeriesSize": 27,
                    "BinRatio": 3,
                    "LiveGraph": False,
                    "SamplingDelay_ms": 50,
                    "Baud": 9600,
                    "Port": "",
                    "ISRPinToggle": True,
                    "AbortOnError": True,
                    "//# Comment"  : "Missing fields will generate warnings and a default value will be added to the configuration."
                     }


def generate_config_file():
  ''' Generates the json configuration file in the current working directory '''

  j_config = json.dumps(config, indent=4)
  with open('config.json', 'w') as file:
      file.write(j_config)
  print(f" Added Config file `config.json` with default multi-tau parameters <{default_config['LinCorrs']},{default_config['SeriesSize']}, {default_config['BinRatio']}>.")

def validate_config_file(config_file):
  ''' Checks for missing fields in the configuration file and adds default values, if missing fields are found.'''
  
  update_state = False

  with open('./config.json', 'r') as f:
    config = json.load(f)
  
  for key in default_config:
    if not key in config:
      config[key] = default_config[key] #Add
      print(f"[Warning] Config Validation : Missing field added -> {key}: {config[key]}")
      update_state = True

  if update_state:
      j_config = json.dumps(config, indent=4)
      with open('./config.json', 'w') as file:
          file.write(j_config)