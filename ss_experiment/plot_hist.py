# it is ran on local machine 
import sys
import subprocess
import os
import numpy as np 
import matplotlib.pyplot as plt 

dataFile = open(sys.argv[1],'r');

#read the_header
dat = dataFile.read().splitlines();
header = dat[0]
header_list = header.split()
graphName = header_list[0].split('.')[0]
# print header_list[0]
algType = header_list[1]
# print header_list[1]
faultRate = float(header_list[2])
# print header_list[2]
# print header_list[3]
baselineIteration =int (header_list[3])
numTrials = int (header_list[4])


#create the list of iteration and make a histgram
iter = [];
numTrials = len(dat)-1

numEntry =[]

for tr in xrange(1,numTrials):
	numEntry.append(float(dat[tr].split(' ')[0]))


iterList  = np.array(numEntry);


# area of interest 1-3 10 bins
binsSep= np.linspace(.9,4,31, endpoint="True" )

# create a figure
plt.figure(figsize=(8, 6), dpi=80);

#create historgram 
plt.hist(iterList,bins=binsSep)

outputFileName= "Conv_%s_%s_%e.pdf"%(graphName,algType,faultRate)

plt.savefig(outputFileName)





