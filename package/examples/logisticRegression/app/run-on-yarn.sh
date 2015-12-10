#!/bin/bash

if [ -z $FCS_RT_ROOT ]; then
  FCS_RT_ROOT=../../..
fi
if [ ! -f "$FCS_RT_ROOT/nam/bin/nam_daemon" ]; then
  echo "FCS_RT_ROOT seems incorrect, please set it to the root directory of FCS_RUNTIME"
  exit -1
fi
SPARK_HOME=$FCS_RT_ROOT/spark-1.4.0-bin-fcs
$SPARK_HOME/bin/spark-submit --class LogisticRegression \
	--driver-memory 2G \
	--executor-memory 1G \
	--executor-cores 2 \
	--executor-accs 1 \
	--num-executors 1 \
	--master yarn-cluster \
	target/logistic-1.0.jar $@ 
