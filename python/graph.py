#from numpy import *
import numpy as np
import math
import matplotlib.pyplot as plt

t = np.linspace(30, 70, 9)
t = np.append(t,80)
fpm = []
fpt = []

f = open("avgFPT.txt", "r")
for val in f:
    fpt.append(float(val))
f.close()

f = open("avgFPM.txt", "r")
for val in f:
    fpm.append(float(val))
f.close()

fig, ax1 = plt.subplots(figsize=(20, 10))
fig.suptitle('20 tasks', fontsize=27, fontweight='bold')
fig.canvas.set_window_title('WCRT for 20 tasks')

ax1.yaxis.grid(True, linestyle='-', which='major', color='lightgrey',
               alpha=0.5)

ax1.set_axisbelow(True)
ax1.set_ylabel('WCRT in ns', fontsize=27)
ax1.set_xlabel('Utilization %', fontsize=27)
ax1.tick_params(axis='both', which='major', labelsize=27)

a, = plt.plot(t, fpt, 'r') 
b, = plt.plot(t, fpm, 'b') 
params = {'legend.fontsize': 27,
          'legend.handlelength': 2}
plt.rcParams.update(params)

plt.legend([a, b], ['FPT', 'FPM'])
plt.show()

fig.savefig('graph20.png', bbox_inches='tight')