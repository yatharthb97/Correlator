import time
import threading
import json
import numpy as np
import math
import serial.tools.list_ports as port_list
from serial import Serial, SerialException

from multitau import x_tics, points_norm_template, channel_size
from utilities import PseudoConsole, WelcomeText, periodic_callbacks



WelcomeText()


# Load Configurarion file
with open('./../../config.json', 'r') as f:
	config = json.load(f)
channel_size = multitau.channel_size(config['LinCorrs'], config['SeriesSize'], config['BinRatio'])
byte_size = 4 # No of bytes used by the incoming data for one value

x_tau_values = x_tics(config['LinCorrs'], config['SeriesSize'], config['BinRatio'])

#Generate Normalization
norm_values = points_norm_template(config['LinCorrs'], config['SeriesSize'], config['BinRatio'])


 
#Set Files
name_ = str(time.time_ns())
y_file_name = f"{name_}_y.dat"
x_file_name = f"{name_}_x_taus.dat"
print(f"Saving data to: ** [x: {x_file_name} , y: {y_file_name}]  **")
with open(x_file_name, 'w') as f:
	np.savetxt(f, x_tau_values.flatten(), fmt='%s', newline = ", ")
datafile = open(y_file_name, 'a')

# Set Live Graph if enabled
if config["LiveGraph"] == True:
	#Create Receiver object to the com port

	from pyqtgraph.Qt import QtGui, QtCore
	import pyqtgraph as pg
	
	app = QtGui.QApplication([])
	
	
	window = pg.GraphicsLayoutWidget(show=True, title= f"Auto-Correlation Plot - {config['Port']}")
	window.resize(1440,720)
	window.setWindowTitle("Auto-Correlation Plot")


	# icon
	icon = QtGui.QIcon("skin.jpg")
	window.setWindowIcon(icon)


	# Enable antialiasing for prettier plots
	pg.setConfigOptions(antialias=True, useOpenGL=True)

	canvas = window.addPlot(title="Auto-Correlation Plot → 0")
	window.setCentralWidget(canvas)

	

	x_right_padding = math.pow(10, math.floor(math.log(x_tau_values[-1], 10))/2)
	#canvas.setXRange(0, x_tau_values[-1])# + x_right_padding)

	#canvas.scaleBy(s=None, center=None, x=None, y=None)
	curve = canvas.plot(pen='y', symbolBrush='c', symbolSize=4)
	curve.setLogMode(False, False)
	canvas.showGrid(x = True, y = True)
	canvas.setLabel('left', 'ACF', 'G(Tau)')
	canvas.setLabel('bottom', 'Lag', 'Tau')
	canvas.setRange(xRange=(0, x_tau_values[-1]), yRange=None, padding=None, update=True, disableAutoRange=True)



# Open Serial Port
port = Serial(port=config["Port"], baudrate=config["Baud"], timeout=None)
if not port.is_open:
	raise Exception("Port could not be opened.")
	

#Defines update function
update_id = 0
time_init = time.perf_counter()
time_stop = time.perf_counter()
def update_fn():
	if port.in_waiting >= channel_size * byte_size:
		raw_data = port.read(channel_size * byte_size)
		data = np.frombuffer(raw_data, dtype = np.uint32, count = channel_size)
		global update_id
		update_id = update_id + 1
		# Points is the number of points read by LC0
		# raw_data = port.readline()
		# no_points = np.frombuffer(raw_data, dtype = '<i4')
		# data = np.frombuffer(raw_data, dtype = '<i4', offset = 4)
		# current_norm  = norm_values * np.unsignedinteger(no_points)
		normed_y = data #/ norm_values
		np.savetxt(datafile, normed_y.flatten(), fmt='%s', newline = ", ")
		datafile.write('\n')

		if config["LiveGraph"] == True:
			global time_init, time_stop
			time_stop = time.perf_counter()
			canvas.setTitle(title=f"Auto-Correlation Plot → {update_id} - Update time: {(time_stop - time_init): .3f} s")
			curve.setData(x_tau_values, normed_y)
			time_init = time.perf_counter()


# Set Sampling Rate
if 'SamplingDelay_ms' in config:
	wait_time_ms = config['SamplingDelay_ms']
else:
	wait_time_ms = 50 

print(f"""Sampling Time: {wait_time_ms}\nPort: {config['Port']}""")


stop_code_asserted = False
# Thread Division
if config["LiveGraph"] == True:
	print("••• Launching Live Graph → Close graph window to end measurement.")
	timer = QtCore.QTimer()
	timer.timeout.connect(update_fn)
	timer.start(wait_time_ms)
	app.exec_()
else: 
	
	def periodic_callback_fn(period_ms=wait_time_ms):
		period_s = period_ms * 0.001 #Convert to seconds
		def g_tick():
			t = time.time()
			while True:
				t += period_s
				yield max(t - time.time(), 0) #Avoid negative values
		sleep_time_gen = g_tick()
		global stop_code_asserted
		while not stop_code_asserted:
			time.sleep(next(sleep_time_gen))
			update_fn()
	
	timer = threading.Thread(target=periodic_callback_fn)
	timer.start()
	print(f"Measurement Thread -> {timer.is_alive()*'Alive' + (not timer.is_alive()) * 'Dead' }")
	PseudoConsole()




# Stop Procedure
if config["LiveGraph"] == True:
	timer.stop()
else:
	stop_code_asserted = True
	print(f"Waiting for measurement thread to join...")
	timer.join()

datafile.close()
port.close()
print(f"•••> Exiting: Updates read: {update_id}")

