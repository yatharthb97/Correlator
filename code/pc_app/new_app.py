#!/usr/bin/python3


#8. Defines update function - Coroutine
def update_fn():
	if port.in_waiting >= total_struct_size:
		#print(f"Inwaiting: {port.in_waiting}")
		
		#Begin new read
		norm_args.clear()

		try:
			raw_data = port.read(total_struct_size)
			#print(raw_data)
		except SerialException:
			if config['LiveGraph']:
				print(f"[ERROR] Serial Device has been disconnected. Please close the LiveGraph to terminate the program.")
				time.sleep(5) #Thread sleeps for 5 seconds, then prints this error again.
			else:
				print(f"[ERROR] Serial Device has been disconnected. The program will terminate.")
				stop_code_asserted = True
		

		# Calc update_time and update_id
		global update_id, measure_clock_start
		
		measure_clock_now = time.perf_counter()
		measurement_time = measure_clock_now - measure_clock_start
		measurement_time = max(0, measurement_time)
		machine.data['time'].append(measurement_time)
		
		update_id = update_id + 1
		machine.data['cntr'].append(update_id)
		

		offsetter = OffsetTracker() # New Offset Tracker Object

		# SYNCHRONIZATION CODE
		if config['EnableSyncCode']:
			sync_code = np.frombuffer(raw_data, dtype = np.int32, count = 1, offset=offsetter.advance(byte_size))

			if sync_code[0] != np.int32(config['Sync Code']):
				print(f"[SYNC ERROR] Sync Code Mismatch! - Update ID: {update_id} - rec:{(sync_code[0])} vs set:{np.int32(config['SyncCode'])}")

		elif config['Feature Line'] == 'ACF':
			# COUNT RATE
			if config['EnableCountRate']:
				count_rate = np.frombuffer(raw_data, dtype = 'f4', count = 1, offset=offsetter.advance(byte_size))
				count_rate = count_rate[0] * 1.0 / config['CRCoarseGrainingInterval_s'] # Adjust to 1s
				
				countratefile.write(f"{measurement_time: .5f}{DATASEP}{count_rate :.3f}\n")

				#Push Count Rate to live graph
				if config['LiveGraph']:
					count_rate_y.append(count_rate)
					live_graph['canvas2'].setTitle(title=f"Count Rate → {count_rate/1000: .3f} kHz")
					live_graph['curve_cnt_rate'].setData(time_x, count_rate_y)


			# POINTS NORM
			if config['EnablePointsNorm']:
				points_norm = np.frombuffer(raw_data, dtype = np.uint32, count = 1, offset=offsetter.advance(byte_size))
				points_norm = points_norm[0]
				norm_args.append(points_norm)
				print(f"{update_id} - {points_norm}")


			# MEAN NORMALIZATION - ACCUMULATE
			if config['EnableMeanNorm']:
				accumulate = np.frombuffer(raw_data, dtype = np.uint32, count = 1, offset=offsetter.advance(byte_size))
				accumulate = accumulate[0]
				norm_args.append(accumulate)
				if points_norm != 0:
					mean = accumulate / points_norm
					meanfile.write(f"{measurement_time}{DATASEP}{mean}\n")
			
			# ACF CHANNEL
			if config['EnableACFCalc']:
				data = np.frombuffer(raw_data, dtype = 'f4', count = this_channel_size, offset=offsetter.advance(this_channel_size*byte_size))
				normed_y = data
				normed_y = normalizer.normalize(data, *norm_args)
				np.savetxt(acffile, normed_y.flatten(), fmt='%f', newline = ", ") #delimiter=',', newline='\n'
				acffile.write('\n') #TODO - Fix this ugly shit

				#Push ACF to LiveGraph
				if config["LiveGraph"] == True:
					live_graph['curve'].setData(x_tau_values, normed_y)

			# Histogram
			# if config['Enable PC Histogram']:
			# 
			# 	count = config['PC Histogram Bins'] + 1
			# 	hist = np.frombuffer(raw_data, dtype = np.uint32, count = count, offset=offsetter.advance(count*byte_size))
			# 	#Does not save to file TODO
			# 	if config["LiveGraph"]:
			# 		live_graph.curve["PCHistogram"].setData(live_graph.hist_x, hist)

			# Performance Counters
			# if config['Enable Performance Counters']:
			# 	count = (config['EnableACFCalc'] + 1)
			# 	pf = np.frombuffer(raw_data, dtype = np.uint32, count = count, offset=offsetter.advance(count*byte_size))
			# 	pf_serial_y.append(pf[0])

			# 	if config['EnableACFCalc']:
			# 		pf_acf_y.append(pf[1])

			# 	if ['LiveGraph']:
			# 		live_graph.curve['PerfCounter Serial'].setData(time_x, pf_serial_y)
			# 		if ['EnableACFCalc']:
			# 			live_graph.curve['PerfCounter ACF'].setData(time_x, pf_acf_y)

		# Update GUI Header
		if config['Live Graph']:
			global update_time_start, update_time_stop
			update_time_stop = time.perf_counter()
			live_graph['gl_title'].setText(f'Photon Statistics: Update → {update_id}    |    Update time → {(update_time_stop - update_time_start):.3f} s    |    Time Elapsed → {time_x[-1] :.1f} s', size='12pt')
			update_time_start = time.perf_counter()





	def get_update_fn(port, parsar_fn):
		
		def built_update_fn():
			if port.in_waiting >= self.total_struct_size:
				#print(f"Inwaiting: {port.in_waiting}")
				
				#Begin new read
				norm_args.clear()

				try:
					raw_data = port.read(self.total_struct_size)
					#print(raw_data)
				except SerialException:
					if config['LiveGraph']:
						print(f"[ERROR] Serial Device has been disconnected. Please close the LiveGraph to terminate the program.")
						time.sleep(5) #Thread sleeps for 5 seconds, then prints this error again.
					else:
						print(f"[ERROR] Serial Device has been disconnected. The program will terminate.")
						self.stop_code_asserted = True
				

				# Calc update_time and update_id
				global update_id, measure_clock_start
				
				measure_clock_now = time.perf_counter()
				measurement_time = measure_clock_now - self.param.measure_clock_start
				measurement_time = max(0, measurement_time)
				self.data['time'].append(measurement_time)
				
				self.param.update_id = self.param.update_id + 1
				self.data['cntr'].append(self.param.update_id)
				

				## custom Parsar Executed Here
				parser_fn()
				## ---------------------------


				# Update GUI Header
				if config['Live Graph']:
					self.param.update_time_stop = time.perf_counter()
					live_graph['gl_title'].setText(f'Photon Statistics: Update → {self.param.update_id}    |    Update time → {(self.param.update_time_stop - self.param.update_time_start):.3f} s    |    Time Elapsed → {self.data['time'][-1] :.1f} s', size='12pt')
					self.param.update_time_start = time.perf_counter()


class ParamStruct:
	def __init__():
		update_time_start = 0
		update_time_stop = 0


## Template for a parser function: def parser(data_handle, graph_handle, config)

def parsar