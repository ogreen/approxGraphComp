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
# example : python3  printTime.py async async1

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
# GRAPHS = ["kron_g500-simple-logn18", "rgg_n_2_18_s0",\
#      "astro-ph","cond-mat","cond-mat-2005" , "caidaRouterLevel"]

# GRAPHS  = GRAPHS + ["kron_g500-logn18", "polblogs"]
# GRAPHS = GRAPHS + ["Wordnet3", "patents_main", "email-EuAll", "soc-sign-epinions", "web-Google", "web-Stanford", "cit-HepTh", "webbase-1M"]
# GRAPHS = GRAPHS + ["amazon0505", "web-BerkStan", "mouse_gene", "human_gene1", "bips07_2476", "bauru5727"]



# finally chosen graph
GRAPHS = ["kron_g500-simple-logn18","astro-ph","cond-mat","rgg_n_2_18_s0",\
			"caidaRouterLevel", "Wordnet3", "patents_main", "web-Google",\
			"cit-HepTh","web-BerkStan", "mouse_gene"]

GraphPrint = ["kron_simple(500,18)", "rgg(2,18)",\
     "astro-ph","cond-mat","cond-mat-2005" , "caidaRouterLevel"]

GraphPrint = GraphPrint + ["kron_(500,18)", "polblogs"]
GraphPrint = GraphPrint + ["Wordnet3", "patents_main", "email-EuAll", "soc-sign-epinions", "web-Google", "web-Stanford", "cit-HepTh", "webbase-1M"]
GraphPrint = GraphPrint + ["amazon0505", "web-BerkStan", "mouse_gene", "human_gene1", "bips07_2476", "bauru5727" ]

# get the experiment parameters
algmType = sys.argv[1];

print("Printing for experiments of type "+algmType);
experimentName = sys.argv[2]


ftAlgmType = 'sssv'

#opening the  data base 
DATABASE = 'timeTable.db'
fdb = sqlite3.connect(DATABASE)

# get the data from sqlite database 
sqlCmd= "select * from tTable where graphName in ({seq}) \
            and algmType=? and experimentName=? and ftAlgmType=? \
            ".format(seq=','.join(['?']*len(GRAPHS)))
sqlArg=  GRAPHS + [algmType, experimentName, ftAlgmType]

print(sqlCmd);
print(sqlArg);
qResults = fdb.execute(sqlCmd, sqlArg);


qResTuple = qResults.fetchall()

graphName = [ row[1] for row in qResTuple ];

ssTime =  np.array([ row[6] for row in qResTuple ]);
lpTime =  np.array([ row[7] for row in qResTuple ]);

print(lpTime);
print(ssTime);

# relOverhead = np.divide(ssTime, lpTime); 
relOverhead = ssTime/lpTime; 
print(relOverhead)
N = len(graphName);
ind = 1+np.arange(N)  # the x locations for the groups
width = 0.5       # the width of the bars

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

rects1 = ax.bar( ind-.25, 100*relOverhead, width, label="FiSV",\
	color=color_list[0], edgecolor='white', linewidth='2')


# adding legend
plt.legend(loc='upper left',ncol=4, fontsize=12)


# setting x and y axis
ax.set_ylim(-5, 110)

# x label and ylabel
plt.ylabel('Relative overhead of Self-stabilization', fontsize=14) 
plt.xlabel('Test Networks', fontsize=14) 

#title
title = "Overhead of Self-stabilization in Fault-free execution "
plt.title(title,fontsize=14);

#setting X ticks 
plt.xticks(ind-0.25)
ax.set_xticklabels(graphName, rotation=15, ha='right', fontsize=12)
          



plt.savefig('output.pdf')
plt.show()  



