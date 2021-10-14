

def PseudoConsole():
	while True:
		user_input = input(f"==> Enter `stop` to stop the reading process →  ")
		
		if user_input == 'stop':
			return

		# Break Primary Node
		try:
			(firstword, rest) = user_input.split(maxsplit=1)

			if firstword == "echo":
				print(f">> {rest}")
		except ValueError:
			pass


def WelcomeText(name):
	print(f'''
		 	  /$$$$$$   /$$$$$$  /$$$$$$$$
		 	 /$$__  $$ /$$__  $$| $$_____/
		 	| $$  \\$$| $$  \\__/| $$         ••→ Session Name •••
		 	| $$$$$$$$| $$      | $$$$$        → {name               }
		 	| $$__  $$| $$      | $$__/   
		 	| $$  | $$| $$    $$| $$      
		 	| $$  | $$|  $$$$$$/| $$      
		 	|__/  |__/ \\______/ |__/  
		''')


# This implementation avoids drifts in timing values.
#Source: https://stackoverflow.com/a/28034554/11733629
def periodic_callbacks(period_ms, fn, *args):
    period = period_ms * 0.001 #Convert to seconds
    def g_tick():
        t = time.time()
        while True:
            t += period
            yield max(t - time.time(), 0) #Avoid negative values
    sleep_times = g_tick()
    while True:
        time.sleep(next(sleep_times))
        fn(*args)


# sync_ =(f"""
# ░▒▓▛▀▀▀▀▀▀▀▀▀▜▓▒░
# ░▒▓▌SYNC CODE▐▓▒░
# ░▒▓▙▄▄▄▄▄▄▄▄▄▟▓▒░
# """)

def StructRepresentation(config):

	def box_generator(text):
		t_l = '┏'
		t_r = '┓'
		b_l = '┗'
		b_r = '┛'
		hbounds = '━'
		vbounds = '┃'

		text_line = vbounds + ' ' + text + ' ' + vbounds
		header = t_l + hbounds * (len(text_line)-2) + t_r
		footer = b_l + hbounds * (len(text_line)-2) + b_r
		return header, text_line, footer


	header = ""
	text_line = ""
	footer = ""
	if config['EnableSyncCode']:
		h, t, f = box_generator('SYNC CODE(4)')
		header = header + h
		text_line = text_line + t
		footer = footer + f

	if config['EnableCountRate']:
		h, t, f = box_generator('COUNT RATE(4)')
		header = header + h
		text_line = text_line + t
		footer = footer + f

	if config['EnablePointsNorm']:
		h, t, f = box_generator('NORM(4)')
		header = header + h
		text_line = text_line + t
		footer = footer + f

	if config['EnableMeanNorm']:
		h, t, f = box_generator('Mean/Accumulate(4)')
		header = header + h
		text_line = text_line + t
		footer = footer + f

	if config['EnableACFCalc']:
		s = config["channel_size"]*4
		h, t, f = box_generator(f'Auto Correlation Function({s})')
		header = header + h
		text_line = text_line + t
		footer = footer + f

	if config['Enable PC Histogram']:
		s = (config["PC Histogram Bins"]+1)*4
		h, t, f = box_generator(f'PC Histogram({s})')
		header = header + h
		text_line = text_line + t
		footer = footer + f

	if config['Enable Performance Counters']:
		s = 4
		h, t, f = box_generator(f'PerfC-Serial({s})')
		header = header + h
		text_line = text_line + t
		footer = footer + f

	if config['Enable Performance Counters'] and config['EnableACFCalc']:
		s = 4
		h, t, f = box_generator(f'PerfC-ACF({s})')
		header = header + h
		text_line = text_line + t
		footer = footer + f

	return header + '\n' + text_line + '\n' + footer