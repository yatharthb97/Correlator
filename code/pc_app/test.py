from livegraph import LiveGraph
import numpy as np

x = np.load('acf_x.npy')
y = np.load('acf_y.npy')

lg = LiveGraph(yRange=0.04)
lg['curve'].setData(x, y)
lg['app'].exec_()