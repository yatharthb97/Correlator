#!/usr/bin/env python3
#!/usr/bin/env python


#Call this script as → ./test.py <filename> <board(optional)> 
#to compile and upload test cases to board
#Author - Yatharth Bhasin


import subprocess
import sys
import os

if len(sys.argv) < 2:
    print("Incomplete arguments!")
    exit()



if sys.argv[1] == "clean":
    clean = subprocess.run(['pio', 'run', '-t', 'clean'])
    exit()

filename = sys.argv[1]

if len(sys.argv) > 2:
    board = sys.argv[2]
else:
    board = "teensy41"


#command = f"pio ci {filename} --lib=\"./src\" --board={board} --src_filter=\"./src/exclude/\""

#Compile
command_vec = ["pio", "ci", filename, f"--board={board}", "--project-conf='./plateformio.ini'"]
print(f"Executing → {command_vec}")
compile = subprocess.run(command_vec)


#If compiles → Upload
if compile.returncode != 0:
    print(f"Compilation failed with process vector: {compile}")
else:
    upload = subprocess.run(["pio", "run", '-t', 'upload'])
    if upload.returncode == 0:
        print("Upload Successful!")
    else:
        print(f"Upload Failed with process vector: {upload}")


###############################################


# if linux:
#     subprocess.call(['sh', command])
# else if windows:
#     subprocess.call(['powershell', command])



#pio ci ./tests/ttl_count.cpp --lib="./src" --board=teensy41
#pio ci ./tests/blink_inbuilt_4.cpp --lib="./src" --board=teensy41
#pio ci ./tests/pit_toggle.cpp --lib="./src" --board=teensy41
