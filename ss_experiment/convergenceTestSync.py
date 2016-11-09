#for different test matrices, runs the synchronous self-stabilizing iteration 
#counts the number of iterations to converge.
import sys
import subprocess
import os
# import numpy as np
# import matplotlib.pyplot as plt



NumTrials = 1000; #run for each case 100 times

faultRate = float(sys.argv[1])


baseNumIteration =0;
numIterationList = [];

for i in range(1,NumTrials):
    cmdstr = 'RSEED=%d PRINT_GRAPH=0 FAULT_PROB1=%f FAULT_PROB2=%f ../convTestSync '%(i, faultRate, faultRate)+sys.argv[2] 
    print(cmdstr)   
    p = subprocess.Popen(cmdstr, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    op, err = p.communicate()

    op = op.decode('utf-8')
    op = op.strip()
    op = op.replace("\n","")

    err = err.decode('utf-8')
    err = err.strip()
    err = err.replace("\n","")

    oparray = op.split(" ")
    tmp = int(oparray[4])/int(oparray[3])
    numIterationList.append(tmp)
    baseNumIteration = int(oparray[3])

    # print(op, err) 

# numIL = np.array(numIterationList);



#write the output into a file
inFileName = os.path.basename(sys.argv[2])
outputFileName= 'cts_'+sys.argv[1] +inFileName+'.dat'

fp = open(outputFileName,'w')

#first line contains filename Sync/Async fault_rate baseline_number_of_iteration Num_trials
outFileHeader = "%s Sync %e  %d  %d"%( inFileName, float(sys.argv[1]), baseNumIteration, NumTrials)
fp.write("%s\n"% outFileHeader)

#Now write the entries
for trial in numIterationList:
    fp.write("%.2f\n"%trial)

# close the file
fp.close()


#area of interest 1-3 10 bins
# binsSep= np.linspace(1,3,10, endpoit="True" )

# create a figure
# plt.figure(figsize=(8, 6), dpi=80);

# #create historgram 
# plt.hist(numIL,bins=binsSep)

# outputFileName= 'cts_'+sys.argv[1] +sys.argv[2]+'.pdf'

# plt.savefig(outputFileName)

