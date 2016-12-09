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
graphName = header_list[0]
algType = header_list[1]
normFaultRate = float(header_list[2])
# % fault rate is 2^{-normFaultRate}*|E| every iteration
numTrials = int (header_list[3])
baselineIteration =int (header_list[4])


# create a figure
plt.figure(figsize=(8, 6), dpi=80);

# colors for plotting
#create historgram 
color_list = plt.cm.gnuplot2(np.linspace(0, 1, 6))
imp=[]

for c in range(1,5):
	numEntry =[]
	datc = dat[c];
	dat_list = datc.split(', ');
	imp.append(dat_list[0]);
	numSuccess = len(dat_list)-1;
	for i in xrange(1,numSuccess-1):
		numEntry.append(float(dat_list[i]))

	#changing into numpy object
	iterList  = np.array(numEntry);

	# normalizing for number of trials
	iterList = 100*iterList/numTrials;
	#changing overheads into percentage
	# print imp[c-1]+"Hello" 
	if dat_list[0]=="TMSV ":
		iterList = 300*(iterList+1)-100;
	else:
		iterList = 100*iterList;

	# area of interest 1-3 10 bins
	binsSep= np.linspace(0,1000,101, endpoint="True" )



	[hist, edge] =  np.histogram(iterList, binsSep[0:101]);
	# plt.hist(iterList1,bins=binsSep)
	hist = np.cumsum(hist)
	# plt.plot(edge[1:100],hist);

	plt.plot(binsSep[0:100], hist, color=color_list[c-1], linewidth=2.5, linestyle="--")



ax = plt.gca()
for label in ax.get_xticklabels() + ax.get_yticklabels():
    label.set_fontsize(14)
    
plt.xlim(0,250)
plt.ylim(-10,150)
plt.grid()
plt.legend(imp)
plt.title('Overhead w.r.t. fault free execution (%)', fontsize=14)
plt.xlabel('Overhead', fontsize=14)
plt.ylabel('Cumulative Distribution (%)', fontsize=14)


#for each implementation get the name of implementation and overhead incurred
# numEntry1 =[]
# dat1 = dat[1];
# dat1_list = dat1.split(', ');
# imp1 = dat1_list[0];
# numSuccess1 = len(dat1)-1;
# for i in xrange(1,numSuccess1):
# 	numEntry1.append(float(dat1_list[i]))




# numEntry3 =[]
# dat3 = dat[3];
# dat3_list = dat3.split(', ');
# imp3 = dat3_list[0];
# numSuccess3 = len(dat3)-1;
# for i in xrange(1,numSuccess3):
# 	numEntry3.append(float(dat3_list[i]))

# numEntry4 =[]
# dat4 = dat[4];
# dat4_list = dat4.split(', ');
# imp4 = dat4_list[0];
# numSuccess4 = len(dat4)-1;
# for i in xrange(1,numSuccess4):
# 	numEntry4.append(float(dat4_list[i]))



# iterList1  = np.array(numEntry2);

# iterList1 = 100*iterList1;

# # area of interest 1-3 10 bins
# binsSep= np.linspace(0,1000,101, endpoint="True" )



# [hist, edge] =  np.histogram(iterList1, binsSep[0:101]);
# # plt.hist(iterList1,bins=binsSep)
# hist = np.cumsum(hist)
# # plt.plot(edge[1:100],hist);
# c = 1
# plt.plot(binsSep[0:100], hist, color=color_list[c], linewidth=2.5, linestyle="--")



# plt.xlabel('\% overhead');
# plt.ylabel('Frequency');

outputFileName= "Conv_%s_%s_2^{-%d}.pdf"%(graphName,algType,normFaultRate)

plt.savefig(outputFileName)




# Convert those into numpy array 

#create the list of iteration and make a histgram
# iter = [];
# numTrials = len(dat)-1

# numEntry =[]

# for tr in xrange(1,numTrials):
# 	numEntry.append(float(dat[tr].split(' ')[0]))


# iterList  = np.array(numEntry);


# # area of interest 1-3 10 bins
# binsSep= np.linspace(.9,4,31, endpoint="True" )

# # create a figure
# plt.figure(figsize=(8, 6), dpi=80);

# #create historgram 
# plt.hist(iterList,bins=binsSep)

# outputFileName= "Conv_%s_%s_%e.pdf"%(graphName,algType,normFaultRate)

# plt.savefig(outputFileName)





