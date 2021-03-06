#===============================================================================
# User Options
#===============================================================================

# Compiler can be set below, or via environment variable
CC        = icpc
OPTIMIZE  = yes
DEBUG     = no
DEVICE    = gpu

#===============================================================================
# Program name & source code list
#===============================================================================

program = nn

source = nearestNeighbor.cpp utils.cpp 

obj = $(source:.cpp=.o)

#===============================================================================
# Sets Flags
#===============================================================================

# Standard Flags
CFLAGS := -std=c++14 -Wall

# Linker Flags
LDFLAGS = 

# Debug Flags
ifeq ($(DEBUG),yes)
  CFLAGS += -g
  LDFLAGS  += -g
endif


# Optimization Flags
ifeq ($(OPTIMIZE),yes)
  CFLAGS += -O3
endif

ifeq ($(DEVICE),gpu)
  CFLAGS +=-qnextgen -fiopenmp -fopenmp-targets=spir64 -D__STRICT_ANSI__
else
  CFLAGS +=-qopenmp
endif
#===============================================================================
# Targets to Build
#===============================================================================

$(program): $(obj) Makefile
	$(CC) $(CFLAGS) $(obj) -o $@ $(LDFLAGS)

utils.o: utils.cpp utils.h  
	$(CC) $(CFLAGS) -c $< -o $@

nearestNeighbor.o : nearestNeighbor.cpp nearestNeighbor.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(program) $(obj)

run: $(program)
	./$(program) filelist.txt -r 5 -lat 30 -lng 90  -t

