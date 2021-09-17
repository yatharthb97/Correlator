

def PseudoConsole():
	while True:
		user_input = input(f"==> Enter `stop` to stop the reading process →  ")
		
		if user_input == 'stop':
			return

		# Break Primary Node
		(firstword, rest) = user_input.split(maxsplit=1)

		if firstword == "echo":
			print(f">> {rest}")