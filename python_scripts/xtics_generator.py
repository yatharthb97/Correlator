#bin/usr/env/ python3
#bin/usr/env/ python
# -*- coding: utf-8 -*-


def generate_x_tics(Lin_channels, Series_size, Bin_ratio):
	''' Generates and returns a list of x-ticks that is used for plotting. '''
	
	def tau_scaling(s, val):
		return (Bin_ratio**s)*val

	xtics = [] #Origin
	#xtics.extend([*range(1, Series_size)]) #s = 0

	for s in range(0, Lin_channels):
		start = s * Series_size
		lin_tick_group = [*range( start, start + Series_size)]
		lin_tick_group = [tau_scaling(s, val) for val in lin_tick_group]
		xtics.extend(lin_tick_group)
	return xtics
