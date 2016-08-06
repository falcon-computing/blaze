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
CAFFE_DIR   := /curr/xuechao/tools/caffe

CFLAGS := -MMD -MP -pthread -fPIC -std=c++0x \
	  -DCAFFE_VERSION=1.0.0-rc3 \
	  -DUSE_CUDNN -DUSE_MKL

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
	   -I$(CAFFE_DIR)/.build_release/src \
	   -I$(CUDA_DIR)/include

LINK    := -L$(MANAGER_DIR)/lib -lblaze \
	   -L$(CAFFE_DIR)/build/lib -lcaffe \
		   -lpthread -lm -ldl

all: vgg.so

vgg.so: vgg.o
	$(PP) -shared -o $@ $< $(LINK) 

%.o: %.cpp
	$(PP) $(COMPILE) $< -o $@

clean:
	rm -rf *.o *.d
	rm -rf vgg.so
