#!/usr/bin/python3
#   produces a bar graph chart for sync failure tests
#   uses graphs described by variable GRAPH
#   uses database described by variable DATABASE
# 
# Usage 
# python printFailureTest.py #algmtype #normprob #max_iter #num_trial 
#algmtype is for type of algorithm used can be sync or async (all lower)
# normprob = normalized probability
# #max iter = maximum allowed iteration for LP to converge (typically 1000)
# num_trial = will try for num_trial time and estimate failure rate for each (typically 100)
# example: python3  printFailureTest.py 10 1000 10
# example : python3  printFailureTest.py sync 10 1000 10

import sys
import numpy as np
import pandas as pd
import subprocess
import matplotlib
matplotlib.use('Agg')
matplotlib.rcParams['backend'] = "Qt4Agg"
import matplotlib.pyplot as plt
import os
import sqlite3
from matplotlib import cm


# assumes the data is saved in the database 


# modify following list of graph to choose which ones do you want 
GRAPHS = ["kron_g500-simple-logn18", "rgg_n_2_18_s0",\
     "astro-ph","cond-mat","cond-mat-2005" , "caidaRouterLevel"]
GRAPHS  = GRAPHS + ["kron_g500-logn18", "polblogs"]
GRAPHS = GRAPHS + ["Wordnet3", "patents_main", "email-EuAll", "soc-sign-epinions", "web-Google", "web-Stanford", "cit-HepTh", "webbase-1M"]
GRAPHS = GRAPHS + ["amazon0505", "web-BerkStan", "mouse_gene", "human_gene1", "bips07_2476", "bauru5727"]


GraphPrint = ["kron_g(500,18)", "rgg(2,18)",\
     "astro-ph","cond-mat","cond-mat-2005" , "caidaRouterLevel"]
GraphPrint = GraphPrint + ["kron_(500,18)", "polblogs"]
GraphPrint = GraphPrint + ["Wordnet3", "patents_main", "email-EuAll", "soc-sign-epinions", "web-Google", "web-Stanford", "cit-HepTh", "webbase-1M"]
GraphPrint = GraphPrint + ["amazon0505", "web-BerkStan", "mouse_gene", "human_gene1", "bips07_2476", "bauru5727"]


# get the experiment parameters
algmType = sys.argv[1];

print("Printing for experiments of type "+algmType);
fault_rate = int(sys.argv[2])
max_iter = int(sys.argv[3])
num_trials = int(sys.argv[4])



#opening the  data base 
DATABASE = 'failureExp.db'
fdb = sqlite3.connect(DATABASE)

# get the data from sqlite database 
sqlCmd= "select * from fTable where graphName in ({seq}) \
            and algmType=? and normFaultRate=? and numTrial=? and maxIter=? \
            ".format(seq=','.join(['?']*len(GRAPHS)))
sqlArg=  GRAPHS + [algmType, fault_rate, num_trials, max_iter]

print(sqlCmd);
print(sqlArg);
qResults = fdb.execute(sqlCmd, sqlArg);


qResTuple = qResults.fetchall()

graphName = [ row[1] for row in qResTuple ];
FiSV =      [ int(row[6]) for row in qResTuple ];
SsSV =      [ int(row[7]) for row in qResTuple ];
SshSV =     [ int(row[8]) for row in qResTuple ];
TmrSV =     [ int(row[9]) for row in qResTuple ];

print(FiSV);
print(SsSV);
print(SshSV);
print(TmrSV);

N = len(graphName);
ind = 1+np.arange(N)  # the x locations for the groups
width = 0.166       # the width of the bars

# plt.style.use('ggplot')
# plt.style.use('fivethirtyeight')
# plt.style.use('bmh')
plt.style.use('seaborn-darkgrid')
# fig, ax = plt.subplots()
fig = plt.figure()

ax =  fig.add_axes([0.1, 0.15, .8, 0.7])

