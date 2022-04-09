import matplotlib
import os, os.path
import fnmatch
import numpy as np
import matplotlib.pyplot as plt

setup_values_opt = []
downtime_values_opt = []
totaltime_values_opt = []
transferredram_values_opt = []

setup_values_non_opt = []
downtime_values_non_opt = []
totaltime_values_non_opt = []
transferredram_values_non_opt = []

def take_value(string):
	for str in string.split():
		if str.isdigit():
			return str

def create_histograms():
	global setup_values_opt
	global totaltime_values_opt
	global downtime_values_opt
	global transferredram_values_opt

	global setup_values_non_opt
	global totaltime_values_non_opt
	global downtime_values_non_opt
	global transferredram_values_non_opt

	x = list(range(0, 202))

	fig = plt.figure()
	ax1 = fig.add_subplot(121)
	ax2 = fig.add_subplot(122)

	ax1.hist(setup_values_opt, edgecolor='black', bins=10)
	ax2.hist(setup_values_non_opt, edgecolor='black', bins=10)

	plt.show()

	fig = plt.figure()
	ax1 = fig.add_subplot(121)
	ax2 = fig.add_subplot(122)

	ax1.hist(downtime_values_opt, edgecolor='black', bins=10)
	ax2.hist(downtime_values_non_opt, edgecolor='black', bins=10)

	plt.show()

	fig = plt.figure()
	ax1 = fig.add_subplot(121)
	ax2 = fig.add_subplot(122)

	ax1.hist(totaltime_values_opt, edgecolor='black', bins=10)
	ax2.hist(totaltime_values_non_opt, edgecolor='black', bins=10)

	plt.show()

	fig = plt.figure()
	ax1 = fig.add_subplot(121)
	ax2 = fig.add_subplot(122)

	ax1.hist(transferredram_values_opt, edgecolor='black', bins=10)
	ax2.hist(transferredram_values_non_opt, edgecolor='black', bins=10)

	plt.show()


def create_scatterplot():
	global setup_values_opt
	global totaltime_values_opt
	global downtime_values_opt
	global transferredram_values_opt

	global setup_values_non_opt
	global totaltime_values_non_opt
	global downtime_values_non_opt
	global transferredram_values_non_opt
	#fig, (ax1, ax2) = plt.subplots(1, 2)
	x = list(range(0, 202))

	fig = plt.figure()
	ax1 = fig.add_subplot(121)
	ax2 = fig.add_subplot(122)

	ax1.scatter(x, setup_values_opt)
	ax2.scatter(x, setup_values_non_opt)
	ax1.set_yticks(np.arange(0, 60, 5))
	ax2.set_yticks(np.arange(0, 60, 5))

	plt.show()

	fig = plt.figure()
	ax1 = fig.add_subplot(121)
	ax2 = fig.add_subplot(122)


	ax1.scatter(x, downtime_values_opt)
	ax2.scatter(x, downtime_values_non_opt)
	ax1.set_yticks(np.arange(0, 350, 10))
	ax2.set_yticks(np.arange(0, 350, 10))

	plt.show()

	fig = plt.figure()
	ax1 = fig.add_subplot(121)
	ax2 = fig.add_subplot(122)

	ax1.scatter(x, totaltime_values_opt)
	ax2.scatter(x, totaltime_values_non_opt)
	plt.show()

	fig = plt.figure()
	ax1 = fig.add_subplot(121)
	ax2 = fig.add_subplot(122)

	ax1.scatter(x, transferredram_values_opt)
	ax2.scatter(x, transferredram_values_non_opt)
	plt.show()

def create_barplots():
	global setup_values_opt
	global totaltime_values_opt
	global downtime_values_opt
	global transferredram_values_opt

	global setup_values_non_opt
	global totaltime_values_non_opt
	global downtime_values_non_opt
	global transferredram_values_non_opt

	setup_mean_opt = np.average(setup_values_opt)
	downtime_mean_opt = np.average(downtime_values_opt)
	totaltime_mean_opt = np.average(totaltime_values_opt)
	transferredram_mean_opt = np.average(transferredram_values_opt)

	setup_mean_non_opt = np.average(setup_values_non_opt)
	downtime_mean_non_opt = np.average(downtime_values_non_opt)
	totaltime_mean_non_opt = np.average(totaltime_values_non_opt)
	transferredram_mean_non_opt = np.average(transferredram_values_non_opt)


	# set width of bar
	barWidth = 0.25
	fig = plt.subplots(figsize =(12, 8))

	values_optimized = [setup_mean_opt, downtime_mean_opt, totaltime_mean_opt]
	values_nonoptimized = [setup_mean_non_opt, downtime_mean_non_opt, totaltime_mean_non_opt]

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
	global setup_values_opt
	global totaltime_values_opt
	global downtime_values_opt
	global transferredram_values_opt

	global setup_values_non_opt
	global totaltime_values_non_opt
	global downtime_values_non_opt
	global transferredram_values_non_opt

	filenames = []
	for f in os.listdir("../results/result_non_opt_noload"):
		if fnmatch.fnmatch(f, '*.txt'):
			filenames.append(f)

	#filenames.remove("")
	print(filenames)

	setup_values_opt = np.zeros(len(filenames))
	downtime_values_opt = np.zeros(len(filenames))
	totaltime_values_opt = np.zeros(len(filenames))
	transferredram_values_opt = np.zeros(len(filenames))

	setup_values_non_opt = np.zeros(len(filenames))
	downtime_values_non_opt = np.zeros(len(filenames))
	totaltime_values_non_opt = np.zeros(len(filenames))
	transferredram_values_non_opt = np.zeros(len(filenames))

	setup = "setup"
	downtime = "downtime"
	totaltime = "total time"
	transferredram = "transferred ram"

	pos = 0


	for file in filenames:
		f = open("../results/result_opt_noload/" + file)
		file_lines = f.readlines()
		#print("num of lines in file", len(file_lines))
		for line in file_lines:
			if setup in line:
				#print("setup")
				val = take_value(line)
				#print(val)
				setup_values_opt[pos] = val

			elif downtime in line:
				#print("downtime")
				val = take_value(line)
				#print(val)
				downtime_values_opt[pos] = val

			elif totaltime in line:
				#print("total time")
				val = take_value(line)
				#print(val)
				totaltime_values_opt[pos] = val

			elif transferredram in line:
				#print("transferredram")
				val = take_value(line)
				#print(val)
				transferredram_values_opt[pos] = val
		pos+=1
		f.close()

	pos = 0
	for file in filenames:
		f = open("../results/result_non_opt_noload/" + file)
		file_lines = f.readlines()
		#print("num of lines in file", len(file_lines))
		for line in file_lines:
			if setup in line:
				#print("setup")
				val = take_value(line)
				#print(val)
				setup_values_non_opt[pos] = val

			elif downtime in line:
				#print("downtime")
				val = take_value(line)
				#print(val)
				downtime_values_non_opt[pos] = val

			elif totaltime in line:
				#print("total time")
				val = take_value(line)
				#print(val)
				totaltime_values_non_opt[pos] = val

			elif transferredram in line:
				#print("transferredram")
				val = take_value(line)
				#print(val)
				transferredram_values_non_opt[pos] = val
		pos+=1
		f.close()

	create_barplots()
	create_scatterplot()
	create_histograms()
	#print("setup", setup_values)
	#print("downtime", downtime_values)
	#print("total time", totaltime_values)
	#print("transferred ram", transferredram_values)

if __name__ == "__main__":
    main()