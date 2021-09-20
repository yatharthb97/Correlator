from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg


def LiveGraph(Port, xRange):	
	app = QtGui.QApplication([])
	
	
	window = pg.GraphicsLayoutWidget(show=True, title= f"Auto-Correlation Plot - {Port}")
	window.resize(1440,720)
	window.setWindowTitle("Auto-Correlation Plot")


	# icon
	icon = QtGui.QIcon("skin.jpg")
	window.setWindowIcon(icon)


	# Enable antialiasing for prettier plots
	pg.setConfigOptions(antialias=True, useOpenGL=True)

	canvas = window.addPlot(title="Auto-Correlation Plot → •")
	window.setCentralWidget(canvas)

	curve = canvas.plot(pen='y', symbolBrush='c', symbolSize=4)
	curve.setLogMode(False, False)
	canvas.showGrid(x = True, y = True)
	canvas.setLabel('left', 'ACF', 'G(Tau)')
	canvas.setLabel('bottom', 'Lag', 'Tau')
	canvas.setRange(xRange=(0, xRange), yRange=None, padding=None, update=True, disableAutoRange=True)

	return app, canvas, curve