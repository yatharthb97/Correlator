#!/usr/bin/python3


import sys
import os
import time
import threading
import json

import numpy as np
from collections import deque
from pyqtgraph.Qt import QtCore
import pyqtgraph as pg
import serial.tools.list_ports as port_list
from serial import Serial, SerialException

import multitau
from normalizer import Normalizer
from utilities import PseudoConsole, WelcomeText, StructRepresentation, OffsetTracker
from livegraph import LiveGraph

#0. Set name and print Welcome Text
if len(sys.argv) > 1:
	name_ = sys.argv[1]
else:
	name_ = str(time.time_ns())
WelcomeText(name_) #Print HeaderText




#1. Load Configurarion file
with open('./../../config.json', 'r') as f:
	config = json.load(f)


#Temporary insertions in `config` are lowercase and underscored.


#1.1 Fix Parameters
multitau_param_set = [config['MT Linear Correlators(LCs)'], config['MT LC Series Size'], config['MT Bin Ratio']]
this_channel_size = multitau.channel_size(*multitau_param_set)
config['channel_size'] = this_channel_size

byte_size = 4 # No of bytes used by the incoming data for one value both float and uint

if config["Feature Line"] == "ACF":
	total_struct_size = (	(config['Enable ACF'] * this_channel_size) + 
							config['Enable Count Rate(CR)'] +
							config['Enable Points Norm'] +
							config['Enable Sync Check'] +
							config['Enable Mean Norm'] + 
							config['Enable Photon Count Histogram'] * config['PC Histogram Bins'] +
							((config['Enable ACF'] + 1) * config['Enable Performance Counters'])
						) * byte_size

elif config['Feature Line'] == "Interarrival":
	total_struct_size = (config['Enable Sync Check'] + 1) * byte_size
elif config['Feature Line'] == "Sampler":
	total_struct_size = 1 #TODO Fix
else:
	print(f"[ERROR] Invalid Feature Line - {config['Feature Line']}")
	sys.exit(1)

stop_code_asserted = False # True means that the user has asserted the end of measurement
update_id = 0 # Set update ID to zero
openfilelist = [] #List of open file references
parent_dir = "./runs/" #Parent directory  where file will be saved. Default is `cwd`.
DATASEP = ','


#1.2 Clocks
now_tmp = time.perf_counter()

#Time values used for measuring inter-arrival of updates
update_time_start = now_tmp
update_time_stop = now_tmp

# Time values used for checkpointing complete measurement interval 
measure_clock_start = now_tmp
measure_clock_stop = now_tmp


#2. Generate auxillary data
x_tau_values = multitau.x_tics(*multitau_param_set)
normalizer = Normalizer(*multitau_param_set)
norm_mode = ("no" * (not config["Enable Points Norm"])) + ("points"  * (not config["Enable Mean Norm"]) * config["Enable Points Norm"]) + ("mean" * config["Enable Mean Norm"] * config["Enable Points Norm"]) #Mean Norm
normalizer.set_mode(norm_mode)
norm_args = [] #List of arguements required for normalization operation





#3. Print Info
print(f"""Sampling Time: {config['Sampling Delay ms']} ms on Port: {config['Port']}""")
print(f" ??? Synchronization Check ??? {config['Enable Sync Check']}")
print(f" ??? Count Rate Mode ??? {config['Enable Count Rate(CR)']}")
print(f" ??? ACF Mode ??? {config['Enable ACF']}")
print(f" ??? Normalization Mode ??? {norm_mode}")
struct_rep = StructRepresentation(config)
print(f"Receiving Data Struct({total_struct_size} bytes):\n{struct_rep}")




# 4. Specialized Resource Init
time_x = deque(maxlen = 100)
time_x.append(0)

count_rate_y = deque(maxlen = 100)
count_rate_y.append(0)

mean_int_y = deque(maxlen = 100)
mean_int_y.append(0)
	

pf_serial_y = deque(maxlen = 100)
pf_serial_y.append(0)

pf_acf_y = deque(maxlen = 100)
pf_acf_y.append(0)

#5. Set Files
if config['Enable ACF']:
	y_file_name = f"{name_}_y.dat"
	x_file_name = f"{name_}_x_taus.dat"
	print(f"Saving ACF data to: ?????????> [x: {x_file_name} , y: {y_file_name}]  <?????????")
	
	#Tau values
	with open(os.path.join(parent_dir, x_file_name), 'w') as f:
		np.savetxt(f, x_tau_values.flatten(), fmt='%s', newline = ", ")

	with open(os.path.join(parent_dir, y_file_name), 'w') as f: # Clear old file
		pass
	acffile = open(os.path.join(parent_dir, y_file_name), 'a') # Open file for saving live ACF data
	openfilelist.append(acffile)

