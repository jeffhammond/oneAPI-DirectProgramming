VENDOR      = intel
OPTIMIZE    = yes
DEBUG       = no
DEVICE      = gpu



# Standard flags
CFLAGS := -I./common -std=c++11 -Wall

# Linker flags
LDFLAGS = -lm

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

EXE = lud

$(EXE): lud.cpp ./common/common.c ./common/common.h \
	kernel_lud_diagonal.sycl \
	kernel_lud_internal.sycl \
       	kernel_lud_perimeter.sycl
	$(CC) $(KERNEL_DIM) $(CFLAGS) -o $(EXE) lud.cpp ./common/common.c $(LDFLAGS)

run: $(EXE)
	./$(EXE) -s 8192

clean:
	rm -f $(EXE)
