

import pandas as pd
import numpy as np
import sys
import matplotlib.pyplot as plt
import json
import glob

from six import iteritems
from matplotlib.lines import Line2D
# unfilled_markers = [m for m, func in iteritems(Line2D.markers)
#                     if func != 'nothing' and m not in Line2D.filled_markers]
# fmarkers = sorted(unfilled_markers,
#                           key=lambda x: (str(type(x)), str(x)))[::-1]
fmarkers=Line2D.filled_markers

plt.style.use('ggplot')


def plotter(frame, filename,graph,ylabel):
 
	fig, axes = plt.subplots()
    
	fig.set_size_inches(16, 8)
	plt.gcf().subplots_adjust(bottom=0.1)

	sysdf = frame[frame['graph'] == graph]

	uniqueImpl= sysdf['fault-rate'].unique();
	print uniqueImpl

	for id1, uniq in enumerate(uniqueImpl):
		sysdata = sysdf[sysdf['fault-rate'] == uniq]
	
		plotdata = sysdata['sv-plain'];
		index = np.arange(len(plotdata))+1

		print index 
		print plotdata.tolist()

		# if (True):
		# 	continue

		plt.plot(index, plotdata,  markersize=13) 

		# plt.rc('xtick', labelsize=18) 
		# plt.xlabel('Iterations',fontsize=18)
		plt.ylabel(ylabel,fontsize=18)    
		# plt.legend( loc=0,    ncol=2, mode="expand", borderaxespad=0., fontsize=18)    
		plt.tight_layout()
		plt.yticks(fontsize=18)
		plt.ylim(bottom=0)
		# plt.xlim(left=1-0.1,right=len(sysdata)+0.1)

		plt.xticks(index,index,fontsize=18)
	plt.savefig(filename, format="pdf",bbox_inches='tight');
	plt.close()


#columns=['graph-name', 'language', 'vertices', 'edges', 'intersections','ba-faster','bb-time', 'ba-time', 'bac-time', 'ba-faster-ratio', 'bac-faster-ratio','dummy']
graphLabelRotation = 60

graphList = ['caidaRouterLevel'] #["coAuthorsDBLP","astro-ph", "smallworld", "preferentialAttachment"]
#df = pd.read_table("bc/"+bcsystems[0]+"/"+ bcGraphList[0] +".log",index_col=None)

counterColumns=["iteration","graph", "fault-rate","sv-plain","new-alg-fault-free", "error-correction","detection-and-correction","field1","field2","field3","field4","field5"]

print graphList
list_ = []    
for graph in graphList:
	df = pd.read_csv("../experiments/"+ graph +".csv",index_col=None,skipinitialspace=True)
	list_.append(df)
    
frame = pd.concat(list_, keys=None)
frame.columns=counterColumns


if (True):
	for graph in graphList:
		plotter(frame, "output.pdf",graph,"Time")
