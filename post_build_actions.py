
import json
import numpy as np

json config;
f = open('config.json', 'r')

#Load x-tics
x_tau_values = np.fromtxt(os.path.join("./plots", "xtics.dat"), unpack = True)


#Now Create a graph instance

#Pass x tics

if config["GraphView"]:
	#Create Receiver object to the com port
	port = Receiver(Baud = $monitor_speed, $port, $upload_port)

	port.graph_y(xvals = x_tau_values) #Sets axis = 1
	port.graph_config([])
	port.open_graph(config["SamplingDelay"])

