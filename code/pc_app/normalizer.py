#!/usr/bin/python3


import numpy as np
import multitau

class Normalizer:

	def __init__(self, lin_corrs, series_size, bin_ratio):
		"""
		Constructor that accepts multi-tau configuration parameters.
		"""

		self.mt_param = [lin_corrs, series_size, bin_ratio]
		self.points_scale_template = multitau.points_scale_template(*self.mt_param)
		self.bin_ratio_scale = multitau.bin_ratio_scale(*self.mt_param)
		self.bin_ratio_scale_sq = np.square(self.bin_ratio_scale)
		self.tau_values = multitau.x_tics(*self.mt_param)
		self.available_modes = ("no", "points", "mean")

	def set_mode(self, mode_str):
		"""
		Sets the normalization mode for the normalizer. Accepted modes -> {"no", "points", "mean"}
		"""
		mode = mode_str.lower()

		if mode == "no":
			self.norm_fn = self.no_norm
		elif mode == "points":
			self.norm_fn = self.points_norm
		elif mode == "mean":
			self.norm_fn = self.mean_norm
		else:
			raise Exception(f"Invalid normalization mode - {mode_str}!")

	def normalize(self, input_y, *args):
		"""
		Callable normalization function - single interface.
		"""
		return self.norm_fn(input_y, *args)
	
	# Pseudo Private Block ↓

	def no_norm(self, input_y):
		"""
		Returns a non-normalized sequence. This function does nothing except return its input.
		"""
		return input_y/self.bin_ratio_scale_sq

	def points_norm(self, input_y, points_no):
		"""
		Returns the input by dividing it with the number of points received by that particular channel.
		"""
		#np.seterr(divide='ignore', invalid='ignore')
		out = input_y
		if points_no != 0:
			norm = self.points_scale_template * points_no
			#norm = norm - self.tau_values
			out = out/norm
		
		return np.array(out)
		#print(points_no)

	def mean_norm(self, input_y, points_no, accumulate):
		"""
		Returns the mean normalized transformation of the input. Accepts the number of points received by each 
		channel and the mean monitor accumulate. Mean = accumulate / points_no.
		"""
		#print(f"p{points_no},  a{accumulate}")
		mean = accumulate / points_no
		print(mean)
		#scaled_mean = self.bin_ratio_scale * mean
		y = self.points_norm(input_y, points_no)
			
		if mean != 0:
			y = y / np.square(mean)
		
		return np.array(y)

	def __repr__(self):
		return f"<Normalizer {self.mt_param}>"