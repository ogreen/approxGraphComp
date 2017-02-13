#!/usr/bin/python3
#runs failurestest for a set of graph, given fault rate, given algorithm (sync or async) 
# this version only runs sync 

# usuage 
# python runExperiment.py #bin #algmtype #experimentname
# bin = binary to run
# #algmtype can be sync or async 
# experimentname a name to distinuish between different experiment
# #max iter = maximum allowed iteration for LP to converge (typically 1000)
# num_trial = will try for num_trial time and estimate failure rate for each (typically 100)


# example: python3  runExperiment.py ../../timeAsync async async1

import sys
import numpy as np
import subprocess
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os
import sqlite3





#opening the  data base 
DATABASE = 'timeTable.db'
fdb = sqlite3.connect(DATABASE)

# list of graphs to run
GRAPH_DIR = "~/graphs/"
GRAPHS = ["kron_g500-simple-logn18.graph", "er-fact1.5-scale20.graph",  "rgg_n_2_18_s0.graph", "astro-ph.graph","cond-mat.graph","cond-mat-2005.graph" , "caidaRouterLevel.graph"]
# GRAPHS = ["astro-ph.graph","cond-mat.graph","cond-mat-2005.graph" , "caidaRouterLevel.graph"]
#"kron_g500-simple-logn20.graph" takes too long
# fname = sys.argv[1]
# BIN ="../../failureTestSync"
BIN = sys.argv[1]
algmType = sys.argv[2];
experimentName = sys.argv[3];

print("Running time experiment for the type "+algmType);

#check if the bin file exists
###


ftAlgmType = 'sssv'

for graph in GRAPHS:
	#check if the graph file exists
	###
	gname = graph.split(".")[0]  # graph name for db

	#check if the entry exists in the database
	cursor =  fdb.cursor();
	qResult = cursor.execute('select * from tTable where \
		graphName=? and algmType=? and experimentName=? and ftAlgmType=? ',\
		[gname, algmType, experimentName, ftAlgmType]);
	numQResult= len(qResult.fetchall());
	if numQResult>0:
		print ("Record for %s already exists: Skipping" % (gname) );
	else:
		cmd = "PRINT_GRAPH=0 %s %s "%(BIN, (GRAPH_DIR+graph) )
		print ("Running....."+cmd)
		p = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE)
		out, err = p.communicate(); 
		out = out.decode("utf-8").split();

		lpTime = float(out[1]);
		ssTime = float(out[2]);
		# algm= "sync" 

		fdb.execute('insert into tTable (graphName, algmType, experimentName, ftAlgmType, lpTime, ssTime )\
					values ( ?, ?, ?, ?, ?, ?)',\
					[ gname, algmType, experimentName, ftAlgmType, lpTime, ssTime] );
		fdb.commit();
		print (out) 
		print (err)


#close the database 
fdb.close();
