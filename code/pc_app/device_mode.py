class StructParsar:
	
	def __init__(self, session_name, parent_dir, config, add_suffix=False):
		
		self.dstore = DataStore(self, parent_path=parent_dir, session_name=session_name, force=False, test_dirs=True)
		self.data = self.dstore.datasets
		
		self.root_dir = self.dstore.session_path #Will raise exception if path is not available
		self.name = session_name

		self.set_config(config)
		self.add_suffix = add_suffix

	
	def fl_structure(self):
		"""
		Layout of feature line structure.
		"""

		if self.config['Feature Line'] == "ACF":
			pass
		elif self.config['Feature Line'] == "Interarrival":
			pass
		elif self.config['Feature Line'] == "Sampler":
			pass
		else:
			raise Exception("Invalid Feature Line!")


	def get_struct_size():
		"""
		Calculates and returns the size of the receiving struct.
		"""
		if self.config['Feature Line'] == "ACF":
			total_struct_size = (	(self.config['Enable ACF'] * this_channel_size) + 
									self.config['Enable Count Rate'] +
									self.config['Enable Points Norm'] +
									self.config['Enable Sync Code'] +
									self.config['Enable Mean Norm'] + 
									self.config['Enable PC Histogram'] * self.config['PC Histogram Bins'] +
									((self.config['Enable ACF'] + 1) * self.config['Enable Performance Counters'])
								) * byte_size

		elif self.config['Feature Line'] == "Interarrival":
			total_struct_size = (self.config['Enable Sync Code'] + 1) * byte_size
		
		elif self.config['Feature Line'] == "Sampler":
			total_struct_size = 1 #TODO Fix



	def copy_config_to_session():
		"""
		Copies the current configuration file to the session directory.
		"""
		from shutil import copyfile
		copyfile(self.config['_this_config_file_path_'], self.root_dir)
		 


	def set_config(config):
		"""
		Creates a local store of configuration.
		Accepts both a dictionary or a config file path.
		"""
		if isadict(config):
			self.config = config
		elif isapath(config):
			with open(config) as file;
			json conf;
			conf.load(config)
			self.config = conf
		else:
			pass



	def generate_data_files():
		"""
		Generates the relavent data files for the feature line.
		"""
		print(f"• Creating files for feature line: {config['Feature Line']}")
		
		if config['Feature Line'] == "ACF":
			if config['Enable ACF']:
				create_file("ACF x Tau Data")
				create_file("ACF y Data")

			if config['Enable Count Rate']:
				self.create_file
			if config['Enable Points Norm']:
				self.create_file("Samples Processed")

			if config['Enable Mean Norm']:
				self.create_file("Mean Intensity")

			if config['Enable PC Histogram']:
				self.create_file("PC Histogram")

			if config['Enable Performance Counters']:
				self.create_file("Perf Counters")

		elif config['Feature Line'] == "Interarrival":
			pass
		elif config['Feature Line'] == "Sampler":
			pass
		else:
			raise Exception("Invalid Feature Line!")



	def generate_datasets():
		"""
		Populates the `data` dict with the required data sets → arrays, lists, or deques as required.
		"""
		
		def deque_template:
			"""
			Constructs a deque and populates it to zero to prevent arithmetic errors, etc.
			"""
			new  = deque(maxlen=self.def_history)
			new.append(0)
			return new

		
		self.data['time_s'] = deque_template()
		self.data['cntr'] = deque_template()

		if config['Feature Line'] == "ACF":
			
			self.mt_param = [self.config['MT Linear Correlators'], self.config['MT Series Size'], self.config['MT Bin Ratio']]

			if config['Enable ACF']:
				self.data['ACF_y'] = np.zeros(self.config['_channel_size_'])
				self.data['ACF_x'] = multitau.x_tics(*self.mt_param) 

			if config['Enable Count Rate']:
				self.data['CountRate_y'] =  deque_template()
			
			if config['Enable Points Norm']:
				self.data['LC0Points_y'] = deque_template()

			if config['Enable Mean Norm']:
				self.data["Mean_y"] = deque_template()

			if config['Enable PC Histogram']:
				self.data["Histogram_y"] = np.zeros(self.config['PC Histogram Bins']+1)

			if config['Enable Performance Counters']:
				self.data['Pf_Serial_y'] = deque_template()
				self.data['Pf_ACF_y'] = deque_template()

		elif config['Feature Line'] == "Interarrival":
			pass
		elif config['Feature Line'] == "Sampler":
			pass
		else:
			raise Exception("Invalid Feature Line!")


	def generate_struct_rep():
		header = ""
		text_line = ""
		footer = ""

		def addbox(h, t, f):
			header.append(h)
			text_line.append(t)
			footer.append(f)

		from utilities import boxed_txt

		if config['Feature Line'] == "ACF":
			if config['Enable Sync Check']:
				h, t, f = boxed_txt('SYNC CODE(4)')
				addbox(h, t, f)
			
			if config['Enable Count Rate(CR)']:
				h, t, f = boxed_txt('COUNT RATE(4)')
				addbox(h, t, f)

			if config['Enable Points Norm']:
				h, t, f = boxed_txt('NORM(4)')
				addbox(h, t, f)

			if config['EnableMeanNorm']:
				h, t, f = boxed_txt('Mean/Accumulate(4)')
				addbox(h, t, f)

			if config['EnableACFCalc']:
				s = config["_channel_size_"]*4
				h, t, f = boxed_txt(f'Auto Correlation Function({s})')
				addbox(h, t, f)

			if config['Enable PC Histogram']:
				s = (config["PC Histogram Bins"]+1)*4
				h, t, f = boxed_txt(f'PC Histogram({s})')
				addbox(h, t, f)

			if config['Enable Performance Counters']:
				s = 4
				h, t, f = boxed_txt(f'PerfC-Serial({s})')
				addbox(h, t, f)

			if config['Enable Performance Counters'] and config['EnableACFCalc']:
				s = 4
				h, t, f = boxed_txt(f'PerfC-ACF({s})')
				addbox(h, t, f)
		
		elif config['Feature Line'] == "Interarrival":
			pass
		
		elif config['Feature Line'] == "Sampler":
			pass
		
		else:
			raise Exception("Invalid Feature Line!")

		#Finally
		return header + '\n' + text_line + '\n' + footer

	
	# Somewhat Private Scope
	def file_name_generator(self, name):
		"""
		Generates the file paths for the data files.
		"""

		_name = name.lower()
		_name = name.replace(' ', '_')
		file_name =  f"{_name}.dat"
		
		suffix =  f"{self.name}_" * self.add_suffix
		
		file_name = (f"{suffix}{file_name}")
		file_name = os.path.join(self.root_dir, file_name),
		return file_name

	def create_file(self, name):
		"""
		Creates a file with a given name (name should be unique or it would override
		the past instance) and saves a reference to the `open_files` dictionary.
		"""

		file = open(self.file_name_generator(name), 'w')
		self.open_files.append(name : file)
		print(f"Saving {name} to: •••> {file_name} <•••")


	def generate_struct_parsar(self):
		
		
		