color_list= ['#e41a1c','#377eb8','#4daf4a','#984ea3','#ff7f00']
color_list=['#66c2a5','#fc8d62','#8da0cb','#e78ac3','#a6d854']
color_list = plt.cm.Set3(np.linspace(0, 1, 5))
color_list = plt.cm.Accent(np.linspace(0, 1, 5))
color_list = ['#1b9e77','#d95f02','#7570b3','#e7298a']
rects1 = ax.bar(-2*width+ind, 100-100*np.array(FiSV)/num_trials, width, label="FiSV",\
	color=color_list[0], edgecolor='white', linewidth='2')
rects2 = ax.bar(-2*width+ind+width, 100-100*np.array(SsSV)/num_trials, width,label="SsSV",\
	 color=color_list[1],edgecolor='white', linewidth='2')
rects3 = ax.bar(-2*width+ind+2*width, 100-100*np.array(SshSV)/num_trials, width, label="SsHSV",\
	color=color_list[2],edgecolor='white', linewidth='2')
rects4 = ax.bar(-2*width+ind+3*width, 100-100*np.array(TmrSV)/num_trials, width, label="TmrSV",\
	color=color_list[3],edgecolor='white', linewidth='2')


# adding legend
plt.legend(loc='upper left',ncol=4, fontsize=12)


# setting x and y axis
ax.set_ylim(-5, 110)

# x label and ylabel
plt.ylabel('Success Rate', fontsize=14) 
plt.xlabel('Test Networks', fontsize=14) 

#title
title = "Success rate for fault rate= $2^{-%d}|E|$ bitflips per iteration "%fault_rate
plt.title(title,fontsize=14);

#setting X ticks 
plt.xticks(1+2*width+np.array(list(range(N))))
ax.set_xticklabels(GraphPrint, rotation=15, ha='right', fontsize=12)
          

outfile = "failureTest_"+algmType+"_%d.pdf"%fault_rate

plt.savefig(outfile)
plt.show()  




# gname = os.path.basename(fname).split(".")[0] 
# print("Running for graphs: " + gname)
# # command
# algo_types = ['FI', 'SS', 'SSH', 'TM']  
# fault_rates = range(9, 16)

# npdata = np.zeros((len(fault_rates), len(algo_types) + 1))

# i = 0
# for fault_rate in fault_rates:
#     cmd = "PRINT_GRAPH=0 MAX_ITER=%s NUM_TRIAL=%d NORM_PROB=%s ../../failureTestSync %s"%(max_iter, num_trials, fault_rate, fname)

#     p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
#     op,err = p.communicate()

#     op = op.decode("utf-8")
#     op = op.split("\n")
#     op = [o.strip() for o in op]
#     print op
#     op2 = op[1].split()

#     npdata[i,0] = fault_rate
#     npdata[i,-1] = int(op2[-1])
#     npdata[i,-2] = int(op2[-2])
#     npdata[i,-3] = int(op2[-3])
#     npdata[i,-4] = int(op2[-4])

#     i+= 1

# output = pd.DataFrame(npdata)

# output.columns = ['fault_rate'] + algo_types
# output.to_csv(gname+'_fault_rate.csv')


# fig, ax = plt.subplots()
# ind = np.arange(len(fault_rates))
# width = 0.2

# rects1 = ax.bar(ind, 100*output['FI']/num_trials, width, color='r')
# rects2 = ax.bar([i + width for i in ind], 100*output['SS']/num_trials, width, color='g')
# rects3 = ax.bar([i + 2*width for i in ind], 100*output['SSH']/num_trials, width, color='b')
# rects4 = ax.bar([i + 3*width for i in ind], 100*output['TM']/num_trials, width, color='purple')

# ax.set_ylabel('Failures %')
# ax.set_ylim(0, 110)
# ax.set_title(gname + 'Failure Rate')
# ax.set_xticks(ind + 2*width)
# ax.set_xticklabels(['2^-'+str(fr) for fr in fault_rates])

# ax.legend((rects1[0], rects2[0], rects3[0], rects4[0]), ('FI', 'SS', 'SSH', 'TM'))

# plt.savefig('')

