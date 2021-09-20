

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


def WelcomeText():
	print('''
		 	  /$$$$$$   /$$$$$$  /$$$$$$$$
		 	 /$$__  $$ /$$__  $$| $$_____/
		 	| $$  \\ $$| $$  \\__/| $$      
		 	| $$$$$$$$| $$      | $$$$$   
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
