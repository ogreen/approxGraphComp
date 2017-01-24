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
algtype = sys.argv[4]

if (str.lower(algtype) == 'sync'):
    algtype = 'sync'
    binary = 'failureTestSync'
elif (str.lower(algtype) == 'async'):
    algtype = 'async'
    binary = 'failureTestAsync'

gname = os.path.basename(fname).split(".")[0] 
print("Running for graphs: " + gname)
# command
algo_types = ['FI', 'SS', 'SSH', 'TM']  
fault_rates = range(9, 16)

npdata = np.zeros((len(fault_rates), len(algo_types) + 1))

i = 0
for fault_rate in fault_rates:
    cmd = "PRINT_GRAPH=0 MAX_ITER=%s NUM_TRIAL=%d NORM_PROB=%s ../../%s %s"%(max_iter, num_trials, fault_rate, binary, fname)
    print(cmd)
    p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    op,err = p.communicate()

    op = op.decode("utf-8")
    print(op)
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
output.to_csv(gname+"_failure_test_"+algtype+'_maxiter_'+max_iter+'.csv')
