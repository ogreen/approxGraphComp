.PHONY:	all clean 
all: sv 

sv: main.c timer.c sv.c ft_sv.c Makefile
	$(CC) -g -O3 -std=gnu99 $(CFLAGS) $(DEFINES) -o $@ main.c timer.c sv.c ft_sv.c $(LDFLAGS) -lrt

 
clean:
	-rm -f sv
	-rm -f *.o
