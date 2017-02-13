#!/usr/bin/python3
#runs failurestest for a set of graph, given fault rate, given algorithm (sync or async) 
# this version only runs sync 

# usuage 
# python3 runExperiment.py #BIN #ALGMTYPE #NORMPROB #MAX_ITER #NUM_TRIAL  #EXPERIMENTNAME
# 		BIN = binary to run
#		#ALGMTYPE can be sync or async 
# 		#NORMPROB = normalized probability
# 		#MAX_ITER = maximum allowed iteration for LP to converge (typically 1000)
# 		#NUM_TRIAL = will try for num_trial time and estimate failure rate for each (typically 100)
# 		#EXPERIMENTNAME = name of experiment

# example: python3  runExperiment.py ../../convTestAsync async 10 1000 10

import sys
import numpy as np
import subprocess
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os
import sqlite3





#opening the  data base 
DATABASE = 'convergeExp.db'
fdb = sqlite3.connect(DATABASE)

# list of graphs to run
GRAPH_DIR = "~/graphs/"

#ch


#use following set for debugging purpose 
# GRAPHS = ["astro-ph.graph"]

# Use following set for running experiment
GRAPHS = ["kron_g500-simple-logn18.graph", "er-fact1.5-scale20.graph",  "rgg_n_2_18_s0.graph", "astro-ph.graph","cond-mat.graph","cond-mat-2005.graph" , "caidaRouterLevel.graph"]
# GRAPHS = ["astro-ph.graph","cond-mat.graph","cond-mat-2005.graph" , "caidaRouterLevel.graph"]
#"kron_g500-simple-logn20.graph" takes too long



# BIN ="../../failureTestSync"
BIN = sys.argv[1]
algmType = sys.argv[2];

print("Running experiment of type "+algmType);
fault_rate = int(sys.argv[3])
max_iter = int(sys.argv[4])
num_trials = int(sys.argv[5])
expName = sys.argv[6]

#check if the bin file exists
###




for graph in GRAPHS:
	#check if the graph file exists
	###
	gname = graph.split(".")[0]  # graph name for db

	#check if the entry exists in the database
	cursor =  fdb.cursor();
	qResult = cursor.execute('select * from convTable where \
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

		# breaking the output based on endlines
		out = out.decode("utf-8").split('\n');

		header = out[0].split();
		ffNumIter = int( header[4]);


		#add data for FISV
		fisv = out[1].split(',');
		print(fisv); 
		if len(fisv)>1:
			for i in list(range(len(fisv)-1)):
				overhead = float(fisv[i+1])
				fdb.execute('insert into convTable (graphName, algmType, normFaultRate, numTrial, \
					maxIter, ffNumIter, ftAlgmType, ohIteration )\
					values ( ?, ?, ?, ?, ?, ?, ?,?)',\
					[ gname, algmType, fault_rate, num_trials, max_iter, ffNumIter, 'fisv', overhead]);
				fdb.commit();

		

		
		sssv = out[2].split(',');
		print(sssv);
		if len(sssv)>1:
			for i in list(range(len(sssv)-1)):
				overhead = float(sssv[i+1])
				fdb.execute('insert into convTable (graphName, algmType, normFaultRate, numTrial, \
					maxIter, ffNumIter, ftAlgmType, ohIteration )\
					values ( ?, ?, ?, ?, ?, ?, ?,?)',\
					[ gname, algmType, fault_rate, num_trials, max_iter, ffNumIter, 'sssv', overhead]);
				fdb.commit();

		sshsv = out[3].split(',');
		print(sshsv);

		if len(sshsv)>1:
			for i in list(range(len(sshsv)-1)):
				overhead = float(sshsv[i+1])
				fdb.execute('insert into convTable (graphName, algmType, normFaultRate, numTrial, \
					maxIter, ffNumIter, ftAlgmType, ohIteration )\
					values ( ?, ?, ?, ?, ?, ?, ?,?)',\
					[ gname, algmType, fault_rate, num_trials, max_iter, ffNumIter, 'sshsv', overhead]);
				fdb.commit();

		tmrsv = out[4].split(',');
		print(tmrsv);
		if len(tmrsv)>1:
			for i in list(range(len(tmrsv)-1)):
				overhead = float(tmrsv[i+1])
				fdb.execute('insert into convTable (graphName, algmType, normFaultRate, numTrial, \
					maxIter, ffNumIter, ftAlgmType, ohIteration )\
					values ( ?, ?, ?, ?, ?, ?, ?,?)',\
					[ gname, algmType, fault_rate, num_trials, max_iter, ffNumIter, 'tmrsv', overhead]);
				fdb.commit();


		# algm= "sync" 

		# fdb.execute('insert into convTable (graphName, algmType, normFaultRate, numTrial, \
		# 			maxIter, numFiSvFailure, numSSSvFailure,numSSHSvFailure ,numTMRSvFailure )\
		# 			values ( ?, ?, ?, ?, ?, ?, ?, ?, ?)',\
		# 			[ gname, algmType, fault_rate, out[3], max_iter, out[4], out[5], out[6], out[7]]);
		# fdb.commit();
		print (out) 

		print (err)


#close the database 
fdb.close();
