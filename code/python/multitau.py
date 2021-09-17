import numpy as np


def channel_size(lin_corrs, series_size, bin_ratio):
  '''
  Returns the expected channel size based on the multi-tau parameters.
  '''
  return  int(series_size + ((series_size - (series_size/bin_ratio)) * (lin_corrs-1)))


def x_tics(lin_corrs, series_size, bin_ratio):
  '''
  Generates and returns a list of x-ticks that can be used for plotting, using the 
  multi-tau parameters.
  '''
  
  def tau_scaling(s, val):
    return (bin_ratio**s)*val

  xtics = []

  for s in range(0, lin_corrs):
    start = 1
    lin_tick_group = [*range(start, start + series_size)]
    lin_tick_group = [tau_scaling(s, val) for val in lin_tick_group]
    if s != 0:
      xtics.extend(lin_tick_group[int(series_size/bin_ratio):]) #Discard initial values
    else:
      xtics.extend(lin_tick_group) #Append full list
  return np.array(xtics)


def points_norm(lin_corrs, series_size, bin_ratio, LC0_points):
	'''
	Generates and returns a list of normalisation values based on the data points fed to 
	the 0th Linear Correlator.
	'''

	def points_scale(s): 
		''' Points handled by the sth linear correlator.'''
		return (bin_ratio**(-s)) * LC0_points

	norm = []

	for s in range(0, Lin_corrs):
		lin_norm_group = [1] * series_size
		lin_norm_group = [int(points_scale(s) * val) for val in lin_norm_group]
		if s != 0:
		  norm.extend(lin_norm_group[int(series_size/bin_ratio):]) #Discard initial values
		else:
		  norm.extend(lin_norm_group) #Append full list
	return np.array(norm)


def points_norm_template(lin_corrs, series_size, bin_ratio):
	return np.array([1] * channel_size(lin_corrs, series_size, bin_ratio))



# def points_norm_template(lin_corrs, series_size, bin_ratio):
# 	'''
# 	Generates and returns a list of normalisation values, which whn multiplied by the points
# 	fed to the 0th linear Correlator - gives a list of points normalization values for all
# 	the tau values.
# 	'''

# 	def points_scale(s): 
# 		''' Points handled by the sth linear correlator.'''
# 		return (bin_ratio**(-s))

# 	norm = []

# 	for s in range(0, Lin_corrs):
# 		lin_norm_group = [1.0] * series_size
# 		lin_norm_group = [(points_scale(s) * val) for val in lin_norm_group]
# 		if s != 0:
# 		  norm.extend(lin_norm_group[int(series_size/bin_ratio):]) #Discard initial values
# 		else:
# 		  norm.extend(lin_norm_group) #Append full list
# 	return np.array(norm)