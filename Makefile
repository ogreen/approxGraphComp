
CC := icc  
# CC := gcc  
CXX := icpc 

CFLAGS := -g -O3 -std=gnu99 -fopenmp 

LOAD_FLAGS := -lrt -lm

CSRCS := timer.c sv.c sssv.c ft_sv.c faultInjection.c  stat.c graph.c 

# CPPSRCS := sssv.c 

HEADERS := timer.h sv.h ft_sv.h faultInjection.h stat.h graph.h

COBJS  := $(CSRCS:.c=.o)

# CPPOBJS  := $(CPPSRCS:.cpp=.o)

OBJS := $(COBJS) 
# for testing the matrix
GRAPH_DIR := graphs
MAT_SM :=  as-22july06.graph astro-ph.graph caidaRouterLevel.graph \
		celegans_metabolic.graph celegansneural.graph chesapeake.graph \
		 citationCiteseer.graph

.PHONY:	all clean test_small 
#all: sv FaultGenerator selfStab_test 
all: sv selfStab_test convTestSync

COBJS: $(CSRCS) $(HEADERS)
	@$(CC) $(CFLAGS)  -c $< -o $@ 
	@echo "Compiled "$<" successfully!"


CPPOBJS: $(CPPSRCS) $(HEADERS)
	@$(CXX) $(CFLAGS)  -c $< -o $@ 
	@echo "Compiled "$<" successfully!"

sv: main.c  $(OBJS) $(HEADERS) Makefile 
	$(CC)  $(CFLAGS) -o $@ main.c $(OBJS) $(LOAD_FLAGS)

selfStab_test: selfStab_test.c $(OBJS) $(HEADERS) Makefile 
	$(CC)  $(CFLAGS) -o $@ selfStab_test.c $(OBJS) $(LOAD_FLAGS)

convTestSync: convTestSync.c $(OBJS) $(HEADERS) Makefile 
	$(CC)  $(CFLAGS) -o $@ convTestSync.c $(OBJS) $(LOAD_FLAGS)

# sssv: sssv.c  $(COBJS) $(HEADERS) Makefile 
# 	$(CC)  $(CFLAGS) -o $@ sssv.c $(COBJS) $(LOAD_FLAGS)


#FaultGenerator: FaultGen.c 
#	$(CC)  $(CFLAGS) -fopenmp -o $@ FaultGen.c $(LOAD_FLAGS)

test_small: sv
	$(foreach testcase,$(MAT_SM),./sv $(GRAPH_DIR)/$(testcase);)
 
clean:
	-rm -f sv $(OBJS)  FaultGenerator
	
