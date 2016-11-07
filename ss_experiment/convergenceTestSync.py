#for different test matrices, runs the synchronous self-stabilizing iteration 
#counts the number of iterations to converge.
import sys


NumTrials = 100; #run for each case 100 times

faultRate = float(sys.argv[1])




for i in xrange(1,NumTrials):
	cmdstr = ('RSEED=%d PRINT_GRAPH=0 FAULT_PROB1=%f FAULT_PROB2=%f ../convTestSync'%(i, faultRate, faultRate)+sys.argv[2] 
	# print(cmdstr)
	f= sys.popen(str)



	