if config['Enable Count Rate(CR)']:
	count_file_name = f"{name_}_countrate.dat"
	countratefile = open(os.path.join(parent_dir, count_file_name), 'w')
	openfilelist.append(countratefile)
	print(f"Saving count rate to: ?????????> {count_file_name} <?????????")

if config['Enable Mean Norm']:
	mean_file_name =  f"{name_}_meanintensity.dat"
	meanfile = open(os.path.join(parent_dir, mean_file_name), 'w')
	openfilelist.append(meanfile)
	print(f"Saving mean intensity to: ?????????> {mean_file_name} <?????????")






#6. Set Live Graph if enabled [[TODO - Refactoring]]
if config["Live Graph"] == True:
	live_graph = LiveGraph(xRange=x_tau_values[-1], port=config['Port'], title="Auto-Correlation Function", x_label = "Lag", y_label="ACF", x_units="Tau", y_units="G(Tau)", config=config )

	if config['Enable Count Rate(CR)'] == True:
		live_graph['window'].resize(1440,960)
		live_graph['window'].nextRow()
		live_graph['canvas2'] = live_graph['window'].addPlot(title="Count Rate ??? ??? kHz [Update 0]", row=4, col=0, rowspan=1)
		live_graph['curve_cnt_rate'] = live_graph['canvas2'].plot(pen='c', symbolBrush='r', symbolSize=4)
		live_graph['canvas2'].setLabel('left', 'I(t)', 'Hz')
		live_graph['canvas2'].setLabel('bottom', 'time', 's')
		live_graph['canvas2'].setPos(time_x[0], 0)

		#live_graph.canvas['Count Rate'].setPos(time_x[0], 0)



#7. Open Serial Port
try:
	port = Serial(port=config["Port"], baudrate=config["Baud"], timeout=None)
except (SerialException, FileNotFoundError) as e:
	print(f"[ERROR] Port {config['Port']} could not be opened. Exiting!")
	sys.exit(1)





#8. Defines update function - Coroutine
def update_fn():
	if port.in_waiting >= total_struct_size:
		#print(f"Inwaiting: {port.in_waiting}")
		
		#Begin new read
		norm_args.clear()


		try:
			raw_data = port.read(total_struct_size)
			#print(raw_data)
		except SerialException:
			if config['Live Graph']:
				print(f"[ERROR] Serial Device has been disconnected. Please close the LiveGraph to terminate the program.")
			else:
				print(f"[ERROR] Serial Device has been disconnected. The program will terminate.")
				stop_code_asserted = True
		
		# Calc update_time and update_id
		global update_id, measure_clock_start
		measure_clock_now = time.perf_counter()
		measurement_time = measure_clock_now - measure_clock_start
		measurement_time = max(0, measurement_time)
		time_x.append(measurement_time)
		update_id = update_id + 1

		offsetr = OffsetTracker()

		# SYNCHRONIZATION CODE
		if config['Enable Sync Check']:
			sync_code = np.frombuffer(raw_data, dtype = np.int32, count = 1, offset=offsetr.advance(byte_size))

			if sync_code[0] != np.int32(config['Sync Code']):
				print(f"[SYNC ERROR] Sync Code Mismatch! - Update ID: {update_id} - rec:{(sync_code[0])} vs set:{np.int32(config['SyncCode'])}")


		# INTERARRIVAL
		if config['Feature Line'] == 'Interarrival':
			ia_stats = np.frombuffer(raw_data, dtype = np.uint32, count = 1, offset=offsetr.advance(byte_size))
			print(f"{measurement_time:.3f} -   {ia_stats}")


		elif config['Feature Line'] == 'ACF':
			# COUNT RATE
			if config['Enable Count Rate(CR)']:
				count_rate = np.frombuffer(raw_data, dtype = 'f4', count = 1, offset=offsetr.advance(byte_size))
				count_rate = count_rate[0] * 1.0 / config['CR Coarse Graining Interval s'] # Adjust to 1s
				
				countratefile.write(f"{measurement_time: .5f}{DATASEP}{count_rate :.3f}\n")

				#Push Count Rate to live graph
				if config['Live Graph']:
					count_rate_y.append(count_rate)
					live_graph['canvas2'].setTitle(title=f"Count Rate ??? {count_rate/1000: .3f} kHz")
					live_graph['curve_cnt_rate'].setData(time_x, count_rate_y)

			# POINTS NORM
			if config['Enable Points Norm']:
				points_norm = np.frombuffer(raw_data, dtype = np.uint32, count = 1, offset=offsetr.advance(byte_size))
				points_norm = points_norm[0]
				norm_args.append(points_norm)
				print(f"{update_id} - {points_norm}")

			# MEAN NORMALIZATION - ACCUMULATE
			if config['Enable Mean Norm']:
				accumulate = np.frombuffer(raw_data, dtype = np.uint32, count = 1, offset=offsetr.advance(byte_size))
				accumulate = accumulate[0]
				norm_args.append(accumulate)
				if points_norm != 0:
					mean = accumulate / points_norm
					meanfile.write(f"{measurement_time}{DATASEP}{mean}\n")
			
			# ACF CHANNEL
			if config['Enable ACF']:
				data = np.frombuffer(raw_data, dtype = 'f4', count = this_channel_size, offset=offsetr.advance(this_channel_size*byte_size))
				normed_y = data
				normed_y = normalizer.normalize(data, *norm_args)
				np.savetxt(acffile, normed_y.flatten(), fmt='%f', newline = ", ") #delimiter=',', newline='\n'
				acffile.write('\n') #TODO - Fix this ugly shit

				#Push ACF to LiveGraph
				if config["Live Graph"] == True:
					live_graph['curve'].setData(x_tau_values, normed_y)

			# Histogram
			# if config['Enable PC Histogram']:
			# 	offset = (config['EnableSyncCode'] + config['EnableCountRate'] + config['EnablePointsNorm'] + config['EnableMeanNorm'] + 
			# 			  (channel_size * config['EnableACFCalc'])
			# 			  ) * byte_size
			# 	count = config['PC Histogram Bins'] + 1
			# 	hist = np.frombuffer(raw_data, dtype = np.uint32, count = count, offset=offset)
			# 	#Does not save to file TODO
			# 	if config["LiveGraph"]:
			# 		live_graph.curve["PCHistogram"].setData(live_graph.hist_x, hist)

			# Performance Counters
			# if config['Enable Performance Counters']:
			# 	offset = (config['EnableSyncCode'] + config['EnableCountRate'] + config['EnablePointsNorm'] + config['EnableMeanNorm'] + 
			# 			  (channel_size * config['EnableACFCalc']) + (congif['Enable PC Histogram']*(config['PC Histogram Bins']+1))
			# 			  ) * byte_size
			# 	count = (config['EnableACFCalc'] + 1)
			# 	pf = np.frombuffer(raw_data, dtype = np.uint32, count = count, offset=offset)
			# 	pf_serial_y.append(pf[0])

			# 	if config['EnableACFCalc']:
			# 		pf_acf_y.append(pf[1])

			# 	if ['LiveGraph']:
			# 		live_graph.curve['PerfCounter Serial'].setData(time_x, pf_serial_y)
			# 		if ['EnableACFCalc']:
			# 			live_graph.curve['PerfCounter ACF'].setData(time_x, pf_acf_y)

		# Update GUI Header
		if config['Live Graph']:
			global update_time_start, update_time_stop
			update_time_stop = time.perf_counter()
			live_graph['gl_title'].setText(f'Photon Statistics: Update ??? {update_id}    |    Update time ??? {(update_time_stop - update_time_start):.3f} s    |    Time Elapsed ??? {time_x[-1] :.1f} s', size='12pt')
			update_time_start = time.perf_counter()


