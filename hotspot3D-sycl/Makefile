VENDOR      = intel
OPTIMIZE    = yes
DEBUG       = no
DEVICE      = gpu



# Standard flags
CFLAGS := -std=c++11 -Wall -I../include

# Linker flags
LDFLAGS =

# Codeplay Compiler
ifneq (,$(findstring codeplay,$(VENDOR)))
  CC = compute++
  CFLAGS += -no-serial-memop -sycl -sycl-driver
  LDFLAGS += -lComputeCpp
endif

# Intel DPC++ Compiler
ifneq (,$(findstring intel,$(VENDOR)))
  CC = dpcpp
  CFLAGS += 
  LDFLAGS += 
endif

# Debug Flags
ifeq ($(DEBUG),yes)
  CFLAGS += -g  
endif

# Optimization Flags
ifeq ($(OPTIMIZE),yes)
  CFLAGS += -O3
endif

ifeq ($(DEVICE),gpu)
  CFLAGS +=-DUSE_GPU
endif


3D: 3D.cpp 3D_helper.cpp kernel_hotspot.sycl
	$(CC) 3D.cpp 3D_helper.cpp $(CFLAGS) $(LIBS) $(LDFLAGS) -o $@

run: 3D
	./3D 512 8 5000 ../data/hotspot3D/power_512x8 ../data/hotspot3D/temp_512x8 output.out

clean:
	rm -f 3D output.out 
