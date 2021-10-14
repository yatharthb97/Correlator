

class StatMethods:

	@classmethod
	def var_estimator(first_moment, second_moment, size):
		
		mean = float(first_moment) / float(size)
		mean2 = mean * mean
		variance = float(second_moment)/ float(size) - mean2

	#StatMethods.var_estimator(accumulate, acf_data[0])


	def norm_lag0_estimator(mean, variance):
		return 1 + float(variance)/float(mean*mean)

	def acf_amplitude(norm_acf):
		beta = norm_acf[len(norm_acf)] - norm_acf[0]
		return beta
	