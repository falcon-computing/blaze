SUBDIRS := nv_opencl \
	   xlnx_opencl \
	   altr_opencl

all: $(SUBDIRS)

release:
	make TARGET=release

package:
	make TARGET=package

clean: 
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done

$(SUBDIRS):
	$(MAKE) -C $@ $(TARGET) 

.PHONY: all clean $(SUBDIRS)
