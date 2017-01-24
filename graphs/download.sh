wget http://www.cc.gatech.edu/dimacs10/archive/data/coauthor/coAuthorsDBLP.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/coAuthorsDBLP.graph



wget http://www.cc.gatech.edu/dimacs10/archive/data/random/rgg_n_2_18_s0.graph.bz2
bunzip2 rgg_n_2_18_s0.graph.bz2

wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/G_n_pin_pout.graph.bz2
bunzip2 G_n_pin_pout.graph.bz2

wget http://www.cc.gatech.edu/dimacs10/archive/data/numerical/NACA0015.graph.bz2
bunzip2 NACA0015.graph.bz2

wget http://www.cc.gatech.edu/dimacs10/archive/data/er/er-fact1.5-scale20.graph.bz2
bunzip2 er-fact1.5-scale20.graph.bz2

wget http://www.cc.gatech.edu/dimacs10/archive/data/numerical/packing-500x100x100-b050.graph.bz2
bunzip2 packing-500x100x100-b050.graph.bz2


wget http://www.cc.gatech.edu/dimacs10/archive/data/streets/belgium.osm.graph.bz2
bunzip2 belgium.osm.graph.bz2


wget http://www.cc.gatech.edu/dimacs10/archive/data/kronecker/kron_g500-simple-logn18.graph.bz2
bunzip2 kron_g500-simple-logn18.graph.bz2


wget http://www.cc.gatech.edu/dimacs10/archive/data/kronecker/kron_g500-simple-logn20.graph.bz2
bunzip2 kron_g500-simple-logn20.graph.bz2

wget http://www.cc.gatech.edu/dimacs10/archive/data/delaunay/delaunay_n18.graph.bz2
bunzip2 delaunay_n18.graph.bz2

scp condesa.cc.gt.atl.ga.us:/home/piyush/approxGraphComp/graphs/caidaRouterLevel.graph  .
scp condesa.cc.gt.atl.ga.us:/home/piyush/approxGraphComp/graphs/astro-ph.graph  .
scp condesa.cc.gt.atl.ga.us:/home/piyush/approxGraphComp/graphs/cnr-2000.graph  .
scp condesa.cc.gt.atl.ga.us:/home/piyush/approxGraphComp/graphs/cond-mat-2005.graph  .

wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/preferentialAttachment.graph.bz2
bunzip2 preferentialAttachment.graph.bz2
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/smallworld.graph.bz2
bunzip2 smallworld.graph.bz2
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/uk-2002.graph.bz2
bunzip2 uk-2002.graph.bz2
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/uk-2007-05.graph.bz2
bunzip2 uk-2007-05.graph.bz2



# NUmber of Components in kron_g500-simple-logn18 = 262130
# NUmber of Components in 5-scale20 = 0 
# NUmber of Components in G_n_pin_pout = 67336
# NUmber of Components in coAuthorsDBLP = 0
# NUmber of Components in rgg_n_2_18_s0 = 130427
# NUmber of Components in packing-500x100x100-b050 = 1516838 note : takes too long, avoid 
# NUmber of Components in uk-2002 = 18520485 . takes too long avoid
# NUmber of Components in as-22july06 = 0

wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/astro-ph.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/astro-ph.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/cond-mat.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/cond-mat.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/cond-mat-2005.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/cond-mat-2005.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/cnr-2000.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/cnr-2000.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/eu-2005.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/eu-2005.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/in-2004.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/in-2004.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/road_central.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/road_central.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/clustering/caidaRouterLevel.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/caidaRouterLevel.graph


# NUmber of Components in astro-ph = 16701 
# NUmber of Components in cond-mat = 16721
# NUmber of Components in cond-mat-2005 = 40382
# NUmber of Components in cnr-2000 = 0 
# NUmber of Components in cnr-2000 = 0 
# NUmber of Components in in-2004 = 1382907 
# NUmber of Components in eu-2005 = 0 
# NUmber of Components in caidaRouterLevel = 192190


wget http://www.cc.gatech.edu/dimacs10/archive/data/walshaw/144.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/144.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/walshaw/wave.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/wave.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/walshaw/m14b.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/m14b.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/walshaw/fe_rotor.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/fe_rotor.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/walshaw/598a.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/598a.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/walshaw/brack2.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/brack2.graph
wget http://www.cc.gatech.edu/dimacs10/archive/data/walshaw/bcsstk32.graph.bz2
PRINT_GRAPH=0 MAX_ITER=1000 NUM_TRIAL=10 NORM_PROB=14 ./sv ~/graphs/bcsstk32.graph

# NUmber of Components in 144 = 0 
# NUmber of Components in wave = 0 
# NUmber of Components in m14b = 0 
# NUmber of Components in fe_rotor = 0 
# NUmber of Components in 598a = 0 
# NUmber of Components in brack2 = 0 
# NUmber of Components in bcsstk32 = 0
# NUmber of Components in auto = 0 
