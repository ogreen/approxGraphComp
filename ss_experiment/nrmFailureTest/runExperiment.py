#!/usr/bin/python3
#runs failurestest for a set of graph, given fault rate, given algorithm (sync or async) 
# this version only runs sync 

# usuage 
# python runExperiment.py #bin #algmtype #normprob #max_iter #num_trial 
# bin = binary to run
# normprob = normalized probability
# #max iter = maximum allowed iteration for LP to converge (typically 1000)
# num_trial = will try for num_trial time and estimate failure rate for each (typically 100)
# #algmtype can be sync or async 
# example: python3  runExperiment.py ../../failureTestSync sync 10 1000 10
# example: python3  runExperiment.py ../../failureTestSync async 10 1000 10
# example:  python3  runExperiment.py ../../normFailureTestAsync async 0.5 100 100

import sys
import numpy as np
import subprocess
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os
import sqlite3





#opening the  data base 
DATABASE = 'nrmfTable.db'
fdb = sqlite3.connect(DATABASE)

# list of graphs to run
GRAPH_DIR = "~/graphs/"
GRAPHS = ["astro-ph.graph", "kron_g500-simple-logn18.graph", "er-fact1.5-scale20.graph",  "rgg_n_2_18_s0.graph", "astro-ph.graph","cond-mat.graph","cond-mat-2005.graph" , "caidaRouterLevel.graph"]
GRAPHS  = GRAPHS + ["kron_g500-logn18.graph", "polblogs.graph"]
GRAPHS = GRAPHS + ["Wordnet3.graph", "patents_main.graph", "email-EuAll.graph", "soc-sign-epinions.graph", "web-Google.graph", "web-Stanford.graph", "cit-HepTh.graph", "webbase-1M.graph"]
GRAPHS = GRAPHS + ["amazon0505.graph", "web-BerkStan.graph", "mouse_gene.graph", "human_gene1.graph", "bips07_2476.graph", "bauru5727.graph"]

# GRAPHS = ["astro-ph.graph","cond-mat.graph","cond-mat-2005.graph" , "caidaRouterLevel.graph"]
#"kron_g500-simple-logn20.graph" takes too long
# fname = sys.argv[1]
# BIN ="../../failureTestSync"
BIN = sys.argv[1]
algmType = sys.argv[2];

print("Running experiment of type "+algmType);
targetFailRate = float(sys.argv[3])
max_iter = int(sys.argv[4])
num_trials = int(sys.argv[5])

#check if the bin file exists
###



# targetFailRate, tmrFailRate, normFaultRate, numTrial, maxIter, experimentName

# targetFailRate float not null,   -- the desired fail rate for TMR
#   tmrFailRate float not null,      -- the measured fail rate for TMR when sampled
#   numTrial integer not null, 
#   normFaultRate integer not null,   -- the value of n corresponding to failure rate
#   maxIter integer,

experimentName = "first"

for graph in GRAPHS:
	#check if the graph file exists
	###
	gname = graph.split(".")[0]  # graph name for db

	#check if the entry exists in the database
	cursor =  fdb.cursor();
	qResult = cursor.execute('select * from nrmfTable where \
		graphName=? and algmType=? and targetFailRate=? and numTrial=? and maxIter=? and expName=?',\
		[gname, algmType, targetFailRate, num_trials, max_iter, experimentName]);
	numQResult= len(qResult.fetchall());
	if numQResult>0:
		print ("Record for %s already exists: Skipping" % (gname) );
	else:
		cmd = "MAX_ITER=%d NUM_TRIAL=%d TMR_FR=%f %s %s "%(max_iter, num_trials, targetFailRate, BIN, (GRAPH_DIR+graph) )
		print ("Running....."+cmd)
		p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE)
		out, err = p.communicate(); 
		out = out.decode("utf-8").split();

		# algm= "sync" 

		fdb.execute('insert into nrmfTable (graphName, algmType, targetFailRate, tmrFailRate, normFaultRate, numTrial, \
					maxIter, numFiSvFailure, numSSSvFailure,numSSHSvFailure ,numTMRSvFailure, expName )\
					values ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)',\
					[ gname, algmType, targetFailRate, out[1], out[2], num_trials, max_iter, out[8], out[9], out[10], out[11], experimentName]);
		fdb.commit();
		print (out) 
		print (err)


#close the database 
fdb.close();
