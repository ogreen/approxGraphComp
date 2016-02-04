
CC := icc  

CFLAGS := -g -O3 -std=gnu99 -fopenmp 

LOAD_FLAGS := -lrt 

SRCS := timer.c sv.c ft_sv.c faultInjection.c  stat.c 

HEADERS := timer.h sv.h ft_sv.h faultInjection.h stat.h

OBJS  := $(SRCS:.c=.o)

# for testing the matrix
GRAPH_DIR := graphs
MAT_SM :=  as-22july06.graph astro-ph.graph caidaRouterLevel.graph \
		celegans_metabolic.graph celegansneural.graph chesapeake.graph \
		 citationCiteseer.graph

.PHONY:	all clean test_small 
all: sv FaultGenerator

OBJS: $(SRCS) $(HEADERS)
	@$(CC) $(CFLAGS)  -c $< -o $@ 
	@echo "Compiled "$<" successfully!"

sv: main.c  $(OBJS) $(HEADERS) Makefile 
	$(CC)  $(CFLAGS) -o $@ main.c $(OBJS) $(LOAD_FLAGS)

FaultGenerator: FaultGen.c 
	$(CC)  $(CFLAGS) -fopenmp -o $@ FaultGen.c $(LOAD_FLAGS)

test_small: sv
	$(foreach testcase,$(MAT_SM),./sv $(GRAPH_DIR)/$(testcase);)
 
clean:
	-rm -f sv $(OBJS) FaultGenerator
	
