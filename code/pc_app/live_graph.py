#!/usr/bin/python3

from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg
import math

class PStatLiveGraph:	

	def __init__(self, port=None):
		"""
		Constructor : takes optional parameter "port" which adds the port-address to the window title
		"""
		self.app = QtGui.QApplication([])
	
		win_title = f"Photon Statistics"
		if port is not None:
			win_title.join(f" - {this_port}")
		
		self.window = pg.GraphicsLayoutWidget(show=True, title=win_title)
		#self.window.setWindowTitle()
		
		self.window.resize(320, 180)

		# icon
		icon = QtGui.QIcon("./Resource/icon.png")
		self.window.setWindowIcon(icon)


		# Enable antialiasing for prettier plots
		pg.setConfigOptions(antialias=True, useOpenGL=True)

		#Init Dashboard

		self.dashboard = pg.LabelItem(title="Photon Statistics: Update → • |    Update time →  •s    |    Time Elapsed → •s", justify="centre")
		self.window.addItem(self.dashboard, col=0, colspan=0.5)
		self.window.nextRow()

		self.canvas = {} #Dictionary of Canvases
		self.curve = {}  #Dictionary of Curves


	def init_plots(config):
		"""
		Sets plots based on the `config` dictionary.
		"""

		if config['Feature Line'] == "ACF":
			_fL1_acf_(config)
		elif config['Interarrival'] == "Interarrival":
			_fl2_interarrival_(config)
		elif config['Sampler'] == "Sampler":
			_fl3_sampler(config)



		
	def _fL1_acf_(config):
		#Add ACF Plot
		if config['Enable ACF']:
			self.add_canvas_plot('ACF', "Auto-correlation Function", 
								  xLabel = ["Tau", ""], yLabel=["ACF(Tau)", ""],
								  xRange=[0, config['channel_size']]
								)

		
		#Add Count Rate Plot
		if config['Enable Count Rate']:
			self.add_canvas_plot('Count Rate', "Count Rate → • kHz", xLabel = ["time", "s"], yLabel=["Intensity(t)", "Hz"])
		
		#Performance Counters
		if config['Enable Performance Counters']:
			self.add_canvas_plot('PerfCounter Serial', "Performance Counter - Serial", half_plot=config['Enable ACF'], xLabel=["time", "s"], yLabel=["Clock Cycles", ""])

		# Add Performance Counter - dual plot
		if config['Enable PerfCounter'] and config['Enable ACF']:
			self.add_canvas_plot('PerfCounter ACF', "Performance Counter - ACF", half_plot=True, xLabel=["time", "s"], yLabel=["Clock Cycles", ""])


		# Add Histogram
		if config["Enable PC Histogram"]:
			xbins = [str(bin) for bin in range(config["PC Histogram bins"])]
			xbins.append["OVF"] # Overflow Bin
			bargraph = pg.BarGraphItem(x=xbins, y=[0]*config["PC Histogram bins"] ,width = 0.6)

			x, y = self.grid_allocator()
			
			hist_canvas = window.addPlot(title=f"Photon Counting Histogram", row=x, col=y, rowspan=2)
			hist_canvas.addItem(bargraph)

			self.canvas['PCHistogram'] = hist_canvas
			self.curves['PCHistogram'] = bargraph

		#Set Screen Size
		screen_size = self.screen_size_calc(final_x, final_y)
		self.window.resize(*screen_size)


	def _fl2_interarrival_(config):
		pass

	def _fl3_sampler(config):
		"""
		Plot Init for 3rd feature line.
		"""
		pass

	def screen_size_calc(final_row, final_col):
		"""
		Returns screen resolution based on number of plots.
		"""
		
		max_ = [1920, 1080]
		min_ = [320, 180]
		one_isto_two = [960, 540]
		
		if final_col >= 3:
			return max_
		if final_col == 1:
			return min_
		if final_col == 2:
			return one_isto_two


	def grid_allocator(halfcol=False):
		"""
		Allocates a row and column number based on the size of the plot (full-row plot or half-row plot)
		"""
		x = 0 # x only takes {0, 1}.
		y = 1 # y=0 is reserved for dashboard.
		
		while True:
			yield x, y
			
			if not last_half and (not half_row): 
				# last plot was a full plot and this plot is also a full plot
				x = 0 # Unchanged
				y = y + 1
				last_half = half_row


			if last_half and (not half_row):
				# last plot was a half plot and this one is a full plot
				x = 0
				y = y + 1
				last_half = half_row


			if last_half and half_row:
				# last was a half plot and this one is also a half plot
				y = y # Unchanged
				x = 1
				last_half = half_row

			if (not last_half) and half_row:
				#last was not half but this one is half
				y = y + 1
				x = 0
				last_half = half_row


	def update_dashboard(self, update_id=0, update_time=0, current_time = 0):
		"""
		Updates the plot dashboard with the passed values.
		"""
		new_dashboard_txt = f'Photon Statistics: Update → {update_id}    |    Update time → {update_time:.3f} s    |    Time Elapsed → {current_time :.1f} s'
		self.dashboard.setText(new_dashboard_txt, size='12pt')

	
	def add_canvas_plot(self, ref_name, title=None, half_plot=False, xRange=None, yRange=None, xLabel=None, yLabel=None, xLog=False, yLog=False):
		"""
		Adds a canvas to the canvas list with the set attributes.
		"""
		this_title = "Live Plot"
		this_x_label = "X axis →"
		this_x_units = ""
		this_y_label = "Y axis →"
		this_y_units = ""

		if title is not None:
			this_title = title

		if xLabel is not None:
			this_x_label = xLabel[0]
			this_x_units = xLabel[1]

		if yLabel is not None:
			this_y_label = yLabel[0]
			this_y_units = yLabel[1]

		
		if ref_name in canvaslist:
			raise Exception(f"PStatLiveGraph.add_canvas(`ref_name`) - `ref_name` {ref_name} is not unique.")

		row, col = self.grid_allocator(half_plot)
		
		new_canvas = window.addPlot(title=f"{this_title}", row=row, col=col, rowspan=(1*half_plot + 2*(not half_plot)))
		new_canvas.showGrid(x = True, y = True)

		new_curve.plot(pen='y', symbolBrush='c', symbolSize=4)
		new_curve.setLogMode(xLog, yLog)

		# Set Label and font size
		new_canvas.setLabel('left', this_y_label, this_y_units)
		new_canvas.setLabel('bottom', this_x_label, this_x_units)
		font = QtGui.QFont()
		font.setPixelSize(12)
		plot.getAxis("bottom").tickFont = font
		plot.getAxis("left").tickFont = font


		#Adjust range of logmode is set ("If you set the x range from 0 to 10000 in log mode, it effectively sets it to a range from 10**0 to 10**10000")
		if xLog and xRange is not None:
			xRange[0] = math.floor(math.log10(xRange[0]))
			xRange[1] = math.floor(math.log10(xRange[1]))
		if yLog and yRange is not None:
			yRange[0] = math.floor(math.log10(yRange[0]))
			yRange[1] = math.floor(math.log10(yRange[1]))


		if xRange is not None:
			xl, xh = xRange
			new_canvas.setRange(xRange=(xl, xh), yRange=None, padding=None, update=True, disableAutoRange=True)
		if yRange is not None:
			yl, yh = yRange
			new_canvas.setRange(yRange=(yl, yh), padding=None, update=True, disableAutoRange=True)

		self.curves[ref_name] = new_curve
		self.canvas[ref_name] = new_canvas

if __name__ == '__main__':
	lv = PStatLiveGraph