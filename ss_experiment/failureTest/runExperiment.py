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
# example:  python3  runExperiment.py ../../failureTestAsync async 12 100 100

import sys
import numpy as np
import subprocess
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os
import sqlite3





#opening the  data base 
DATABASE = 'failureExp.db'
fdb = sqlite3.connect(DATABASE)

# list of graphs to run
GRAPH_DIR = "~/graphs/"
GRAPHS = ["kron_g500-simple-logn18.graph", "er-fact1.5-scale20.graph",  "rgg_n_2_18_s0.graph", "astro-ph.graph","cond-mat.graph","cond-mat-2005.graph" , "caidaRouterLevel.graph"]
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
fault_rate = int(sys.argv[3])
max_iter = int(sys.argv[4])
num_trials = int(sys.argv[5])

#check if the bin file exists
###




for graph in GRAPHS:
	#check if the graph file exists
	###
	gname = graph.split(".")[0]  # graph name for db

	#check if the entry exists in the database
	cursor =  fdb.cursor();
	qResult = cursor.execute('select * from fTable where \
		graphName=? and algmType=? and normFaultRate=? and numTrial=? and maxIter=?',\
		[gname, algmType, fault_rate, num_trials, max_iter]);
	numQResult= len(qResult.fetchall());
	if numQResult>0:
		print ("Record for %s already exists: Skipping" % (gname) );
	else:
		cmd = "PRINT_GRAPH=0 MAX_ITER=%d NUM_TRIAL=%d NORM_PROB=%d %s %s "%(max_iter, num_trials, fault_rate, BIN, (GRAPH_DIR+graph) )
		print ("Running....."+cmd)
		p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE)
		out, err = p.communicate(); 
		out = out.decode("utf-8").split();

		# algm= "sync" 

		fdb.execute('insert into fTable (graphName, algmType, normFaultRate, numTrial, \
					maxIter, numFiSvFailure, numSSSvFailure,numSSHSvFailure ,numTMRSvFailure )\
					values ( ?, ?, ?, ?, ?, ?, ?, ?, ?)',\
					[ gname, algmType, fault_rate, out[3], max_iter, out[4], out[5], out[6], out[7]]);
		fdb.commit();
		print (out) 
		print (err)


#close the database 
fdb.close();
