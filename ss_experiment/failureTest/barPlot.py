import numpy as np
import matplotlib.pyplot as plt


def barPlot(df):
	"""
	Takes a list of list and returns a bar plot
	input = [[label1, label2, label3 ...], [v11, v12, v13 ...], [v21, v22, v23 ...]]
	length of each inner list is same;
	Label is x axis, vi is Y axis
	"""

	# Terminology 
	# groupDim: Group Dimension i.e. number of data points in a group
	# numGroups: Number of groups i.e. number of label  

	# calculating data related parameters
	groupDim = len(df) -1;
	numGroups = len(df[0]);


	#calculating/defining  plot related parameter
	width = 0.2 	# for groupDim=4 ~ 0.25 and 0.05 for spacing 

	index = np.arange(numGroups);



	#plot proprties 
	plt.style.use('bmh')


	#create a figure 
	fig, ax = plt.subplots(figsize=(8,6));	

	# for loop for each data frame 
	for di in list(range(groupDim)):

		plt.bar(index+width*di,
		#using df['pre_score'] data,
		df[di+1],
		# of width
		width,
		# with alpha 0.5
		alpha=0.5,
		# with color
		color='#EE3224',
		)


	plt.show();