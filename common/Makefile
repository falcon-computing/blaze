include ../Makefile.config

JAVA_OUT=../accrdd/src/main/java
YARN_OUT=../hadoop-2.6.0-src/hadoop-yarn-project/hadoop-yarn/fcs-yarn-common/src/main/java
PYTHON_OUT=./python

all: task.proto acc_conf.proto msgGamNam.proto
	$(PROTOBUF_DIR)/bin/protoc --java_out=$(JAVA_OUT) --python_out=$(PYTHON_OUT) task.proto
	$(PROTOBUF_DIR)/bin/protoc --python_out=$(PYTHON_OUT) acc_conf.proto
	$(PROTOBUF_DIR)/bin/protoc --java_out=$(YARN_OUT) msgGamNam.proto
