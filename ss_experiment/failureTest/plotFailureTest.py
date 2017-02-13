#!/usr/bin/python3
import sys
import numpy as np
import pandas as pd
import subprocess
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
plt.style.use('ggplot')
import os

fmarkers=Line2D.filled_markers

fname = sys.argv[1]
output = pd.DataFrame.from_csv(fname)

gname = os.path.basename(fname).split(".")[0]
gname = gname.split("_")[0]
num_trials=100

fig, ax = plt.subplots()
fault_rates = output['fault_rate']
ind = 2**(-1*fault_rates)
width = 0.2

s1, = ax.plot(ind, 100*output['FI']/num_trials, linewidth=5.0, marker=fmarkers[0], markersize=15, label="Fault Injected SV")
s2, = ax.plot(ind, 100*output['SS']/num_trials, linewidth=5.0, marker=fmarkers[1], markersize=15, label="SS")
s3, = ax.plot(ind, 100*output['SSH']/num_trials, linewidth=5.0, marker=fmarkers[2], markersize=15, label="SS with H")
s4, = ax.plot(ind, 100*output['TM']/num_trials, linewidth=5.0, marker=fmarkers[3], markersize=15, label="Triple Modular")

ax.set_ylabel('Failures %')
ax.set_ylim(-10, 110)
ax.set_title(gname + 'Failure Rate')
ax.set_xscale("log",basex=2);
ax.set_xlabel('Fault Rate')

#Shrink current axis by 20%
box = ax.get_position()
ax.set_position([box.x0, box.y0, box.width * 0.8, box.height])

# Put a legend to the right of the current axis
ax.legend(handles=[s1, s2, s3, s4], loc='center left', bbox_to_anchor=(1, 0.5))

plt.savefig(fname + '_failure_rate.png')