class DataStore:
    
    """
    Data Store is an object that manages creation of a session directory, 
    and maintains a dictionary of references of objects in its scope.
    """
    cls.class_lock = threading.Lock

    def __init__(self, parent_path=".", session_name=None, save_to_file=True, force=False, test_dirs=False):
        self.name_ = session_name
        if session_name = None:
            self.name_ = str(time.time_ns())

            path = self.DirCreationPolicy(parent_path, name_, force, test_dirs)
            if path == None:
                raise Exception(f"Data Store could not find a directory for: {parent_path} → {name_}.")
            
            os.mkdir(path, mode = 0o777)
            self.session_path = path

            self.datasets = {} # Dictionary of Data objects


        @classmethod
        def DirCreationPolicy(cls, parent_path, name_, force=False, test_dirs=False):
            """
            Negotiates a directory path for the data store.
            """

            #Acquire global lock
            cls.class_lock.acquire()
            
            path = os.path.join(parent_path, name_)
            path_available = os.path.isdir(path):

            if path_available:
                return path
            else:
                if test_dirs == True :
                    if name_.startswith("test"):
                           from shutil import rmtree
                           shutil.rmtree(path)
                           return path
                
                elif force == True:
                    MAXTRY = 100
                    trycnt = 0
                    while trycnt <= MAXTRY:
                       try_path = os.path.join(parent_path, f"{name_}_{trycnt}")
                       trycnt = trycnt + 1
                        if not os.path.isdir(try_path):
                           return try_path
                
                # Finally
                cls.class_lock.release()
                
                return None

        def __repr__():
            return f"<DataStore obj: {self.session_path}>"