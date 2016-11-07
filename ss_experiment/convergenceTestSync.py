#for different test matrices, runs the synchronous self-stabilizing iteration 
#counts the number of iterations to converge.
import sys
import subprocess


NumTrials = 100; #run for each case 100 times

faultRate = float(sys.argv[1])




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

    print(op, err) 

