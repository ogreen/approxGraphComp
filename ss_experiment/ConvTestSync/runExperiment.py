#!/usr/bin/python3
import sys
import numpy as np
import subprocess
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os


BIN ="../../convTestSync"

fname = sys.argv[1]
max_iter = sys.argv[2]
num_trials = int(sys.argv[3])

graph = os.path.basename(fname).split(".")[0] 
print("Running for graphs: " + graph)
# command
algo_types = ['FI', 'SS', 'SSH', 'TM']  
fault_rates = np.linspace(9,15,4).tolist()

npdata = np.zeros((len(fault_rates), len(algo_types) + 1))

i = 0
for fault_rate in fault_rates:
    outfile = "convTestSync_%s_%s.csv"%(graph, fault_rate)
    ofstream= open(outfile,'wb')
    cmd = "PRINT_GRAPH=0 MAX_ITER=%s NUM_TRIAL=%d NORM_PROB=%s %s %s "%(max_iter, num_trials, fault_rate, BIN, fname)

    print ("Running....."+cmd)
    p = subprocess.Popen(cmd,shell=True,stdout=ofstream)
    # p.wait();
    ofstream.close()