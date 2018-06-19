PP=g++
CC=gcc

ifeq ($(FCS_RT_ROOT),)	
FCS_RT_ROOT=../../..
endif
ifeq ("$(wildcard $(FCS_RT_ROOT)/nam/bin/nam_daemon)","")
$(error FCS_RT_ROOT seems incorrect, please set it to the root directory of FCS_RUNTIME)
endif

ifeq ($(XILINX_SDX),)	
$(error XILINX_SDX not set, please set it right)
endif

SRC       := $(wildcard ./*.cpp)
HDR       := $(wildcard ./*.h)
OBJ       := $(patsubst ./%.cpp,./%.o,$(SRC))

DEPLOYMENT=hwc

ifeq ($(DEPLOYMENT),hwc)
	FPGA = huawei_vu9p
else ifeq ($(DEPLOYMENT),aws)
	FPGA = aws_vu9p
else
	FPGA = ku115
endif

ifeq ($(FPGA),aws_vu9p)
	PMM_FLAGS = -D DIE_NUM=3 -D BANKID0=3 -D BANKID1=1 -D BANKID2=2
else ifeq ($(FPGA),huawei_vu9p)
	PMM_FLAGS = -D DIE_NUM=3 -D BANKID0=0 -D BANKID1=1 -D BANKID2=3
else
	PMM_FLAGS = -D DIE_NUM=2 -D BANKID0=1 -D BANKID1=2 -D BANKID2=3
endif

CFLAGS	:= -fPIC -std=c++0x -m64 -O3 -march=native $(PMM_FLAGS)
COMPILE	:= $(CFLAGS) \
	   -I. \
	   -I../cpu_task \
	   -I$(FCS_RT_ROOT)/include \
	   -I$(FCS_RT_ROOT)/extern/boost_1_55_0/include \
	   -I$(FCS_RT_ROOT)/extern/googletools/include \
	   -I$(XILINX_SDX)/runtime/include/1_2 -I$(XILINX_SDX)/Vivado_HLS/include

XLNXLINK    := -lpthread -lm -ldl -lrt -lstdc++ -L$(XILINX_SDX)/runtime/lib/x86_64 -lxilinxopencl 

ACC 	:= PairhmmACC 

DST = $(addsuffix .so, $(ACC))

all: $(DST)

%.so: $(OBJ)
	$(PP) -shared $(XLNXLINK) $(OBJ) -o $@

%.o: %.cpp $(HDR)
	$(CC) -c $(COMPILE) $< -o $@

clean:
	rm -rf $(OBJ) $(DST) 
