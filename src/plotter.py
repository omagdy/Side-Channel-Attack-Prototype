import csv
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

import csv
import matplotlib.pyplot as plt

x=[]
y1=[]
y2=[]

x_axis_width = 10000
y_axis_height = 3000

red_patch = mpatches.Patch(color='red', label='Probe numbers[5]')
blue_patch = mpatches.Patch(color='blue', label='Probe numbers[7]')

with open('data.csv', 'r') as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
        x.append(int(row[0]))
        y1.append(int(row[1]))
        y2.append(int(row[2]))


fig, ax1 = plt.subplots()

color = 'tab:blue'
ax1.set_xlabel('Time (microseconds)')
ax1.set_ylabel('numbers[5] Access cycles (rstc or rstcp)', color=color)
ax1.scatter(x, y1, color=color)
ax1.tick_params(axis='y')
ax1.axis([0, x_axis_width, 0, y_axis_height])

ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis

color = 'tab:red'
ax2.set_ylabel('numbers[7] Access cycles (rstc or rstcp)', color=color)
ax2.scatter(x, y2, color=color)
ax2.axis([0, x_axis_width, 0, y_axis_height])

fig.tight_layout()  # otherwise the right y-label is slightly clipped

plt.legend(handles=[red_patch, blue_patch])
plt.show()