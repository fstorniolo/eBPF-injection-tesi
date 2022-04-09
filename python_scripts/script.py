import matplotlib
import os, os.path
import fnmatch
import numpy as np
import matplotlib.pyplot as plt

setup_values = []
downtime_values = []
totaltime_values = []
transferredram_values = []

def take_value(string):
	for str in string.split():
		if str.isdigit():
			return str

def create_scatterplot():



def create_barplots():
	global setup_values
	global totaltime_values
	global downtime_values
	global transferredram_values

	setup_mean = np.average(setup_values)
	downtime_mean = np.average(downtime_values)
	totaltime_mean = np.average(totaltime_values)
	transferredram_mean = np.average(transferredram_values)

	# set width of bar
	barWidth = 0.25
	fig = plt.subplots(figsize =(12, 8))

	values_optimized = [setup_mean, downtime_mean]
	values_nonoptimized = [500, 500]

	br1 = np.arange(len(values_optimized))
	br2 = [x + barWidth for x in br1]

	# Make the plot
	plt.bar(br1, values_optimized, color ='blue', width = barWidth,
	        edgecolor ='grey', label ='my values')
	plt.bar(br2, values_nonoptimized, color ='orange', width = barWidth,
	        edgecolor ='grey', label ='old values')

	 
	# Adding Xticks
	plt.xlabel('', fontweight ='bold', fontsize = 15)
	plt.ylabel('Time ms', fontweight ='bold', fontsize = 15)
	plt.xticks([r + barWidth/2 for r in range(len(values_optimized))], ['setup', 'downtime'])

	plt.legend()
	plt.show()

	#// print(values)
	#fig = plt.figure(figsize = (10, 5))
	 
	# creating the bar plot
	#plt.bar(2, values, color ='maroon',width = 0.4)
	 
	#plt.xlabel("Courses offered")
	#plt.ylabel("No. of students enrolled")
	#plt.title("Students enrolled in different courses")
	#plt.show()

def main():
	global setup_values
	global totaltime_values
	global downtime_values
	global transferredram_values

	filenames = []
	for f in os.listdir("../results"):
		if fnmatch.fnmatch(f, '*.txt'):
			filenames.append(f)

	#filenames.remove("")
	print(filenames)

	setup_values = np.zeros(len(filenames))
	downtime_values = np.zeros(len(filenames))
	totaltime_values = np.zeros(len(filenames))
	transferredram_values = np.zeros(len(filenames))	

	setup = "setup"
	downtime = "downtime"
	totaltime = "total time"
	transferredram = "transferred ram"

	pos = 0


	for file in filenames:
		f = open("../results/" + file)
		file_lines = f.readlines()
		#print("num of lines in file", len(file_lines))
		for line in file_lines:
			if setup in line:
				#print("setup")
				val = take_value(line)
				#print(val)
				setup_values[pos] = val 

			elif downtime in line:
				#print("downtime")
				val = take_value(line)
				#print(val)
				downtime_values[pos] = val	

			elif totaltime in line:
				#print("total time")
				val = take_value(line)
				#print(val)
				totaltime_values[pos] = val

			elif transferredram in line:
				#print("transferredram")
				val = take_value(line)
				#print(val)
				transferredram_values[pos] = val
		pos+=1
		f.close()

	create_barplots()
	#print("setup", setup_values)
	#print("downtime", downtime_values)
	#print("total time", totaltime_values)
	#print("transferred ram", transferredram_values)

if __name__ == "__main__":
    main()