include ../../Makefile.config

# check all variables
ifeq ($(BOOST_DIR),)
$(error BOOST_DIR not set properly in Makefile.config)
endif
ifeq ($(PROTOBUF_DIR),)
$(error PROTOBUF_DIR not set properly in Makefile.config)
endif

MANAGER_DIR := ../../manager
CAFFE_DIR   := /curr/xuechao/tools/caffe-cpuonly
CAFFE_FCS_DIR := /curr/xuechao/prog/caffe_fcs_test

CFLAGS := -MMD -MP -pthread -fPIC -std=c++0x \
	  -DCAFFE_VERSION=1.0.0-rc3 \
	  -DUSE_CUDNN -DUSE_MKL

ifeq ($(NDEBUG),)
CFLAGS := $(CFLAGS) -g
else
CFLAGS := $(CFLAGS) -O2 -DNDEBUG
endif

COMPILE := -c $(CFLAGS) \
	   -I$(MANAGER_DIR)/include \
	   -I$(MANAGER_DIR)/src \
	   -I$(BOOST_DIR)/include \
	   -I$(PROTOBUF_DIR)/include \
	   -I$(GLOG_DIR)/include \
	   -I$(GFLAGS_DIR)/include \
	   -I$(OCV_DIR)/include \
	   -I$(CAFFE_DIR)/include \
	   -I$(CAFFE_DIR)/src \
	   -I$(CAFFE_DIR)/build/src \
	   -I$(CUDA_DIR)/include \
	   -I$(CAFFE_FCS_DIR)/examples/cpp_classification

				 
LINK    := -L$(MANAGER_DIR)/lib -lblaze \
	   -L$(CAFFE_DIR)/build/lib -lcaffe \
	   -L$(CAFFE_FCS_DIR)/examples/cpp_classification/ -lfalconMLlib \
	   -L$(OCV_DIR)/lib -lopencv_core -lopencv_highgui -lopencv_imgproc \
		   -lpthread -lm -ldl

DEPS	:= CaffeEnv.o CaffePlatform.o \
	   CaffeQueueManager.o

DST= caffe_platform.so

all: $(DST)

package: 
	cp $(DST) ../../package/nam/lib

$(DST): $(DEPS)
	$(PP) -shared -o $@ $(DEPS) $(LINK) 

%.o: %.cpp *.h
	$(PP) $(COMPILE) $< -o $@

clean:
	rm -rf *.o *.d
	rm -rf $(DST)
