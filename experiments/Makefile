# for testing the matrix
GRAPH_DIR := ../graphs

SHELL := /bin/bash

# MAT_SM :=  as-22july06.graph astro-ph.graph caidaRouterLevel.graph \
# 		celegans_metabolic.graph celegansneural.graph chesapeake.graph \
# 		 citationCiteseer.graph

MAT_SM :=  astro-ph.graph  citationCiteseer.graph
MAT_NM := $(addprefix $(GRAPH_DIR), $(MAT_SM))

CSV_OUT := $(MAT_SM:.graph=.csv)

all: $(CSV_OUT)

.PHONY: clean

%.csv: $(GRAPH_DIR)/%.graph
	nohup ./run_exp.sh   $<  > $@ 
	@echo "Ran "$<" successfully!"
	echo $(MAT_NM)

clean:
	rm -rf *.csv 