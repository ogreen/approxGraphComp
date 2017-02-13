import numpy as np
import matplotlib.pyplot as plt

# Create a figure of size 8x6 inches, 80 dots per inch
plt.figure(figsize=(8, 6), dpi=80)

# plot style 
plt.style.use('ggplot')

# Create a new subplot from a grid of 1x1
plt.subplot(1, 1, 1)

X = np.linspace(-np.pi, np.pi, 256, endpoint=True)
# C, S = np.cos(X), np.sin(X)





# number of lines 
n = 5

# C = [];
phi = np.linspace(-np.pi, np.pi, n, endpoint=True)

# generating list of colors 
# color_list = plt.cm.Set3(np.linspace(0, 1, n))
# color_list = plt.cm.Dark2(np.linspace(0, 1, n))
# color_list = plt.cm.Accent(np.linspace(0, 1, n))
color_list=['#66c2a5','#fc8d62','#8da0cb','#e78ac3','#a6d854']
color_list= ['#e41a1c','#377eb8','#4daf4a','#984ea3','#ff7f00']

for i in list(range(n)):
	C = np.sin(X + 2*np.pi*i/n)
	plt.plot(X, C, color=color_list[i], linewidth=2.0, linestyle="-")
	


# Plot cosine with a blue continuous line of width 1 (pixels)
# plt.plot(X, C, color="blue", linewidth=1.0, linestyle="-")

# Plot sine with a green continuous line of width 1 (pixels)
# plt.plot(X, S, color="green", linewidth=1.0, linestyle="-")

# Set x limits
plt.xlim(-4.0, 4.0)

# Set x ticks
plt.xticks(np.linspace(-4, 4, 9, endpoint=True))

# Set y limits
plt.ylim(-1.0, 1.0)

# Set y ticks
plt.yticks(np.linspace(-1, 1, 5, endpoint=True))

# Save figure using 72 dots per inch
# plt.savefig("exercice_2.png", dpi=72)

# Show result on screen
plt.show()