#!/bin/bash

source config.sh
export HADOOP_CONF_DIR

#if [ -z $FCS_RT_ROOT ]; then
#  FCS_RT_ROOT=../../..
#fi
#if [ ! -f "$FCS_RT_ROOT/nam/bin/nam_daemon" ]; then
#  echo "FCS_RT_ROOT seems incorrect, please set it to the root directory of FCS_RUNTIME"
#  exit -1
#fi
FCS_RT_ROOT=/curr/diwu/prog/fcs_runtime
SPARK_HOME=$FCS_RT_ROOT/spark-1.5.1-bin-fcs
$SPARK_HOME/bin/spark-submit --class TestApp \
		--driver-memory 8G \
		--executor-memory 8G \
		--executor-cores 1 \
		--num-executors 1 \
		--conf spark.app.name="caffe" \
		--conf spark.yarn.executor.nodeLabelExpression="VGG-16" \
		--master yarn-cluster \
		target/caffecppclassification-1.0.jar $@

