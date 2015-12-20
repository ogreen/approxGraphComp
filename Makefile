
CC := gcc 

CFLAGS := -g -O3 -std=gnu99

LOAD_FLAGS := -lrt 

SRCS := timer.c sv.c ft_sv.c faultInjection.c 

HEADERS := timer.h sv.h ft_sv.h faultInjection.h 

OBJS  := $(SRCS:.c=.o)

.PHONY:	all clean 
all: sv 

OBJS: $(SRCS) $(HEADERS)
	@$(CC) $(CFLAGS)  -c $< -o $@ 
	@echo "Compiled "$<" successfully!"

sv: main.c  $(OBJS) $(HEADERS) Makefile 
	$(CC)  $(CFLAGS) -o $@ main.c $(OBJS) $(LOAD_FLAGS)

 
clean:
	-rm -f sv $(OBJS)
	
