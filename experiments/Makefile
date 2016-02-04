# for testing the matrix
GRAPH_DIR := ../graphs

SHELL := /bin/bash

# MAT_SM :=  as-22july06.graph astro-ph.graph caidaRouterLevel.graph \
# 		celegans_metabolic.graph celegansneural.graph chesapeake.graph \
# 		 citationCiteseer.graph

# MAT_SM :=  astro-ph.graph  citationCiteseer.graph
# MAT_SM :=  astro-ph.graph  caidaRouterLevel.graph coAuthorsDBLP.graph ldoor.graph \
# 			delaunay_n24.graph hugebubbles-00000.graph road_central.graph hugetric-00020.graph \
# 			G_n_pin_pout.graph preferentialAttachment.graph cond-mat-2005.graph

MAT_SM :=  G_n_pin_pout.graph  astro-ph.graph  audikw1.graph   \
			 caidaRouterLevel.graph \
  		cnr-2000.graph  coAuthorsDBLP.graph  coPapersDBLP.graph  cond-mat-2005.graph \
   		delaunay_n18.graph  er-fact1.5-scale20.graph  kron_g500-simple-logn18.graph \
    	ldoor.graph  preferentialAttachment.graph  rgg_n_2_18_s0.graph  

MAT_NM := $(addprefix $(GRAPH_DIR), $(MAT_SM))

CSV_OUT := $(MAT_SM:.graph=.csv)

all: $(CSV_OUT)

.PHONY: clean

%.csv: $(GRAPH_DIR)/%.graph
	nohup ./run_exp.sh   $<  > $@ 

clean:
	rm -rf *.csv 