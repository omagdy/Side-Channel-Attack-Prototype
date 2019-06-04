import csv
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches


files = ['Probe-numbers[5].csv', 'Probe-numbers[7].csv']

color = ['r', 'b']

red_patch = mpatches.Patch(color='red', label='Probe numbers[5]')
blue_patch = mpatches.Patch(color='blue', label='Probe numbers[7]')

i=0
for f in files:
	with open(f, 'r') as csvfile:
		reader = csv.reader(csvfile, delimiter=' ', quotechar='|')
		x=[float(v) for v in next(reader)[:-1]]
		y=[float(v) for v in next(reader)[:-1]]
		plt.scatter(y,x,c=color[i])
		i+=1

plt.ylim(0,1000)
plt.ylabel('TSC Cycles')
plt.xlabel('Iteration')
plt.legend(handles=[red_patch, blue_patch])
plt.show()
