include ../../../Makefile.config

# check all variables
ifeq ($(BOOST_DIR),)
$(error BOOST_DIR not set properly in Makefile.config)
endif
ifeq ($(PROTOBUF_DIR),)
$(error PROTOBUF_DIR not set properly in Makefile.config)
endif


MANAGER_DIR := ../../../manager
PLATFORM_DIR := ../../../platforms/caffe
CAFFE_DIR   := /curr/xuechao/prog/caffe_fcs
SDACCEL_DIR := /curr/software/Xilinx/SDAccel/2015.4

PP = $(SDACCEL_DIR)/lnx64/tools/gcc/bin/gcc

CFLAGS := -MMD -MP -pthread -fPIC -std=c++0x \
	  -DCAFFE_VERSION=1.0.0-rc3 \
	  -DUSE_MKL -DCPU_ONLY
#	  -DUSE_CUDNN -DUSE_MKL

CFLAGS += -D FPGA_DEVICE -D C_KERNEL

ifeq ($(NDEBUG),)
CFLAGS := $(CFLAGS) -g
else
CFLAGS := $(CFLAGS) -O2 -DNDEBUG
endif

COMPILE := -c $(CFLAGS) \
	   -I$(MANAGER_DIR)/src \
	   -I$(PLATFORM_DIR) \
	   -I$(BOOST_DIR)/include \
	   -I$(PROTOBUF_DIR)/include \
	   -I$(GLOG_DIR)/include \
	   -I$(GFLAGS_DIR)/include \
	   -I$(CAFFE_DIR)/include \
	   -I$(CAFFE_DIR)/src \
	   -I$(CAFFE_DIR)/.build_debug/src \
	   -I$(OCV_DIR)/include
#  -I$(CUDA_DIR)/include

COMPILE += -I$(CAFFE_DIR)/fcs/examples/cpp_classification \
		   -I$(SDACCEL_DIR)/runtime/include/1_2 \
		   -I$(SDACCEL_DIR)/Vivado_HLS/include


LINK    := -L$(MANAGER_DIR)/lib -lblaze \
	   -L$(CAFFE_DIR)/build/lib -lcaffe \
	   -L$(OCV_DIR)/lib -lopencv_core -lopencv_highgui -lopencv_imgproc \
	   -lpthread -lm -ldl

LINK += -L/curr/hanhu/sdaccel_library/pkg_ku3_2015.4_2ddr/pcie -L$(SDACCEL_DIR)/runtime/lib/x86_64 -lxilinxopencl

all: classification.so

classification.so: classification.o
	$(PP) -shared -o $@ $< $(LINK) 

%.o: %.cpp
	$(PP) $(COMPILE) $< -o $@

clean:
	rm -rf *.o *.d
	rm -rf classification.so
