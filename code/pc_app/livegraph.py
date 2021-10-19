#!/usr/bin/python3

from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg
import math

def LiveGraph(xRange=None, yRange = None, **kwargs):	
	 
	# Reading of optional attributes
	this_title = "Live Plot"
	this_port = "Serial Device"
	
	this_x_label = "X axis →"
	this_x_units = ""

	this_y_label = "Y axis →"
	this_y_units = ""

	if 'title' in kwargs:
		this_title = kwargs['title']
	
	if 'port' in kwargs:
		this_port = kwargs['port']

	if 'x_label' in kwargs:
		this_x_label = kwargs['x_label']
	if 'x_units' in kwargs:
		this_x_units = kwargs['x_units']

	if 'y_label' in kwargs:
		this_y_label = kwargs['y_label']
	if 'y_units' in kwargs:
		this_y_units = kwargs['y_units']

	xlog = kwargs['config']["Log xAxis ACF"]

	app = QtGui.QApplication([])
	
	
	window = pg.GraphicsLayoutWidget(show=True, title= f"{this_title} - {this_port}")
	window.resize(1440,720)
	window.setWindowTitle(this_title)

	# icon
	icon = QtGui.QIcon("icon.png")
	window.setWindowIcon(icon)


	# Enable antialiasing for prettier plots
	pg.setConfigOptions(antialias=True, useOpenGL=True)
	
	#Add Updating title
	gl_title = pg.LabelItem(title="Photon Statistics: Update → • - Update time: • s", justify="centre")
	window.addItem(gl_title, col=0, colspan=2)
	window.nextRow()

	canvas = window.addPlot(title=f"{this_title}", row=1, col=0, rowspan=2)
	#window.setCentralWidget(canvas)

	curve = canvas.plot(pen='y', symbolBrush='c', symbolSize=4)
	curve.setLogMode(xlog, False)
	canvas.showGrid(x = xlog, y = True)
	canvas.setLabel('left', this_y_label, this_y_units)
	canvas.setLabel('bottom', this_x_label, this_x_units)

	#Adjust range of logmode is set ("If you set the x range from 0 to 10000 in log mode, it effectively sets it to a range from 10**0 to 10**10000")
	if xlog and xRange is not None:
		xRange = math.floor(math.log10(xRange))

	if xRange != None:
		canvas.setRange(xRange=(0, xRange), yRange=None, padding=None, update=True, disableAutoRange=True)
	if yRange != None:
		canvas.setRange(yRange=(0, yRange), padding=None, update=True, disableAutoRange=True)

	return {"app": app, "window": window, "canvas": canvas, "curve": curve, "gl_title" : gl_title}