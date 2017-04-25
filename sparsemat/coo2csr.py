

#usage: converts binary .edge file to csr file 

import sys
import numpy as np
import scipy.io as sio





# load the input file 
matfile = sys.argv[1];
mat = sio.mmread(matfile);


# symmetricize the matrix
mat = mat + mat.transpose();

# convert to csr 
mat = mat.tocsr();


# eliminate the diagonal
mat.setdiag(0);
mat.eliminate_zeros(); 

# output format fgraph
outfile = matfile.split('.')[0]+'.graph'
f = open(outfile,'w')

#writing list of list in the outfile 
mlol = [list(line.nonzero()[1]) for line in mat]

#getting dimension 
nv = mat.get_shape()[0]
nnz =  mat.getnnz();		# number of non-zeros in the sparse matrix
ne  = nnz/2;

# writing header 
header = [nv, ne, 0]; 
sheader = ' '.join(str(x) for x in header);
print(sheader,file=f)


#writing it line by line 
for ml in mlol:
	ml= (np.array(ml)+1).tolist()
	pml = ' '.join(str(x) for x in ml);
	print(pml,file=f)

#close the file
f.close()