# 9. Reset timers/clocks
now_tmp = time.perf_counter()
measure_clock_start = now_tmp
update_time_start = now_tmp


# 10. Thread Division and Launching
measurement_sampling_delay = int(config['Sampling Delay ms']/2) # Twice as fast as transmission
if config["Live Graph"] == True:
	print("????????? Launching Live Graph ??? Close graph window to end measurement.")
	measurement_thread = QtCore.QTimer()
	measurement_thread.timeout.connect(update_fn)
	measurement_thread.start(measurement_sampling_delay)
	live_graph['app'].exec_()
else: 
	# Specailised Implementation of periodic callback function
	def periodic_callback_fn(period_ms=measurement_sampling_delay):
		period_s = period_ms * 0.001 # Convert to seconds
		def g_tick():
			t = time.time()
			while True:
				t += period_s
				yield max(t - time.time(), 0) #Avoid negative values
		sleep_time_gen = g_tick()
		update_fn() # Call once
		global stop_code_asserted
		while not stop_code_asserted: 
			time.sleep(next(sleep_time_gen))
			update_fn() # Call periodically, until `stop_code_asserted`
	
	measurement_thread = threading.Thread(target=periodic_callback_fn)
	measurement_thread.start()
	print(f"Measurement Thread -> {measurement_thread.is_alive()*'Alive' + (not measurement_thread.is_alive()) * 'Dead' }")
	PseudoConsole()




# 11.0 Stop Procedure - Stop measurement thread
if config["Live Graph"]:
	measurement_thread.stop()
else:
	stop_code_asserted = True
	print(f"Waiting for measurement thread to join...")
	print(f"Measurement Thread -> {measurement_thread.is_alive() * 'Alive' + (not measurement_thread.is_alive()) * 'Dead' }")
	measurement_thread.join()

# 11.1 File Close
for file in openfilelist:
	file.close()
port.close()

# 11.2 Summary
received_data_estimate = total_struct_size * update_id
print(f"""?????????> Exiting: Updates read: {update_id}
              Received data: {received_data_estimate} bytes """)