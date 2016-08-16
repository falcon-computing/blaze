#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/Makefile.config

export BLAZE_HOME=$DIR
export SPARK_HOME=$DIR/spark-1.5.1
export MAVEN=/curr/diwu/tools/apache-maven-3.3.9
export LD_LIBRARY_PATH=$BOOST_DIR/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$PROTOBUF_DIR/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$GLOG_DIR/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$GFLAGS_DIR/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$HADOOP_DIR/lib/native:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$JAVA_HOME/jre/lib/amd64/server:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$MKL_DIR/lib/intel64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$BLAZE_HOME/manager/lib:$LD_LIBRARY_PATH
export PATH=$PROTOBUF_DIR/bin:$PATH
export PATH=$MAVEN/bin:$PATH
