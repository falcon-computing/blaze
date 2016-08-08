#!/bin/bash

SPARK_HOME=/curr/diwu/prog/blaze/spark-1.5.1
BLAZE_HOME=/curr/diwu/prog/blaze
#SPARK_HOME=/curr/xuechao/prog/blaze/spark-1.5.1
#BLAZE_HOME=/curr/xuechao/prog/blaze
MANAGER=$BLAZE_HOME/manager/bin/acc_manager

DATA=file:/curr/diwu/prog/data/mnist60k.scale
NUM_PARTITION=8
NUM_LABEL=10

HADOOP_CONF_DIR=/curr/diwu/prog/hadoop-conf-dir
YARN_CONF_DIR=$HADOOP_CONF_DIR

