#!/usr/bin/python3
import sys
import numpy as np
import pandas as pd
import subprocess
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os

fname = sys.argv[1]
max_iter = sys.argv[2]
num_trials = int(sys.argv[3])

gname = os.path.basename(fname).split(".")[0] 
print("Running for graphs: " + gname)
# command
algo_types = ['FI', 'SS', 'SSH', 'TM']  
fault_rates = range(9, 16)

npdata = np.zeros((len(fault_rates), len(algo_types) + 1))

i = 0
for fault_rate in fault_rates:
    cmd = "PRINT_GRAPH=0 MAX_ITER=%s NUM_TRIAL=%d NORM_PROB=%s ./failureTestSync %s"%(max_iter, num_trials, fault_rate, fname)

    p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    op,err = p.communicate()

    op = op.decode("utf-8")
    op = op.split("\n")
    op = [o.strip() for o in op]
    
    op2 = op[1].split()

    npdata[i,0] = fault_rate
    npdata[i,-1] = int(op2[-1])
    npdata[i,-2] = int(op2[-2])
    npdata[i,-3] = int(op2[-3])
    npdata[i,-4] = int(op2[-4])

    i+= 1

output = pd.DataFrame(npdata)

output.columns = ['fault_rate'] + algo_types
output.to_csv(gname+'_fault_rate.csv')


fig, ax = plt.subplots()
ind = np.arange(len(fault_rates))
width = 0.2

rects1 = ax.bar(ind, 100*output['FI']/num_trials, width, color='r')
rects2 = ax.bar([i + width for i in ind], 100*output['SS']/num_trials, width, color='g')
rects3 = ax.bar([i + 2*width for i in ind], 100*output['SSH']/num_trials, width, color='b')
rects4 = ax.bar([i + 3*width for i in ind], 100*output['TM']/num_trials, width, color='purple')

ax.set_ylabel('Failures %')
ax.set_ylim(0, 110)
ax.set_title(gname + 'Failure Rate')
ax.set_xticks(ind + 2*width)
ax.set_xticklabels(['2^-'+str(fr) for fr in fault_rates])

ax.legend((rects1[0], rects2[0], rects3[0], rects4[0]), ('FI', 'SS', 'SSH', 'TM'))

plt.savefig(gname + '_failure_rate.png')

