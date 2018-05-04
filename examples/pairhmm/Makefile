PP=g++
CC=gcc

ifeq ($(FCS_RT_ROOT),)	
FCS_RT_ROOT=../../..
endif
ifeq ("$(wildcard $(FCS_RT_ROOT)/nam/bin/nam_daemon)","")
$(error FCS_RT_ROOT seems incorrect, please set it to the root directory of FCS_RUNTIME)
endif

SRC       := $(wildcard ./*.cpp)
HDR       := $(wildcard ./*.h)
OBJ       := $(patsubst ./%.cpp,./%.o,$(SRC))

CFLAGS	:= -fPIC -std=c++0x -m64 -O3 -march=native -D DIE_NUM=2
COMPILE	:= $(CFLAGS) \
	   -I. \
	   -I../cpu_task \
	   -I$(FCS_RT_ROOT)/include \
	   -I$(FCS_RT_ROOT)/extern/boost_1_55_0/include \
	   -I$(FCS_RT_ROOT)/extern/googletools/include \
	   -I$(XILINX_SDX)/runtime/include/1_2 -I$(XILINX_SDX)/Vivado_HLS/include

XLNXLINK    := -lpthread -lm -ldl -lrt -lstdc++ -L$(XILINX_SDX)/runtime/lib/x86_64 -lxilinxopencl 

ACC 	:= PairhmmTest 

DST = $(addsuffix .so, $(ACC))

all: $(DST)

%.so: $(OBJ)
	$(PP) -shared $(XLNXLINK) $(OBJ) -o $@

%.o: %.cpp $(HDR)
	$(CC) -c $(COMPILE) $< -o $@

clean:
	rm -rf $(OBJ) $(DST) 
