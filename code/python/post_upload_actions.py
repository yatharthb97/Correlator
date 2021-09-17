import time
import threading
import json
import numpy as np

import serial.tools.list_ports as port_list
from serial import Serial, SerialException

from multitau import x_tics, points_norm_template
from utilities import PseudoConsole



print("Loading post script...")


# Load Configurarion file
with open('./../../config.json', 'r') as f:
	config = json.load(f)


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


# Open Serial Port
port = Serial(port=config["Port"], baudrate=config["Baud"], timeout=0.5)
if not port.is_open:
	raise Exception("Port could not be opened.")


# Set Live Graph if enabled
if config["LiveGraph"] == True:
	#Create Receiver object to the com port

	from pyqtgraph.Qt import QtGui, QtCore
	import pyqtgraph as pg

	app = QtGui.QApplication([])

	window = pg.GraphicsLayoutWidget(show=True, title= f"Auto-Correlation Plot - {config['Port']}")
	window.resize(1000,600)
	window.setWindowTitle("Auto-Correlation Plot")

	# Enable antialiasing for prettier plots
	pg.setConfigOptions(antialias=True)

	graph = window.addPlot(title="Auto-Correlation Plot")
	curve = graph.plot(pen='y')
	

#Defines update function
def update_fn():
	if port.in_waiting() > 0:
		raw_data = port.readline()
		data = np.frombuffer(raw_data, dtype = 'i4')

		# Points is the number of points read by LC0
		# raw_data = port.readline()
		# no_points = np.frombuffer(raw_data, dtype = '<i4')
		# data = np.frombuffer(raw_data, dtype = '<i4', offset = 4)
		# current_norm  = norm_values * np.unsignedinteger(no_points)
			
		normed_y = data / norm_values
		datafile.write(str(normed_y))
		np.savetxt(datafile, normed_y.flatten(), newline = ", ")

		if config["LiveGraph"] == True:
			curve.setData(x_tau_values, normed_y)


# Set Sampling Rate
if 'SamplingDelay_ms' in config:
	wait_time_ms = config['SamplingDelay_ms']
else:
	wait_time_ms = 50 

# Thread Division
if config["LiveGraph"] == True:
	timer = QtCore.QTimer()
	timer.timeout.connect(update_fn)
	timer.start(wait_time_ms)
	app.exec_()
else: 
	timer = threading.Timer(wait_time_ms * 0.001, update_fn)	
	timer.start()
	PseudoConsole()



print("Closing post script...")
# Stop Procedure
if config["LiveGraph"] == True:
	timer.stop()
else:
	timer.cancel()
datafile.close()
port.close()
